#include <Arduino.h>
#include "Device.h"

Device::Device(byte pin) {
  // Constructor
  // Takes an int as pin to power up
  _powerPin = pin;
  _powerState = false;
  pinMode(_powerPin, OUTPUT);
  digitalWrite(_powerPin, LOW);
}

void Device::on(void) {
  // Switch device on by putting pin HIGH
  digitalWrite(_powerPin, HIGH);
  _powerState = true;
  //Serial.println("device on!");
}

void Device::off(void) {
  // turn off the Device by putting pin LOW
  digitalWrite(_powerPin, LOW);
  _powerState = false;
  //Serial.println("device off!");
}

boolean Device::isOn(void) {
  // return state 
  // on == true
  // off == false
  // aka isOn
  return _powerState;  
}
