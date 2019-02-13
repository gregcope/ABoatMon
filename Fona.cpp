#include <Adafruit_FONA.h>

#include <Arduino.h>
#include "Fona.h"

// https://www.baldengineer.com/hologram-io-connects-fona-mqtt.html
// https://www.hackster.io/baldengineer/fona-gps-tracker-2c2bbd
// https://github.com/baldengineer/fona-gps-tracker/blob/master/MQTT-FONA-Teensy32-GPS.ino

//https://learn.adafruit.com/adafruit-fona-mini-gsm-gprs-cellular-phone-module?view=all#pinouts
Fona::Fona(int enablePin) {
  // Constructor
  // Takes an int as pin to power up
  _enablePin = enablePin;
  pinMode(_enablePin, OUTPUT);
}

boolean Fona::on(void) {
  // Switch device on by putting pin HIGH
  digitalWrite(_enablePin, LOW);
  _powerState = true;
  Serial.begin(9600);
  Serial.flush();
  DEBUGln("Fona on!");
  delay(2001);
  DEBUGln("should still be on Fona on!");
  digitalWrite(_enablePin, HIGH);
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
