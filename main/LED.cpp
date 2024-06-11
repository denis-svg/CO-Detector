#include "LED.h"

void LED::toggle(bool state) {
  if (state == on)
    return;

  on = state;
  if (mode == steady || mode == flickering) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, state ? HIGH : LOW);
  }
}

void LED::tick() {
  if (mode == pulsating) {
    unsigned long t = millis();
    float x = (t % Tp) / 1000.0;
    float hertz = 1000.0 / Tp; // pulses per second
    float intensity = (sin(x * 2 * PI * hertz) + 1.0) / 2.0;
    float V = intensity * 255;
    analogWrite(pin, V);
  } else if (mode == flickering) {
    unsigned long t = millis();
    bool s_on = (t / (Tf / 2)) % 2 == 1;
    this->toggle(s_on);
  }
}
