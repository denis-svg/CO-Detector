#include "MQ7Sensor.h"

#define LOW_DURATION 90000
#define HIGH_DURATION 60000
// #define LOW_DURATION 9000
// #define HIGH_DURATION 6000

MQ7Sensor::MQ7Sensor(int pinIN, int pinMOSFET, int pinButton, int pinBuzzer,
                     int pinLED, int maxVoltage, int bitRes, void (*alert)(int))
    : pinIN(pinIN), pinMOSFET(pinMOSFET), pinButton(pinButton),
      pinBuzzer(pinBuzzer), pinLED(pinLED), maxVoltage(maxVoltage),
      bitRes(bitRes), alert(alert) {
  pinMode(pinIN, INPUT);
  pinMode(pinMOSFET, OUTPUT);
  pinMode(pinLED, OUTPUT);
  pinMode(pinBuzzer, OUTPUT);
  pinMode(pinButton, INPUT_PULLUP);
}

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

void MQ7Sensor::cycle_voltage()
{
  static bool high = false;
  static unsigned long prev_t = 0;

  unsigned long t = millis();
  if ((high && (t - prev_t < LOW_DURATION)) || (!high && (t - prev_t < HIGH_DURATION)))
    return;
  high = !high;

  digitalWrite(pinMOSFET, high ? HIGH : LOW);
  Serial.print(">sensor:");
  Serial.println(1023); // draw a line on the plot to show transition
  Serial.print("Transitioned to cycle: ");
  Serial.println(high);

  if (high)
  { // just transitioned to HIGH, last value shows true PPM
    // TODO compute ppm from sensor voltage
    ppm = analogRead(pinIN);

    if (ppm >= WARNING_PPM)
    {
      Serial.println("high ppm ");
      tone(pinBuzzer, 2000);
      digitalWrite(pinLED, HIGH);
      alert(ppm);
    }
    else
    {
      noTone(pinBuzzer);
      digitalWrite(pinLED, LOW);
    }
  }

  prev_t = t;
}

void MQ7Sensor::tick() {
  cycle_voltage();
  print_sensor();

  if (digitalRead(pinButton) == LOW) {
    noTone(pinBuzzer);
    digitalWrite(pinLED, LOW);
  }
}
