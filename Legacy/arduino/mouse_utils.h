#pragma once

#include <Arduino.h>
#include "USBHIDMouse.h"

class MouseUtils {
  public:
    USBHIDMouse Mouse;

    bool random_move = false;

    void setup();

    void move(int InX, int InY);
    void move2(int8_t InX, int8_t InY);

    void mouse_click(String event);

    void loop();

  private:
    void rand_mouse_move();
};