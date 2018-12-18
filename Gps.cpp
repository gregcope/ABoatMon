#include <Arduino.h>
#include "Gps.h"

Gps::Gps(byte pin) {
  // Constructor
  // Takes an int as pin to power up
  _powerPin = pin;
  _powerState = false;
  pinMode(_powerPin, OUTPUT);
  digitalWrite(_powerPin, LOW);
}

void Gps::on(void) {
  // Switch device on by putting pin HIGH
  digitalWrite(_powerPin, HIGH);
  _powerState = true;
  Serial1.begin(9600);
  Serial1.flush();
  Serial.println("GPS device on!");
  Serial.flush();
}

void Gps::off(void) {
  // turn off the Device by putting pin LOW
  Serial1.flush();
  digitalWrite(_powerPin, LOW);
  _powerState = false;
  Serial.println("GPS device off!");
}

boolean Gps::isOn(void) {
  // return state 
  // on == true
  // off == false
  // aka isOn
  return _powerState;  
}

unsigned long Gps::getFix(unsigned long timeout) {

  // takes a gps fix tmeout
  // returns millis to get a fix OR zero if failed

  gpsTimerStart = millis();
  gpsFixTimeoutMs = gpsTimerStart + timeout;
  gpsTimeToFixMs = 0;
  gpsFix = false;
  initialHDOP = 0;
  finalHDOP = 0;
  hdop = 0;
  
  on();
  setupGPS();
  DEBUGln("setupGPS");

  while ( !gpsFixTimeoutReached && !gpsFix ) {
    // Whilst we have not reached the GPS timeout, nor got a fix, keep going ...
    drainNmea();
    
    hdop = nmea.hdop.value();
    DEBUGln(hdop);

    if ( hdop != 0 && hdop <= ACCEPTABLE_GPS_HDOP_FOR_FIX && initialHDOP == 0 ) {
      DEBUGln("gpsFix is true - we have an acceptable fix!!!!");
      DEBUG("nmea.hdop.value(): ");
      DEBUGln(hdop);
      initialHDOP = hdop;  
      gpsTimeToFixMs = millis() - gpsTimerStart;
      gpsFix = true;
      DEBUG("gpsTimeToFixMs: ");
      DEBUGln(gpsTimeToFixMs);
      printGPSData();
      //off();
      //return gpsTimeToFixMs;
    }

   if ( hdop != 0 && hdop <= GOOD_GPS_HDOP_FOR_FIX ) {
      DEBUGln("gpsFix is true - we have a good fix!!!!");
      DEBUG("nmea.hdop.value(): ");
      DEBUGln(hdop);
      finalHDOP = hdop;  
      gpsTimeToFixMs = millis() - gpsTimerStart;
      gpsFix = true;
      DEBUG("gpsTimeToFixMs: ");
      DEBUGln(gpsTimeToFixMs);
      printGPSData();
      off();
      return gpsTimeToFixMs;
    }

    if ( gpsFixTimeoutMs <= millis() && gpsFix == false) {
      // we reached the timeout ... too bad
      gpsFixTimeoutReached = true;
      // we failed to get a fix ...
      off();
      return 0; 
    }
 
  } // we fell out of loop with a reasonable GPS fix.

  DEBUGln("We only got a reasonable fix !!!");
  DEBUG("gpsTimeToFixMs: ");
  DEBUGln(gpsTimeToFixMs); 
  printGPSData();
  off();
  return gpsTimeToFixMs;
}

boolean Gps::drainNmea(void) {

  // grab all avaliable data and feed it to the gps nmea parser

  //DEBUGln("drainNmea()");
  serial1Output = false;
  nmeaOutput = false;

  while (Serial1.available() > 0) {
    serial1Output = true;
    if (nmea.encode(Serial1.read())) {
      nmeaOutput = true;
      // returns true if a sentence is complete
      // updates nmea.passedChecksum()
    }
  }
  
  return serial1Output;
}

void Gps::setupGPS(void) {

  // Function to setup GPS
  
  // http://forums.adafruit.com/viewtopic.php?f=19&p=143502
  //DEBUGln("  Enable SBAS sat search");
  Serial1.print("$PMTK313,1*2E\r\n");  // Enable to search a SBAS satellite
  //DEBUGln("  Enable WAAS as DPGS source");
  Serial1.print("$PMTK301,2*2E\r\n");  // Enable WAAS as DGPS Source
  // https://code.google.com/p/ardupilot-mega/source/browse/libraries/AP_GPS/AP_GPS_NMEA.h?spec=svneff9f9d0906c1e3cd4e217b0363d0f9d44394e75&name=f401de6d52&r=eff9f9d0906c1e3cd4e217b0363d0f9d44394e75
  //DEBUGln("  Set GPS hz to 1hz");
  Serial1.print("$PMTK220,1000*1F\r\n"); // 1HZ
  // http://aprs.gids.nl/nmea/#gga
  // 0 = Invalid, 1 = GPS fix, 2 = DGPS fix
  // 1 in this example
  // $GPGGA,064951.000,2307.1256,N,12016.4438,E,1,8,0.95,39.9,M,17.8,M,,*65
  TinyGPSCustom fixqual(nmea, "GPGGA", 6); // $GPGGA sentence, 6th element

}

void Gps::printGPSData(void) {

  Serial.print(F("GPS Location: "));
  if (nmea.location.isValid())
  { 
    Serial.print(nmea.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(nmea.location.lng(), 6);
  }
  else
  { 
    Serial.print(F("INVALID"));
  }

  // print date /time in ISO 8601 format
  // YYYY-MM-DDTHHMMSSCCZ
  Serial.print(F(", Date/Time: "));
  if (nmea.date.isValid())
  { 
    Serial.print(nmea.date.year());
    Serial.print(F("-"));
    Serial.print(nmea.date.month());
    Serial.print(F("-")); 
    if (nmea.date.day() < 10) Serial.print(F("0"));
    Serial.print(nmea.date.day());
  }
  else
  { 
    Serial.print(F("INVALID"));
  }

  Serial.print(F("T"));
  if (nmea.time.isValid())
  { 
    if (nmea.time.hour() < 10) Serial.print(F("0"));
    Serial.print(nmea.time.hour());
    Serial.print(F(":"));
    if (nmea.time.minute() < 10) Serial.print(F("0"));
    Serial.print(nmea.time.minute());
    Serial.print(F(":"));
    if (nmea.time.second() < 10) Serial.print(F("0"));
    Serial.print(nmea.time.second());
    Serial.print(F("."));
    if (nmea.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(nmea.time.centisecond());
    Serial.print("Z");
  }
  else
  { 
    Serial.print(F("INVALID"));
  } 

  Serial.print(F(", "));
  if (nmea.hdop.isValid())
  {

    Serial.print(F("HDOP: "));
    Serial.print(nmea.hdop.value());
    Serial.print(F(" HDOP is: "));

    if (nmea.hdop.value() < 150  && nmea.hdop.value() != 0) {
      Serial.print(F("GOOD"));
    } else {
      Serial.print(F("BAD"));
    }
   

  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(", "));
  if (nmea.satellites.isValid())
  {
    Serial.print(F("SATs in use: "));
    Serial.print(nmea.satellites.value());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
  Serial.flush();
}
