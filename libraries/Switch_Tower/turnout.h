#include "Arduino.h"

#ifndef Turnout_h
#define Turnout_h

struct turnout {
  uint16_t min_pos = 300;
  uint16_t max_pos = 700;
  uint16_t current_pos;
  uint16_t target_pos;
};

#endif
