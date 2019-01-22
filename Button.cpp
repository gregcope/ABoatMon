#include <Arduino.h>
#include "Button.h"

Button::Button(byte pin) {
  // Constructor
  // Takes an int as pin to power up
  pinMode (_buttonPin, INPUT_PULLUP);
}

boolean Button::isClosed(void) {
  // If low (connected to GND) switch closed
  if (digitalRead (_buttonPin) == LOW) {
    return true;
  }
  return false;  
}
