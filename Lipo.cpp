#include <Arduino.h>
#include "Lipo.h"

Lipo::Lipo(byte pin) {
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
      DEBUG("analogRead: ");
      DEBUGln(analogRead(_voltageDividerPin));
      lipoBatteryReadings+=analogRead(_voltageDividerPin);
    }

  lipoBatteryVolts = BATT_FORMULA(lipoBatteryReadings / 10.0);
  DEBUGln(lipoBatteryVolts);
  return(lipoBatteryVolts);
}
