#include <Arduino.h>
#include "Lipo.h"

Lipo::Lipo(int pin) {
  // Constructor
  // Takes an int as pin to power up
  _voltageDividerPin = pin;
}

float Lipo::read(void) {
  Serial.println("reading Lipo");
  Serial.flush();
  lipoBatteryReadings = 0 ;
  lipoBatteryVolts = 0;
  analogRead(_voltageDividerPin);
    for (byte i=0; i<10; i++) {
      //take 10 samples, and average
      //DEBUG("analogRead: ");
      //DEBUGln(analogRead(_voltageDividerPin));
      lipoBatteryReadings+=analogRead(_voltageDividerPin);
    }

  lipoBatteryVolts = BATT_FORMULA(lipoBatteryReadings / 10.0);
  //DEBUGln(lipoBatteryVolts);
  return(lipoBatteryVolts);

  //  dtostrf(batteryVolts,3,2, BATstr); //update the BATStr which gets sent every BATT_CYCLES or along with the MOTION message
  // 3 chars long 2 after decimal point
  //if (batteryVolts <= BATT_LOW) BATstr = "LOW";
}
