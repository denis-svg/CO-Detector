#include "Button.h"

void Button::poll() {
  stateJustChanged = false;
  prev_down = curr_down;
  curr_down = !digitalRead(pin);

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
