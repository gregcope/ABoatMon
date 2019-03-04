#include <Arduino.h>
#include "Gns.h"

Gns::Gns(byte pin) {
  
  // Constructor
  // Takes an int as pin to power up
  
  _powerPin = pin;
  pinMode(_powerPin, OUTPUT);
}

boolean Gns::on(void) {

  // switches on GNS
  // calls setup
  
  // Switch device on by putting pin HIGH
  digitalWrite(_powerPin, HIGH);
  _powerState = true;
  gpsPort.begin(9600);
  gpsPort.flush();
  Serial.println("GNS device on!");
  Serial.flush();
  setupGNS();
  if ( gpsPort.available() > 0) {
    // spitting something out so assume on!
    return true;
  } else {
    return false;
  }
}

void Gns::off(void) {

  // clears buffers and powers off GNS
  
  // turn off the Device by putting pin LOW
  digitalWrite(_powerPin, LOW);
  gpsPort.flush();
  //delay(500);
  _powerState = false;
  Serial.println("GNS device off!");
}

boolean Gns::isOn(void) {

  // returns GNS power state
  // does not mean anything is actually working!
  
  // on == true
  // off == false
  //DEBUG("gps state is:");
  //DEBUGln(_powerState);
  return _powerState;
}

void Gns::setupGNS(void) {

  // Function to setup GNS
  // Sets BAUD, search for SBAS and enable WAAS/DGPS
  
  DEBUG("in setupGNS")
  // https://github.com/SlashDevin/NeoGPS/blob/master/examples/NMEArevGeoCache/NMEArevGeoCache.ino#L102
  // done with;
  // gpsPort.print
  //    ( F( "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n" // RMC only...
  //       "$PMTK220,1000*1F\r\n" ) );    // ...and 1 Hz update rate
  // http://forums.adafruit.com/viewtopic.php?f=19&p=143502
  gpsPort.print( F("PMTK313,1*2E\r\n"  // Enable to search a SBAS satellite
    // https://code.google.com/p/ardupilot-mega/source/browse/libraries/AP_GPS/AP_GPS_NMEA.h?spec=svneff9f9d0906c1e3cd4e217b0363d0f9d44394e75&name=f401de6d52&r=eff9f9d0906c1e3cd4e217b0363d0f9d44394e75
    "$PMTK301,2*2E\r\n" // Enable WAAS as DGPS Source
    "$PMTK220,1000*1F\r\n" ) );    // ...and 1 Hz update rate
  

  // http://forums.adafruit.com/viewtopic.php?f=19&p=143502
  //DEBUGln("  Enable SBAS sat search");
  //Serial1.print("$PMTK313,1*2E\r\n");  // Enable to search a SBAS satellite
  //DEBUGln("  Enable WAAS as DPGS source");
  // http://aprs.gids.nl/nmea/#gga
  // report antenna
  //Serial1.print("$PGCMD,33,1*6C\r\n");
  DEBUGln(" ... done")
}

