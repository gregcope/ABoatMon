#include <Arduino.h>
#include "Vcc.h"
Vcc::Vcc(int pin0, int pin1) {
  // Constructor
  // Takes an int as pin to power up
  // and another int for the voltage dividor pin
  _powerPin = pin0;
  _voltageDividerPin = pin1;
}

boolean Vcc::on(void) {
  // Switch device on by putting pin HIGH
  digitalWrite(_powerPin, HIGH);
  _powerState = true;
  DEBUG("reg on")
  // TODO: Add code to check charger has gone on?!?!?!
}

void Vcc::off(void) {
  // turn off the Device by putting pin LOW
  digitalWrite(_powerPin, LOW);
  _powerState = false;
  DEBUG("reg off")
}

boolean Vcc::isOn(void) {
  // return state 
  // on == true
  // off == false
  //DEBUG("Vcc state is:");
  //DEBUGln(_powerState);
  return _powerState;  
}


float Vcc::read(void) {
  Serial.println("reading vcc");
  Serial.flush();
  vccReadings = 0 ;
  vccVolts = 0;
  analogRead(_voltageDividerPin);
    for (byte i=0; i<10; i++) {
      //take 10 samples, and average
      //DEBUG("analogRead: ");
      //DEBUGln(analogRead(_voltageDividerPin));
      vccReadings+=analogRead(_voltageDividerPin);
    }

  vccVolts = BATT_FORMULA(vccReadings / 10.0);
  //DEBUGln(lipoBatteryVolts);
  return(vccVolts);

  //  dtostrf(batteryVolts,3,2, BATstr); //update the BATStr which gets sent every BATT_CYCLES or along with the MOTION message
  // 3 chars long 2 after decimal point
  //if (batteryVolts <= BATT_LOW) BATstr = "LOW";
}
