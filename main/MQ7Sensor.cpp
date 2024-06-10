#include "MQ7Sensor.h"

#define LOW_DURATION 90000
#define HIGH_DURATION 60000

MQ7Sensor::MQ7Sensor(int pinIN, int pinMOSFET, int pinBuzzer, int maxVoltage,
                     int bitRes, void (*handle_reading)(int))
    : pinIN(pinIN), pinMOSFET(pinMOSFET), pinBuzzer(pinBuzzer),
      maxVoltage(maxVoltage), bitRes(bitRes), handle_reading(handle_reading) {
  pinMode(pinIN, INPUT);
  pinMode(pinMOSFET, OUTPUT);
  pinMode(pinBuzzer, OUTPUT);
}

void MQ7Sensor::print_sensor() {
  static unsigned long prev_t = 0;

  unsigned long t = millis();
  if (t - prev_t < 500)
    return;

  int sensorValue = analogRead(pinIN);
  Serial.print(">sensor:");
  Serial.println(sensorValue);
  prev_t = t;
}

void MQ7Sensor::take_reading() {
  float VL = analogRead(pinIN);
  // formula extracted from the datasheet by Sparkfun:
  // https://learn.sparkfun.com/tutorials/hazardous-gas-monitor
  ppm = 3.027 * exp(1.0698 * (VL * maxVoltage / bitRes));
  handle_reading(ppm);
}

void MQ7Sensor::cycle_voltage() {
  static bool high = true;
  static unsigned long prev_t = 0;

  unsigned long t = millis();
  if ((high && (t - prev_t < LOW_DURATION)) ||
      (!high && (t - prev_t < HIGH_DURATION)))
    return;
  high = !high;

  digitalWrite(pinMOSFET, high ? HIGH : LOW);
  Serial.print("Transitioned to cycle: ");
  Serial.println(high ? "low (1.5V)" : "high (5V)");

  if (high) { // just transitioned to HIGH, last value shows true PPM
    // it's important we take a reading while the sensor is receiving 5V,
    // and since the sensor pin and heater pin are connected in our case,
    // it's best we take a reading at the start of the HIGH cycle
    take_reading();
  }

  prev_t = t;
}

void MQ7Sensor::tick() {
  cycle_voltage();
  print_sensor();
}
