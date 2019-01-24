#include <Arduino.h>
#include "Button.h"

// Not much too this ...

Button::Button(byte pin) {
  // Constructor
  // Takes an int as pin to waatch
  pinMode (_buttonPin, INPUT_PULLUP);
}

boolean Button::isClosed(void) {
  // If low (connected to GND) switch closed
  if (digitalRead (_buttonPin) == LOW) {
    return true;
  }
  return false;  
}
