#include "Arduino.h"
#include "Device.h"

Device::Device(byte pin) {
  // Constructor
  // Takes an int as pin to power up
  _powerPin = pin;
  pinMode(_powerPin, OUTPUT);
}

void Device::on(void) {
  // Switch device on by putting pin LOW
  digitalWrite(_powerPin, HIGH);
  Serial.print("device on!");
}

void Device::off(void) {
  // turn off the Device by putting pin LOW
  digitalWrite(_powerPin, LOW);
  Serial.print("device off!");
}
