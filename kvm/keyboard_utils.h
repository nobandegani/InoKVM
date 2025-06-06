#pragma once

#include <Arduino.h>
#include "USBHIDKeyboard.h"


class KeyboardUtils {
  public:
    USBHIDKeyboard Keyboard;

    void setup();

    void press(uint8_t key);

    void releaseAll(uint32_t InDelay);

    uint8_t parse_key(String input);
};