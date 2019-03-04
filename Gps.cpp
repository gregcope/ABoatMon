#include <Arduino.h>
#include "Gps.h"

Gps::Gps(byte pin) {
  // Constructor
  // Takes an int as pin to power up
  _powerPin = pin;
  pinMode(_powerPin, OUTPUT);
}

void Gps::init(void) {
  // 0 = Invalid, 1 = GPS fix, 2 = DGPS fix
  // 1 in this example
  // $GPGGA,064951.000,2307.1256,N,12016.4438,E,1,8,0.95,39.9,M,17.8,M,,*65
  DEBUGln("Init ... doing TinyGPSCustom ")
  TinyGPSCustom fixqual(nmea, "GPGGA", 6); // $GPGGA sentence, 6th element
  // report antenna
  // looking for
  // $PGTOP,11,3,6F
  // 1 = short, 2 = internal, 3 = external
  TinyGPSCustom antenna(nmea, "PGTOP", 2); // PGTOP sentence, 2nd element
}

boolean Gps::on(void) {
  // Switch device on by putting pin HIGH
  digitalWrite(_powerPin, HIGH);
  _powerState = true;
  Serial1.begin(9600);
  Serial1.flush();
  Serial.println("GPS device on!");
  Serial.flush();
  setupGPS();
  if ( Serial1.available() > 0) {
    // spitting something out so assume on!
    return true;
  } else {
    return false;
  }
}

void Gps::off(void) {
  // turn off the Device by putting pin LOW
  digitalWrite(_powerPin, LOW);
  Serial1.flush();
  //delay(500);
  _powerState = false;
  Serial.println("GPS device off!");
}

boolean Gps::isOn(void) {
  // return state
  // on == true
  // off == false
  //DEBUG("gps state is:");
  //DEBUGln(_powerState);
  return _powerState;
}

boolean Gps::updateFix(unsigned long timeout, int updates) {

  // takes a nmea timeout
  // assume timeout is 12 secs - 12000
  // run until the nmea object is updated
  // at least 10 times?
  // return true or false

  DEBUG("updateFix: ")

  nmeaTimeoutMs = millis() + timeout;
  nmeaUpdates = nmea.sentencesWithFix() + updates;

  // make sure GPS is on (may not be needed)
  if ( !isOn() ) {
    //DEBUGln("gps is off, turning it on")
    on();
  }

  //DEBUG("nmeaUpdates: ")
  //DEBUG(nmeaUpdates)
  //DEBUG(", nmea.sentencesWithFix()")
  //DEBUG(nmea.sentencesWithFix())
  //DEBUG(", nmeaTimeoutMs: ")
  //DEBUG(nmeaTimeoutMs)
  //DEBUG(", millis is: ")
  //DEBUGln(millis())

  while ( nmeaTimeoutMs > millis() ) {

    drainNmea();

    //if ( nmea.location.isUpdated() ) {
    //  // if the object was updated, update counter
    //  DEBUGln("nmea.location.isUpdated")
    //  nmeaUpdated ++;
    //}

    // when we have 10 fix updates (10 secs ish)
    // return
    //DEBUG("nmea.sentencesWithFix():")
    //DEBUGln(nmea.sentencesWithFix());
    if ( nmeaUpdates <= nmea.sentencesWithFix() ) {
      DEBUG("Got nmea updates: ")
      DEBUGln(updates);
      return true;
    }

  }

  DEBUG("nmeaUpdates: ")
  DEBUG(nmeaUpdates)
  DEBUG(", nmea.sentencesWithFix(): ")
  DEBUG(nmea.sentencesWithFix())
  DEBUG(", nmeaTimeoutMs: ")
  DEBUG(nmeaTimeoutMs)
  DEBUG(", millis is: ")
  DEBUG(millis())
  DEBUG(", fixqual: ");
  DEBUG(fixqual.value());
  DEBUG(", antenna: ");
  DEBUGln(antenna.value());

  if ( ( nmeaUpdates - nmea.sentencesWithFix() ) < 10 ) {
    // got some fixes, just not 10 ...
    return true;
  }
  // fell out of timeout with insufficient nmea updates
  return false;
}

boolean Gps::getUpdatedFix(unsigned long timeout, int updates) {

  on();
  updateFix(timeout, updates);
  off();

  DEBUG("getUpdatedFix: ");

  if ( nmea.location.isUpdated() ) {
    printGPSData();
    return true;
  } else {
    // give it another go
    //DEBUGln("No GPS update - giving it another go updateFix()")
    //updateFix(timeout);
    //if ( nmea.location.isUpdated() ) {
    //printGPSData();
    //} else {
    DEBUGln("No GPS update")
    return false;
    //}
  }
  return true;
}

