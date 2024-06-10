#include "Button.h"

void Button::poll() {
  stateJustChanged = false;
  prev_down = curr_down;
  pinMode(pin, INPUT_PULLUP);
  curr_down = !digitalRead(pin);

  // TODO fix input pullup on pin 25 https://www.esp32.com/viewtopic.php?t=17431

  // If a button has been in the same state the last two ticks,
  // we assume it has settled and isn't bouncing anymore.
  // Otherwise, it will probably settle in the next call.
  //
  // For this to work as debouncing, this function should be called
  // more often than 5ms (bounce duration)
  // but less often than 20ms (click duration).
  if (prev_down == curr_down) {
    stateJustChanged = pressed != curr_down;
    pressed = curr_down;
  }
}
