#include <Arduino.h>
#include "Button.h"

// Not much too this ...

// https://www.baldengineer.com/detect-short-long-button-press.html

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
