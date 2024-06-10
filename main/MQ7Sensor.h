#ifndef MQ7SENSOR_H
#define MQ7SENSOR_H

#include "Button.h"
#include "LED.h"
#include <Arduino.h>

class MQ7Sensor {
private:
  void print_sensor();
  void cycle_voltage();

public:
  MQ7Sensor(int pinIN, int pinMOSFET, int pinBuzzer, int maxVoltage, int bitRes,
            void (*handle_reading)(int));
  int pinIN;
  int pinMOSFET;
  int pinBuzzer;

  int maxVoltage;
  int bitRes;
  void (*handle_reading)(int);
  void tick();
  void take_reading();

  // last recorded PPM, can be max 2.5 minutes old
  float ppm;
};

#endif
