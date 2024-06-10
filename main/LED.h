#pragma once

#include <Arduino.h>

class LED {
private:
  int pin;

public:
  enum Mode { steady, flickering, pulsating };

  Mode mode = steady;
  bool on = false;
  int Tp = 2000; // pulsation period measured in ms
  int Tf = 100;  // flicker period measured in ms

  LED(int pin) : pin{pin} { pinMode(pin, OUTPUT); };
  void toggle(bool state);
  void tick();
};
