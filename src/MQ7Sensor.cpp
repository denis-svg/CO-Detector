#include "MQ7Sensor.h"

MQ7Sensor::MQ7Sensor(int pinIN, int pinMOSFET, int pinButton, int pinBuzzer,
                     int pinLED, int maxVoltage, int bitRes)
    : pinIN(pinIN), pinMOSFET(pinMOSFET), pinLED(pinLED), pinButton(pinButton),
      pinBuzzer(pinBuzzer), maxVoltage(maxVoltage), bitRes(bitRes) {
  pinMode(pinIN, INPUT);
  pinMode(pinMOSFET, OUTPUT);
  pinMode(pinLED, OUTPUT);
  pinMode(pinBuzzer, OUTPUT);
  pinMode(pinButton, INPUT_PULLUP);
}

void MQ7Sensor::print_sensor() {
  static unsigned long prev_t = 0;

  unsigned long t = millis();
  if (t - prev_t < 500)
    return;

  int sensorValue = analogRead(pinIN);
  Serial.print("sensor:");
  Serial.println(sensorValue);
  prev_t = t;
}

void MQ7Sensor::cycle_voltage() {
  static bool high = false;
  static unsigned long prev_t = 0;

  unsigned long t = millis();
  if ((high && (t - prev_t < 60000)) || (!high && (t - prev_t < 90000)))
    return;
  high = !high;
  digitalWrite(pinMOSFET, high ? HIGH : LOW);
  Serial.print("sensor:");
  Serial.println(1023); // draw a line on the plot to show transition

  if (high) { // just transitioned to HIGH, last value shows true PPM
    // TODO compute ppm from sensor voltage
    ppm = analogRead(pinIN);
  }

  prev_t = t;
}

void MQ7Sensor::tick() {
  cycle_voltage();
  print_sensor();

  if (digitalRead(pinButton) == LOW) {
    tone(pinBuzzer, 2000);
    digitalWrite(pinLED, HIGH);
  } else {
    noTone(pinBuzzer);
    digitalWrite(pinLED, LOW);
  }
}
