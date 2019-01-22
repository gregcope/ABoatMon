#include <Arduino.h>
#include "Led.h"

Led::Led(int pin) {
  // Constructor
  // Takes an int as pin to power up
  _ledPin = pin;
  pinMode(_ledPin, OUTPUT);
}

void Led::on(void) {

  // switch if on and return (with it on)
  digitalWrite(_ledPin,HIGH);
}

void Led::off(void) {

  // simply switch it off and return (with it off)
  digitalWrite(_ledPin,LOW); 
}

void Led::blink(int DELAY_MS, byte loops) {

  // Blinks an LED ... blocking
  // for DELAY_MS
  // and loops number of times

  pinMode(_ledPin, OUTPUT);
  while (loops--) {
    digitalWrite(_ledPin, HIGH);
    delay(DELAY_MS);
    digitalWrite(_ledPin, LOW);
  }
}
