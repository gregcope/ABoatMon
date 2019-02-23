#include <Arduino.h>
#include "Gns.h"

Gns::Gns(byte pin) {
  // Constructor
  // Takes an int as pin to power up
  _powerPin = pin;
  pinMode(_powerPin, OUTPUT);
}

boolean Gns::on(void) {
  // Switch device on by putting pin HIGH
  digitalWrite(_powerPin, HIGH);
  _powerState = true;
  Serial1.begin(9600);
  Serial1.flush();
  Serial.println("GnS device on!");
  Serial.flush();
  setupGNS();
  if ( Serial1.available() > 0) {
    // spitting something out so assume on!
    return true;
  } else {
    return false;
  }
}

void Gns::off(void) {
  // turn off the Device by putting pin LOW
  digitalWrite(_powerPin, LOW);
  Serial1.flush();
  //delay(500);
  _powerState = false;
  Serial.println("GNS device off!");
}

boolean Gns::isOn(void) {
  // return state
  // on == true
  // off == false
  //DEBUG("gps state is:");
  //DEBUGln(_powerState);
  return _powerState;
}

void Gns::setupGNS(void) {

  // Function to setup GNS
  DEBUG("in setupGNS")

  // http://forums.adafruit.com/viewtopic.php?f=19&p=143502
  //DEBUGln("  Enable SBAS sat search");
  Serial1.print("$PMTK313,1*2E\r\n");  // Enable to search a SBAS satellite
  //DEBUGln("  Enable WAAS as DPGS source");
  Serial1.print("$PMTK301,2*2E\r\n");  // Enable WAAS as DGPS Source
  // https://code.google.com/p/ardupilot-mega/source/browse/libraries/AP_GPS/AP_GPS_NMEA.h?spec=svneff9f9d0906c1e3cd4e217b0363d0f9d44394e75&name=f401de6d52&r=eff9f9d0906c1e3cd4e217b0363d0f9d44394e75
  //DEBUGln("  Set GPS hz to 1hz");
  Serial1.print("$PMTK220,1000*1F\r\n"); // 1HZ
  // http://aprs.gids.nl/nmea/#gga
  // report antenna
  Serial1.print("$PGCMD,33,1*6C\r\n");
  DEBUGln(" ... done")
}