unsigned long Gns::getInitialFix(unsigned long timeout) {

  // get an initial fix
  // takes a timeout
  // switches on GNS and loops until either
  // fix found or timeout reached
  // looks for a good fix if possible
  // returns gnsTimeToFixMs (which could be 0 for no fix)

  // initialise vars
  gnsFixTimeoutReached = false;
  gnsTimerStart = millis();
  gnsFixTimeoutMs = gnsTimerStart + timeout;
  gnsTimeToFixMs = 0;
  initialHDOP = 0;
  finalHDOP = 0;
  hdop = 0;

  // switch on GPS
  on();

  DEBUG("Gns::getInitialFix - timeout: ");
  DEBUGln(timeout);

  
  // while we have not timed out
  while ( !gnsFixTimeoutReached ) {
    // capture now
    now = millis();

    // update the nmea object lots of times
    // which updates gps object struct gps_fix
    if ( updateFix(12000) ) {
      // do nothing - it works
    } else {
      // no nmea ... bail
      // no updateds bail
      //return 0;
    }

    // see if we got a sensible fix
    DEBUG("Gns::getInitialFix: ");
    DEBUG(now);

    DEBUG(", fix.status: ");
    if ( fix.status ) {
      DEBUG(fix.status);
    } else {
      DEBUG("FALSE");
    }

    if ( fix.valid.hdop ) {
      hdop = fix.hdop;
    }

    //DEBUG(", hdop.age: ");
    //DEBUG(nmea.hdop.age());

    DEBUG(", fix.valid.location: ");
    if ( fix.valid.location ) {
      DEBUG("TRUE");
    } else {
      DEBUG("FALSE");
    }

    DEBUG(", HDOP: ");
    DEBUG(hdop);
    DEBUG(", Serial1.ava: ");
    DEBUG(Serial1.available());
    DEBUG(", ACCEPTABLE HDOP: ");
    DEBUG(ACCEPTABLE_GNS_HDOP_FOR_FIX);
    DEBUG(", initial HDOP: ");
    DEBUG(initialHDOP);
    DEBUG(", gnsFixTimeoutMs: ");
    DEBUG(gnsFixTimeoutMs);
    DEBUG(", fix.status: ");
    DEBUGln(fix.status);

    // do we have an acceptable fix yet?
    if ( ( hdop != 0 && hdop <= ACCEPTABLE_GNS_HDOP_FOR_FIX )  && initialHDOP == 0 ) {
      DEBUGln("gnsFix is true - we have an inital acceptable fix!!!!");
      DEBUG("fix.hdop(): ");
      DEBUGln(hdop);
      // stops this if statement next time
      initialHDOP = hdop;
      gnsTimeToFixMs = now - gnsTimerStart;
      DEBUG("gnsTimeToFixMs: ");
      DEBUGln(gnsTimeToFixMs);
      //printGPSData();
      //off();
      //return gpsTimeToFixMs;
    }

    // do we have a good fix?
    if ( hdop != 0 && hdop <= GOOD_GNS_HDOP_FOR_FIX ) {
      DEBUGln("gnsFix is true, hdop low - we have a good fix!!!!");
      DEBUG("fix.hdop(): ");
      DEBUGln(hdop);
      finalHDOP = hdop;
      gnsTimeToFixMs = now - gnsTimerStart;
      DEBUG("gnsTimeToFixMs: ");
      DEBUGln(gnsTimeToFixMs);
      //printGPSData();
      off();
      return gnsTimeToFixMs;
    }

    // we reached the timeout ... too bad
    if ( ( gnsFixTimeoutMs <= now ) && initialHDOP == 0) {

      //gpsFixTimeoutReached = true;
      // we failed to get a fix ...
      DEBUGln("We failed to get fix !!!");
      off();
      return 0;
    }

    // we reached a timeout, lets bomb
    if ( gnsFixTimeoutMs <= now ) {
      gnsFixTimeoutReached = true;
    }


  } // we fell out of loop with a reasonable GPS fix.

  DEBUGln("We only got a reasonable fix !!!");
  DEBUG("gnsTimeToFixMs: ");
  DEBUGln(gnsTimeToFixMs);
  //printGPSData();
  off();
  return gnsTimeToFixMs;
}

boolean Gns::updateFix(unsigned long timeout) {

  // takes a nmea timeout and number of updates to do
  // assume timeout is 12 secs - 12000
  // update gps object struct gps_fix
  // return true or false if we got a fix

  fixes = 0;

  DEBUG("Gns::updateFix with timeout: ")
  DEBUGln(timeout)

  // set timeout
  nmeaTimeoutMs = millis() + timeout;

  // make sure GPS is on
  if ( !isOn() ) {
    //DEBUGln("gps is off, turning it on")
    on();
  }

  DEBUGln("Gns::updateFix - about to drain nmea");

  // drain the Serial gpsPort for required time
  while (nmeaTimeoutMs > millis()) {
    while (gps.available( gpsPort )) {
        fixes ++;
        fix = gps.read();
        DEBUGln("Gns::updateFix - internal fix"); 
      }
  }

  DEBUG("Gns::updateFix reached nmeaTimeoutMs - ");
  DEBUG(nmeaTimeoutMs);
  DEBUG(", fixes: ");
  DEBUGln(fixes);

  // if we have a fix return true, else return false)
  if ( fix.valid.location ) {
    DEBUGln("Gns::updateFix - fix.valid.location is true")
    return true;
  } else {
    DEBUGln("Gns::updateFix - fix.valid.location is false")
    return false;
  }
}
