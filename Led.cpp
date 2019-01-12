#include <Arduino.h>
#include "Led.h"

Led::Led(int pin) {
  // Constructor
  // Takes an int as pin to power up
  _ledPin = pin;
  pinMode(_ledPin, OUTPUT);
}

void Led::on(void) {
  digitalWrite(_ledPin,HIGH);
}

void Led::off(void) {
  digitalWrite(_ledPin,LOW); 
}

void Led::blink(int DELAY_MS, byte loops)
{
  pinMode(_ledPin, OUTPUT);
  while (loops--)
  {
    digitalWrite(_ledPin,HIGH);
    delay(DELAY_MS);
    digitalWrite(_ledPin,LOW);
  }
}
