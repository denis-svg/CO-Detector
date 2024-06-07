#include "MQ7Sensor.h"

#define LOW_DURATION 90000
#define HIGH_DURATION 60000

MQ7Sensor::MQ7Sensor(int pinIN, int pinMOSFET, Button btn, int pinBuzzer,
                     int pinLED, int maxVoltage, int bitRes, void (*alert)(int))
    : pinIN(pinIN), pinMOSFET(pinMOSFET), btn(btn), pinBuzzer(pinBuzzer),
      pinLED(pinLED), maxVoltage(maxVoltage), bitRes(bitRes), alert(alert) {}

void MQ7Sensor::print_sensor()
{
  static unsigned long prev_t = 0;

  unsigned long t = millis();
  if (t - prev_t < 500)
    return;

  int sensorValue = analogRead(pinIN);
  Serial.print(">sensor:");
  Serial.println(sensorValue);
  prev_t = t;
}

void MQ7Sensor::cycle_voltage() {
  static bool high = false;
  static unsigned long prev_t = 0;

  unsigned long t = millis();
  if ((high && (t - prev_t < LOW_DURATION)) ||
      (!high && (t - prev_t < HIGH_DURATION)))
    return;
  high = !high;

  digitalWrite(pinMOSFET, high ? HIGH : LOW);
  Serial.print(">sensor:");
  Serial.print("Transitioned to cycle: ");
  Serial.println(high ? "low (1.5V)" : "high (5V)");

  if (high) { // just transitioned to HIGH, last value shows true PPM
    float VL = analogRead(pinIN);
    // formula extracted from the datasheet by Sparkfun:
    // https://learn.sparkfun.com/tutorials/hazardous-gas-monitor
    ppm = 3.027 * exp(1.0698 * (VL * maxVoltage / bitRes));
    Serial.print(">ppm:");
    Serial.println(ppm);

    if (ppm >= WARNING_PPM) {
      Serial.println("high ppm ");
      tone(pinBuzzer, 2000);
      digitalWrite(pinLED, HIGH);
      alert(ppm);
    } else {
      noTone(pinBuzzer);
      digitalWrite(pinLED, LOW);
    }
  }

  prev_t = t;
}

void MQ7Sensor::tick() {
  cycle_voltage();
  print_sensor();
  btn.poll();

  if (btn.pressed) {
    noTone(pinBuzzer);
    digitalWrite(pinLED, LOW);
  }
}