unsigned long Gps::getInitialFix(unsigned long timeout) {

  // takes a gps fix tmeout
  // returns millis to get a fix OR zero if failed

  gpsFixTimeoutReached = false;
  gpsTimerStart = millis();
  gpsFixTimeoutMs = gpsTimerStart + timeout;
  gpsTimeToFixMs = 0;
  initialHDOP = 0;
  finalHDOP = 0;
  hdop = 0;
  now = 0;
  //nmeaUpdated = 0;

  on();

  DEBUGln("getInitialFix");

  while ( !gpsFixTimeoutReached ) {
    // capture now
    now = millis();

    // update the nmea object lots of times
    if ( updateFix(12000, 10) ) {
      // do nothing - it works
    } else {
      // no nmea ... bail
      // no updateds bail
      DEBUGln("no updated fixes")
      //return 0;
    }
    //while ( nmeaUpdated < 9 ) {
    //drainNmea();
    //   if ( nmea.hdop.isUpdated() ) {
    //     nmeaUpdated ++;
    //     DEBUG(".");
    //   }
    // }

    // see if we got a sensible fix
    DEBUG("getInitialFix: ");
    DEBUG(now);

    DEBUG(", hdop.isUpdated: ");
    if ( nmea.hdop.isUpdated() ) {
      DEBUG("TRUE");
    } else {
      DEBUG("FALSE");
    }

    hdop = nmea.hdop.value();

    DEBUG(", hdop.age: ");
    DEBUG(nmea.hdop.age());

    DEBUG(", loc.isUpdated: ");
    if (nmea.location.isUpdated() ) {
      DEBUG("TRUE");
    } else {
      DEBUG("FALSE");
    }

    DEBUG(", HDOP:");
    DEBUG(hdop);
    DEBUG(", Serial1.ava: ");
    DEBUG(Serial1.available());
    DEBUG(", ACCEPTABLE HDOP: ");
    DEBUG(ACCEPTABLE_GPS_HDOP_FOR_FIX);
    DEBUG(", initial HDOP: ");
    DEBUG(initialHDOP);
    DEBUG(", gpsFixTimeoutMs: ");
    DEBUG(gpsFixTimeoutMs);
    DEBUG(", nmea.passCksum: ");
    DEBUG(nmea.passedChecksum());
    DEBUG(", fixqual: ");
    DEBUG(fixqual.value());
    DEBUG(", antenna: ");
    DEBUGln(antenna.value());

    // do we have an acceptable fix yet?
    if ( ( hdop != 0 && hdop <= ACCEPTABLE_GPS_HDOP_FOR_FIX )  && initialHDOP == 0 ) {
      DEBUGln("gpsFix is true - we have an inital acceptable fix!!!!");
      DEBUG("nmea.hdop.value(): ");
      DEBUGln(hdop);
      // stops this if statement next time
      initialHDOP = hdop;
      gpsTimeToFixMs = now - gpsTimerStart;
      DEBUG("gpsTimeToFixMs: ");
      DEBUGln(gpsTimeToFixMs);
      printGPSData();
      //off();
      //return gpsTimeToFixMs;
    }

    // do we have a good fix?
    if ( hdop != 0 && hdop <= GOOD_GPS_HDOP_FOR_FIX ) {
      DEBUGln("gpsFix is true, hdop low - we have a good fix!!!!");
      DEBUG("nmea.hdop.value(): ");
      DEBUGln(hdop);
      finalHDOP = hdop;
      gpsTimeToFixMs = now - gpsTimerStart;
      DEBUG("gpsTimeToFixMs: ");
      DEBUGln(gpsTimeToFixMs);
      printGPSData();
      off();
      return gpsTimeToFixMs;
    }

    // we reached the timeout ... too bad
    if ( ( gpsFixTimeoutMs <= now ) && initialHDOP == 0) {

      //gpsFixTimeoutReached = true;
      // we failed to get a fix ...
      DEBUGln("We failed to get fix !!!");
      off();
      return 0;
    }

    // we reached a timeout, lets bomb
    if ( gpsFixTimeoutMs <= now ) {
      gpsFixTimeoutReached = true;
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

char* Gps::getdateTime() {
  // function to get recent GPS dateTime in short ISO 8601 format
  // if not recent, fireup GPS
  // format: 20190127T130140Z
  // Num of chars: 8+T+6+Z+Null = 17
  // Code semipinched from;
  // https://github.com/mikalhart/TinyGPSPlus/blob/master/examples/FullExample/FullExample.ino

  // initial state ...
  strcpy(_dateTime, "NO GPS DATE TIME");
  // update nmea data if it is older than 2 secs
  if ( nmea.date.age() > 2000 ) {
    on();
    updateFix(5000, 3);
    off();
  }

  if ( nmea.date.age() > 2000 ) {
    // did not get an update
    // so return default of "NO GPS DATE TIME"
    return _dateTime;
  }

  // format the _dateTime stringx
  sprintf(_dateTime, "%04d%02d%02dT%02d:%02d:%02dZ", nmea.date.year(), nmea.date.month(), nmea.date.day(), nmea.time.hour(), nmea.time.minute(), nmea.time.second());

  // return
  return _dateTime;
}

void Gps::getLocation(double &newLat, double &newLon) {

  // if location is 2 seconds or more old, update
  if ( nmea.location.age() > 2000 ) {
    on();
    updateFix(5000, 3);
    off();
  }
  newLat = nmea.location.lat();
  newLon = nmea.location.lng();  
}

void Gps::getRawLocation(char &charLat, char &charLon) {

//  sprintf(&lat, "%c%02c%
  // if location is 2 seconds or more old, update
  if ( nmea.location.age() > 2000 ) {
    on();
    updateFix(5000, 3);
    off();
  }

  // converting rawLat deg / billionths into longs
  // https://www.disk91.com/2016/technology/internet-of-things-technology/simple-lora-gps-tracker-based-on-rn2483-and-l80/
  // payload testing
  // https://ukhas.org.uk/guides:common_coding_errors_payload_testing
  // simple GPS lib
  // http://www.technoblogy.com/show?10WT

  // sprintf config
  // http://www.cplusplus.com/reference/cstdio/printf/
  sprintf(&charLat, "%s%i.%08li",  nmea.location.rawLat().negative ? "-" : "+", nmea.location.rawLat().deg, nmea.location.rawLat().billionths);
  sprintf(&charLat, "%s%i.%08li",  nmea.location.rawLng().negative ? "-" : "+", nmea.location.rawLng().deg, nmea.location.rawLng().billionths);  
}


void Gps::distanceMoved(double &lat, double &lon, double &distance) {

  // if location is 2 seconds or more old, update
  if ( nmea.location.age() > 2000 ) {
    on();
    updateFix(5000, 3);
    off();
  }

  //Serial.print("org lat/lon: ");
  //Serial.print(lat, 6);
  //Serial.print(",");
  //Serial.print(lon, 6);
  //Serial.print(", new lat/lon: ");
  //Serial.print(nmea.location.lat(), 6);
  //Serial.print(",");
  //Serial.print(nmea.location.lng(), 6);
  //Serial.print(", distance: ");
  
  //_distance = nmea.distanceBetween(nmea.location.lat(), nmea.location.lng(), lat, lon);
  distance = haversine(nmea.location.lat(), nmea.location.lng(), lat, lon);
  //Serial.println(distance);

  //return _distance;
}


void Gps::setupGPS(void) {

  // Function to setup GPS
  DEBUG("in setupGPS")

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

// https://community.particle.io/t/tinygps-using-distancebetween/28233/3
double Gps::haversine(double lat1, double lon1, double lat2, double lon2) {
    const double rEarth = 6371000.0; // in meters
    double x = pow( sin( ((lat2 - lat1)*M_PI/180.0) / 2.0), 2.0 );
    double y = cos(lat1*M_PI/180.0) * cos(lat2*M_PI/180.0);
    double z = pow( sin( ((lon2 - lon1)*M_PI/180.0) / 2.0), 2.0 );
    double a = x + y * z;
    double c = 2.0 * atan2(sqrt(a), sqrt(1.0-a));
    double d = rEarth * c;
    // Serial.printlnf("%12.9f, %12.9f, %12.9f, %12.9f, %12.9f, %12.9f", x, y, z, a, c, d);
    return d; // in meters
}

void Gps::printGPSData(void) {

  Serial.print(F("GPS Location: "));
  if (nmea.location.isValid())
  {
    Serial.print(nmea.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(nmea.location.lng(), 6);


 Serial.print(F("GPS raw loc (deg then billionths): "));
  char s[30];
  char t[30];
  sprintf(s, "%s%i.%09li",  nmea.location.rawLat().negative ? "-" : "+", nmea.location.rawLat().deg, nmea.location.rawLat().billionths);
  sprintf(t, "%s%i.%09li",  nmea.location.rawLng().negative ? "-" : "+", nmea.location.rawLng().deg, nmea.location.rawLng().billionths);  
  Serial.print(s);
  Serial.print(",");
  Serial.print(t);
    
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
