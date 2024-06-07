#ifndef MQ7SENSOR_H
#define MQ7SENSOR_H

#include "Button.h"
#include <Arduino.h>

class MQ7Sensor {
private:
  void print_sensor();
  void cycle_voltage();

public:
  MQ7Sensor(int pinIN, int pinMOSFET, Button btn, int pinBuzzer, int pinLED,
            int maxVoltage, int bitRes, void (*alert)(int));
  int pinIN;
  int pinMOSFET;
  int pinButton;
  int pinBuzzer;
  int pinLED;
  Button btn;

  int maxVoltage;
  int bitRes;
  void (*alert)(int);
  void tick();
  // last recorded PPM, can be max 2.5 minutes old
  float ppm;
  // duration in minutes to silence the buzzer for
  // after pressing the button when the alarm was on
  int silencedDuration = 5;
  const int WARNING_PPM = 99999;
};

#endif
