## ESP_DoubleResetDetector

[![arduino-library-badge](https://www.ardu-badge.com/badge/ESP_DoubleResetDetector.svg?)](https://www.ardu-badge.com/ESP_DoubleResetDetector)

### Releases v1.0.2

1. Fix bug by left-over cpp file.
2. Fix bug in example.
3. Enhance README.md

This library is based on, modified, bug-fixed and improved from [`DataCute`](https://github.com/datacute/DoubleResetDetector) to add support for ESP32.
 
Using this library to detect a double reset, using

1. RTC Memory, EEPROM or SPIFFS for ESP8266
2. EEPROM and SPIFFS for ESP32.

## Prerequisite
1. [`Arduino IDE 1.8.12 or later` for Arduino](https://www.arduino.cc/en/Main/Software)
2. [`ESP32 core 1.0.4 or later`](https://github.com/espressif/arduino-esp32/releases) for ESP32 (Use Arduino Board Manager)
3. [`ESP8266 core 2.6.3 or later`](https://github.com/esp8266/Arduino/releases) for ES82662 (Use Arduino Board Manager)

### Quick Start

#### Installing use Arduino Library Manager

1. The easiest way is to use `Arduino Library Manager`. Search for `ESP_DoubleResetDetector`, then select / install the latest version.
2. More detailed instructions at [![arduino-library-badge](https://www.ardu-badge.com/badge/ESP_DoubleResetDetector.svg?)](https://www.ardu-badge.com/ESP_DoubleResetDetector)

#### Manual Install

1. Navigate to [ESP_DoubleResetDetector](https://github.com/khoih-prog/ESP_DoubleResetDetector) page.
2. Download the latest release `ESP_DoubleResetDetector-master.zip`.
3. Extract the zip file to `ESP_DoubleResetDetector-master` directory 
4. Copy whole 
  - `ESP_DoubleResetDetector-master` folder to Arduino libraries directory such as `~/Arduino/libraries`.

### Releases

#### Releases v1.0.2

1. Fix bug by left-over cpp file.
2. Fix bug in example.
3. Enhance README.md

#### Releases v1.0.1

1. Add EEPROM and SPIFFS support, besides RTC memory, for ESP8266
2. Add SPIFFS support, besides EEPROM, for ESP32

#### PURPOSE:

Detects a double reset so that an alternative start-up mode can be used. One example use is to allow re-configuration of a device's wifi credentials.

#### Usage

How to use

```cpp
// These defines must be put before #include <ESP_DoubleResetDetector.h>
// to select where to store DoubleResetDetector's variable.
// For ESP32, You must select one to be true (EEPROM or SPIFFS)
// For ESP8266, You must select one to be true (RTC, EEPROM or SPIFFS)
// Otherwise, library will use default EEPROM storage
#define ESP_DRD_USE_EEPROM      false
#define ESP_DRD_USE_SPIFFS      true    //false

#ifdef ESP8266
  #define ESP8266_DRD_USE_RTC     false   //true
#endif

#define DOUBLERESETDETECTOR_DEBUG       true  //false

#include <ESP_DoubleResetDetector.h>      //https://github.com/khoih-prog/ESP_DoubleResetDetector

// Number of seconds after reset during which a 
// subseqent reset will be considered a double reset.
#define DRD_TIMEOUT 10

// RTC Memory Address for the DoubleResetDetector to use
#define DRD_ADDRESS 0

DoubleResetDetector* drd;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  
  Serial.begin(115200);
  Serial.println("\Starting");
  
  drd = new DoubleResetDetector(DRD_TIMEOUT, DRD_ADDRESS);

  if (drd->detectDoubleReset()) 
  {
    Serial.println("Double Reset Detected");
    digitalWrite(LED_BUILTIN, LOW);
  } 
  else 
  {
    Serial.println("No Double Reset Detected");
    digitalWrite(LED_BUILTIN, HIGH);
  }
}

void loop()
{
  // Call the double reset detector loop method every so often,
  // so that it can recognise when the timeout expires.
  // You can also call drd.stop() when you wish to no longer
  // consider the next reset as a double reset.
  drd->loop();
}
```

Also see examples: 
1. [ConfigOnDoubleReset](examples/ConfigOnDoubleReset)
2. [minimal](examples/minimal)

### TO DO

1. Search for bug and improvement.
2. Similar features for Arduino (UNO, Mega, etc...)

### Contributions and thanks

1. Thanks to [zobix](https://github.com/zobix) for report the bug in [Isssue 2](https://github.com/khoih-prog/ESP_DoubleResetDetector/issues/2)

### Contributing
If you want to contribute to this project:
- Report bugs and errors
- Ask for enhancements
- Create issues and pull requests
- Tell other people about this library

### Copyright
Copyright 2019- Khoi Hoang
