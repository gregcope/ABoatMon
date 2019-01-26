#include <Arduino.h>
#include "Button.h"

// Not much too this ...

Button::Button(byte pin) {
  // Constructor
  // Takes an int as pin to watch
  _buttonPin = pin;
  pinMode(_buttonPin, INPUT_PULLUP);
}

boolean Button::isClosed(void) {
  // If low (connected to GND) switch closed
  // and return true
  //DEBUG("Button: ");
  //DEBUG(_buttonPin);
  //DEBUG(", isClosed: ");

  if ( digitalRead(_buttonPin) == LOW ) {
    //DEBUGln("Switch is closed");
    return true;
  }
  return false;  
}
