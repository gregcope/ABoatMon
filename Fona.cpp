#include <Arduino.h>
#include "Fona.h"

Fona::Fona(int enablePin) {
  // Constructor
  // Takes an int as pin to power up
  _enablePin = enablePin;
}

boolean Fona::on(void) {
  // Switch device on by putting pin HIGH
  digitalWrite(_enablePin, LOW);
  _powerState = true;
  Serial.begin(9600);
  Serial.flush();
  if ( Serial.available() > 0) {
    // spitting something out so assume on!
    return true;
  } else {
    return false;  
  } 
}

void Fona::off(void) {
  // turn off the Device by putting pin LOW
  digitalWrite(_enablePin, HIGH);
  Serial.flush();
  //delay(500);
  _powerState = false;
}

boolean Fona::isOn(void) {
  // return state 
  // on == true
  // off == false
  //DEBUG("gps state is:");
  //DEBUGln(_powerState);
  return _powerState;  
}
