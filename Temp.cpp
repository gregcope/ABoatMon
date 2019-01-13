#include <Arduino.h>
#include "Temp.h"

Temp::Temp(int power, int pin) {
  // Constructor
  // Takes an int as pin to power up, and data pin
  _powerPin = power;
  _dataPin = pin;
}

void Temp::startRead(void) {
  // issue a read command to let the uC do other stuff
  DEBUG("temp startRead: ");
}

float Temp::read(void) {
  // this blocks till a read or timeout reached
  DEBUG("temp read: ");
  _tempInC = 20.7;
  return _tempInC;
}
