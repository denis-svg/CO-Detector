#pragma once
#include <Arduino.h>

class Button {
private:
  bool prev_down = false;
  bool curr_down = false; // button state only changes when curr == prev

public:
  bool pressed = false;
  bool stateJustChanged = false;
  int pin;

  Button(int pin) : pin{pin} { pinMode(pin, INPUT_PULLUP); };

  void poll();
};
