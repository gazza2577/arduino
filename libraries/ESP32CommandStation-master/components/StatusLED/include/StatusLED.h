/**********************************************************************
ESP32 COMMAND STATION

COPYRIGHT (c) 2019-2020 Mike Dunston

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses
**********************************************************************/

#ifndef STATUS_LED_H_
#define STATUS_LED_H_

#include <esp_event.h>
#include <executor/Service.hxx>
#include <executor/StateFlow.hxx>
#include <NeoPixelBrightnessBus.h>
#include <utils/constants.hxx>
#include <utils/Singleton.hxx>

#include "sdkconfig.h"

#ifndef CONFIG_STATUS_LED_UPDATE_INTERVAL_MSEC
#define CONFIG_STATUS_LED_UPDATE_INTERVAL_MSEC 450
#endif

#if !defined(CONFIG_STATUS_LED) || defined(CONFIG_STATUS_LED_COLOR_RGB)
#define NEO_COLOR_TYPE RgbColor
#define NEO_COLOR_MODE NeoRgbFeature
#define NEO_COLOR_MODE_NAME "RGB"
#elif defined(CONFIG_STATUS_LED_COLOR_GRB)
#define NEO_COLOR_MODE NeoGrbFeature
#define NEO_COLOR_MODE_NAME "GRB"
#elif defined(CONFIG_STATUS_LED_COLOR_RGBW)
#define NEO_COLOR_MODE NeoRgbwFeature
#define NEO_COLOR_TYPE RgbwColor
#define NEO_COLOR_MODE_NAME "RGBW"
#elif defined(CONFIG_STATUS_LED_COLOR_GRBW)
#define NEO_COLOR_MODE NeoGrbwFeature
#define NEO_COLOR_TYPE RgbwColor
#define NEO_COLOR_MODE_NAME "GRBW"
#elif defined(CONFIG_STATUS_LED_COLOR_BRG)
#define NEO_COLOR_TYPE RgbColor
#define NEO_COLOR_MODE NeoBrgFeature
#define NEO_COLOR_MODE_NAME "BRG"
#elif defined(CONFIG_STATUS_LED_COLOR_RBG)
#define NEO_COLOR_TYPE RgbColor
#define NEO_COLOR_MODE NeoRbgFeature
#define NEO_COLOR_MODE_NAME "RBG"
#else
#error "StatusLED: unknown LED color order"
#endif

#if !defined(CONFIG_STATUS_LED) || defined(CONFIG_STATUS_LED_TYPE_WS2811)
#define NEO_METHOD NeoEsp32Rmt6Ws2811Method
#define NEO_METHOD_NAME "RMT(6)-Ws2811"
#elif defined(CONFIG_STATUS_LED_TYPE_WS281X)
#define NEO_METHOD NeoEsp32Rmt6Ws2812xMethod
#define NEO_METHOD_NAME "RMT(6)-Ws2812"
#elif defined(CONFIG_STATUS_LED_TYPE_WS281X_800K)
#define NEO_METHOD NeoEsp32Rmt6800KbpsMethod
#define NEO_METHOD_NAME "RMT(6)-Ws2812-800kbps"
#elif defined(CONFIG_STATUS_LED_TYPE_WS281X_400K)
#define NEO_METHOD NeoEsp32Rmt6400KbpsMethod
#define NEO_METHOD_NAME "RMT(6)-Ws2812-400kbps"
#elif defined(CONFIG_STATUS_LED_TYPE_SK6812) || defined(CONFIG_STATUS_LED_TYPE_LC6812)
#define NEO_METHOD NeoEsp32Rmt6Sk6812Method
#define NEO_METHOD_NAME "RMT(6)-sk6812"
#elif defined(CONFIG_STATUS_LED_TYPE_APA106)
#define NEO_METHOD NeoEsp32Rmt6Apa106Method
#define NEO_METHOD_NAME "RMT(6)-APA106"
#else
#error "StatusLED: unknown LED type"
#endif

class StatusLED : public StateFlowBase, public Singleton<StatusLED>
{
public:
  enum COLOR : uint8_t
  {
    OFF
  , RED
  , GREEN
  , YELLOW
  , BLUE
  , RED_BLINK
  , GREEN_BLINK
  , BLUE_BLINK
  , YELLOW_BLINK
  };

  enum LED : uint8_t
  {
    WIFI,
    OPS_TRACK,
    PROG_TRACK,
    EXT_1,
    EXT_2,
    MAX_LED
  };

  StatusLED(Service *service)
    : StateFlowBase(service)
    , bus_(nullptr)
    , updateInterval_(MSEC_TO_NSEC(CONFIG_STATUS_LED_UPDATE_INTERVAL_MSEC))
  {
    for(int index = 0; index < LED::MAX_LED; index++)
    {
      colors_[index] = RGB_OFF_;
      state_[index] = false;
    }
#if CONFIG_STATUS_LED
    start_flow(STATE(init));
#endif // CONFIG_STATUS_LED
  }

  void stop()
  {
    set_terminated();
    timer_.ensure_triggered();
  }

  void setStatusLED(const LED, const COLOR, const bool=false);

  void wifi_event(system_event_t *);
private:
  StateFlowTimer timer_{this};
  std::unique_ptr<NeoPixelBrightnessBus<NEO_COLOR_MODE, NEO_METHOD>> bus_;
  NEO_COLOR_TYPE colors_[LED::MAX_LED];
  bool state_[LED::MAX_LED];
  const uint64_t updateInterval_;

  NEO_COLOR_TYPE RGB_RED_{NEO_COLOR_TYPE(255, 0, 0)};
  NEO_COLOR_TYPE RGB_GREEN_{NEO_COLOR_TYPE(0, 255, 0)};
  NEO_COLOR_TYPE RGB_YELLOW_{NEO_COLOR_TYPE(255, 255, 0)};
  NEO_COLOR_TYPE RGB_BLUE_{NEO_COLOR_TYPE(0, 0, 255)};
  NEO_COLOR_TYPE RGB_OFF_{NEO_COLOR_TYPE(0)};

  STATE_FLOW_STATE(init);
  STATE_FLOW_STATE(update);
  STATE_FLOW_STATE(update_bus);
};

#endif // STATUS_LED_H_