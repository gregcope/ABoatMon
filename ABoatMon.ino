/*
 * Greg Cope <gregcope@gmail.com>
 * see; https://github.com/gregcope/ABoatMon
 * 
 * Allot of code pinched from;
 * https://github.com/LowPowerLab/RFM69/blob/master/Examples/MotionMote/MotionMote.ino
 * 
 * TODO
 * Look at https://github.com/cdl1051/DS18B20_NROO/blob/master/DS18B20.h
 * https://lowpowerlab.com/forum/projects/temperature-sensing-with-ds18b20/msg18040/#msg18040
 * https://lowpowerlab.com/forum/moteino/improvedoptimized-ds18b201wire-read/msg14975/#msg14975
 * 
 * 
 */ 

// External includes
// https://github.com/mikalhart/TinyGPSPlus/releases 
#include <TinyGPS++.h>
// http://www.pjrc.com/teensy/td_libs_OneWire.html
#include <OneWire.h>

// internal classes/includes
#include "Device.h"
#include "Config.h"
#include "Sleep.h"

// PIN defines
#define GPS_POWER 12
#define GPS_TX 10 // serial1
#define GPS_RX 11 // serial1

#define BUTTON_LED 26
#define BUTTON 25
 
#define BILGE_SWITCH 3 // Other line from BilgeSwitch to GND

#define LIPO_VOLTAGE_DIVIDER A0

#define TEMP_POWER 27
#define TEMP_DATA 28

#define VCC_12V-24V_VOLTAGE_DIVIDER A5 
#define VCC_12V-24V_MOSFET 19

#define FONA_TX 8  // serial
#define FONA_RX 9  // serial
#define FONA_POWER 30
#define FONA_KEY 14
#define FONA_PS 18
#define FONA_NETSTAT 13

// Static defines
#define FOURMIN_CYCLES 5 // 8 sec sleep * 30 cycles = 240 secs or 4 mins
#define HOUR_CYCLES 450 // 8 sec sleep * 450 cyles == 3600 secs or 1 hour

// 3.3 (Vcc) / 1024 (ADC precision) =  0.00322
// Motineo example 1m+470k = 470 / ( 1000 +470 ) = 0.32 rounded up
// Ours 1.8m + 3.3m = 3300000 / ( 1800000 + 3300000 ) = 0.647 or 1.55 (inverse)
#define BATT_FORMULA(reading) reading * 0.00322 * 1.5455 // >>> fine tune this parameter to match your voltage when fully charged
                                                       // details on how this works: https://lowpowerlab.com/forum/index.php/topic,1206.0.html

#define NMEA_TIMEOUT_SECS 24 // time to try and get a proper NMEA senstence
#define GPS_FIX_TIMEOUT_MSECS 480000 // time to try and get a fix in msecs
#define ACCEPTABLE_GPS_HDOP_FOR_FIX 160

// debug functions
#define DEBUG(input)   {Serial.print(input); Serial.flush();}
#define DEBUGln(input) {Serial.println(input); Serial.flush();}

// Objects

// tinyGPS is a nmea feed parser
TinyGPSPlus nmea;
Sleep sleep;
// these are devices, and have physical on/off/interface things
Device gpsDevice(GPS_POWER);
Device buttonLed(BUTTON_LED);
Device tempSensor(TEMP_POWER);
// Make one wire faster
// http://www.cupidcontrols.com/2014/10/moteino-arduino-and-1wire-optimize-your-read-for-speed/

Config config;

//Device charger(CHARGER_POWER);
//Device fona(FONA_POWER);

// Variables
byte bilgeSwitchPosition = 0;

// Cycle Vars
unsigned long cycleCount = 0;
byte fourMinCycleCount = 0;
unsigned int hourCycleCount = 0;
unsigned long NOW = 0;

// GPS / Nmea vars
//unsigned long nmeaSentenceTimeOutMs = 0;
boolean serial1Output = false; // are we getting Serial1 output
boolean nmeaOutput = false; // are we parsing anything
unsigned long gpsFixTimeoutMs = 0;
unsigned long timerStart = 0; 
unsigned long gpsTimeToFixMs = 0;
boolean gpsFix = false;
boolean gpsFixTimeoutReached = false;
//boolean nmeaSentenceTimeoutReached = false;
double fixLat = 100; // invalid Lat
double fixLng = 100; // invalid Lat
double alarmLat = 100; // invalid Lat
double alarmLng = 100; // invalid Lat

// LipoBattery
float lipoBatteryAlarm = 3.1;
float lipoBatteryVolts = 0;
unsigned int lipoBatteryReadings = 0;
char lipoBatteryVoltsString[10]; //longest battery voltage reading message = 9chars

// Message vars
String messageStr = "";
byte needToSendMessage = 0;
byte batMessageSent = 0;
byte bilgeMessageSent = 0;
byte gpsNoFixMessageSent = 0;
byte gpsGeoFenceMessageSent = 0;

//
// Code from here on ...
//
void setup() {

  // as we are in setup, fire up LED
  buttonLed.on();  

  // Clear the serial ...
  Serial.begin(9600); 
  Serial.flush();

  // Let things settle...
  sleep.kip1Sec();
  DEBUGln("setup start");

  // we are off
  config.load();
  DEBUGln(*config.getGSMPhone());
  char phone[12] = "+9999999888";
  config.setGSMPhone(phone);
  
  DEBUG("Loaded SavedLat is: '");
  DEBUG(*config.getSavedLat());
  DEBUGln("'");
  DEBUG("Loaded SavedLng is: '");
  DEBUG(*config.getSavedLng());
  DEBUGln("'");

  double lat = -200;
  double lng = -300;

  config.setSavedLat(&lat);
  config.setSavedLng(&lng);  

  DEBUGln("save config");
  DEBUGln(millis());
  config.save();
  
  DEBUGln("load config config");
  config.load();
  
  DEBUG("getGSMPhone is: ");
  DEBUGln(config.getGSMPhone());

  DEBUG("SavedLat should be -200: '");
  DEBUG(*config.getSavedLat());
  DEBUGln("'");
  DEBUG("SavedLng should be -300: '");
  DEBUG(*config.getSavedLng());
  DEBUGln("'");

  // For sanity have setup functions per thing
  setupBilgeSwitch();
  setupGPS();
  setupGSM();

  // done
  buttonLed.off();
  DEBUGln("setup Done");
}

void loop() {
  
  // running so put LED on
  buttonLed.on();
  DEBUG("loop: ");

  // up cycle Count
  cycleCount ++;
  DEBUGln(cycleCount);
  
  // check bildge Switch each loop
  if ( checkBilgeSwitch() > 0 ) {
    // panic ...
    // have we sent a message?
    DEBUGln("Bilgeswitch on!!!!");
    if ( bilgeMessageSent == 0 ) {
      DEBUGln("Need to send bilge message");
      needToSendMessage = 1;
      messageStr = "Bilge Switch is on";
      // assume we have sent a message to 
      bilgeMessageSent = 1;
    } 
  } else {
    // bilgeSwitch is off
    // some sort of debounce for messages here
    if ( bilgeMessageSent == 1 ) {
      DEBUGln("Clearing bilgeMessageSent");
      bilgeMessageSent = 0;  
    }
  }

  // four minute checks
  fourMinCycleCount++;
  DEBUGln(fourMinCycleCount);
  if ( fourMinCycleCount == FOURMIN_CYCLES) {
    DEBUG("4 mins check, fourMinCycleCount: ");
    
    lipoCheckBattery();
    //flipCharger();

    //drainNmea();

    // reset counter
    fourMinCycleCount = 0;
  }

  // Hourly checks
  hourCycleCount++;
  if ( hourCycleCount == HOUR_CYCLES) {
    DEBUGln("Top of the hour ...");

    lipoCheckBattery();
    //checkPosition();

    // reset counter
    hourCycleCount = 0;
  }

   // do we need to send a message
  if ( needToSendMessage == 1 ) {
    sendMessage();
  }

  // finished so tidy and goto sleep
  messageStr = "";
  buttonLed.off();
  sleep.kip8Secs();
}

void sendMessage() {

  DEBUG("Sending message: ");
  DEBUGln(messageStr);
  // clear needToSendMessage flag
  needToSendMessage = 0;
}

void lipoCheckBattery() {
    DEBUGln("lipoCheckBattery() .. battery readings: ");
    lipoBatteryReadings = 0 ;
    // read it first and throw it away
    analogRead(LIPO_VOLTAGE_DIVIDER);
    for (byte i=0; i<10; i++) {
      //take 10 samples, and average
      lipoBatteryReadings+=analogRead(LIPO_VOLTAGE_DIVIDER);
    }
    //DEBUG("lipoBatteryReadings (x10) = ");
    //DEBUGln(lipoBatteryReadings);
    // work out the volts from the ADC value
    lipoBatteryVolts = BATT_FORMULA(lipoBatteryReadings / 10.0);
    DEBUGln(lipoBatteryVolts);

    // work out if we need to bleat
    if ( lipoBatteryVolts < lipoBatteryAlarm ) {
      dtostrf(lipoBatteryVolts, 3,2, lipoBatteryVoltsString); //update the BATStr which gets sent every BATT_CYCLES or along with the MOTION message
      needToSendMessage = 1;

      // Do we need to add a ", " to messageStr?
      if ( messageStr.length() > 0 ) {
        messageStr.concat(", ");  
      }

      // add battery message
      messageStr.concat("BattLow: ");
      messageStr.concat(lipoBatteryVoltsString);
      messageStr.concat("V");
      //messageStr = "BattLow: " + lipoBatteryVoltsString + "V";
    }
}

byte checkBilgeSwitch() {

  // Function to return state of bilgeSwitch
  // We do not debounce, as we just want to know state there and then
  // If it is bouncing we will pick it up next loop
  // http://www.gammon.com.au/switches

  if (digitalRead (BILGE_SWITCH) == LOW) {
    // bilgeSwitch is on!!!!
    bilgeSwitchPosition = 1;
  } else {
    // bilgeSwitch must be off...
    bilgeSwitchPosition = 0;
  }
  return bilgeSwitchPosition;
}

void setupBilgeSwitch() {

  // enabled internal Pull up on BILGE_SWITCH pin
  // not much to go wrong here ..
  // or anything to check
  pinMode (BILGE_SWITCH, INPUT_PULLUP);
 
}

void setupGSM() {
  DEBUGln("setupGSM");  

  // fire up GSM
  // go get last SMS message as phoneNumber
  // delete all messages
  // send message to phoneNumber
  // get SMS back from phoneNumber
  // set phoneNumber in config
}

void setupGPS() {

  DEBUGln("setupGPS");
  // Setup the GPS from first use or hardreset

  // check if GPS powers up
  // and outputs NMEA sentences
  if ( switchOnAndConfigGPS() ) {
    // true !!!
    DEBUGln("setupGPS() true");
      
    // now see if we can get a decent fix within the timeout
    if ( checkForGPSFix() ) {
      // We got a fix.  Awesome!!!
      DEBUGln("checkForGPSFix() true");
    } else {
      // Uhooooo - no fix - failed!!!
      DEBUGln("checkForGPSFix() false");
    } // end of if ( checkForGPSFix() )
  } else {
    // failed !!!
    DEBUGln("setupGPS() false");
  }

  // done, switch it off
  pinMode(12, INPUT);
  //gpsDevice.off();
}

boolean switchOnAndConfigGPS() {

  DEBUGln("switchAndConfigGPS()");

  // switch on 
  //gpsDevice.on();
  pinMode(12, OUTPUT);
  digitalWrite(12, LOW);

  DEBUGln("gpsDevice.on()");
     
  // Configure GPS settings at startUp/Power up
  Serial1.begin(9600); 
  Serial1.flush();

  // wait 2 secs for GPS to stablise, because we are nice
  sleep.kip2Secs();
  
  // http://forums.adafruit.com/viewtopic.php?f=19&p=143502
  Serial1.print("$PMTK313,1*2E\r\n");  // Enable to search a SBAS satellite
  Serial1.print("$PMTK301,2*2E\r\n");  // Enable WAAS as DGPS Source
  // https://code.google.com/p/ardupilot-mega/source/browse/libraries/AP_GPS/AP_GPS_NMEA.h?spec=svneff9f9d0906c1e3cd4e217b0363d0f9d44394e75&name=f401de6d52&r=eff9f9d0906c1e3cd4e217b0363d0f9d44394e75
  Serial1.print("$PMTK220,1000*1F\r\n"); // 1HZ
  //Serial1.print("$PSRF151,1*3F\r\n"); // WAAS_ON
  //Serial1.print("$PMTK513,1*28\r\n"); // Search for SBAS Sat

  // http://aprs.gids.nl/nmea/#gga
  // 0 = Invalid, 1 = GPS fix, 2 = DGPS fix
  // 1 in this example
  // $GPGGA,064951.000,2307.1256,N,12016.4438,E,1,8,0.95,39.9,M,17.8,M,,*65
  TinyGPSCustom fixqual(nmea, "GPGGA", 6); // $GPGGA sentence, 6th element

  // it usually takes 1 or 2 secs to get enought chars
  // for 1 nmea sentence from cold start
  // so try for 6 secs to get at least 2 NMEA sentences parsed
  // this should mean the GPS is trying to fix
  for (byte i=0; i<NMEA_TIMEOUT_SECS; i++) {
      drainNmea();
      sleep.kip1Sec();
  }

  DEBUG("nmea.passedChecksum() is:");
  DEBUGln(nmea.passedChecksum() );
  if ( nmea.passedChecksum()  < 2 ) {
    // we are bailing
    DEBUGln("ERROR: No NMEA chars from GPS device:  Check power or Serial connections");   
    return false;
  }
  // else we are good
  return true;
}

boolean checkForGPSFix() {
  
  // Check the GPS is getting a decent fix, within GPS_FIX_TIMEOUT_MSECS
  // assumes GPS in on.
  
  gpsFixTimeoutMs = millis() + GPS_FIX_TIMEOUT_MSECS;
  unsigned long timerStart = millis(); 
  gpsTimeToFixMs = 0;
  DEBUG("gpsFixTimeoutMs: ");
  DEBUGln(gpsFixTimeoutMs);
  gpsFix = false;

  while ( !gpsFixTimeoutReached && !gpsFix ) {
    // whilst we have not got a gpsFix
    // or reach the gpsFix

    drainNmea();
    printGPSData();

    if ( nmea.hdop.value() != 0 && (int)nmea.hdop.value() <= ACCEPTABLE_GPS_HDOP_FOR_FIX ) {
        DEBUGln("gpsFix is true - we have a fix!!!!");
        DEBUG("nmea.hdop.value(): ");
        DEBUGln(nmea.hdop.value());       
        gpsTimeToFixMs = millis() - timerStart;
        DEBUG("gpsTimeToFixMs: ");
        DEBUGln(gpsTimeToFixMs);
        return true;
    }
 
    if ( gpsFixTimeoutMs <= millis() ) {
      // timeout reached ...
      gpsFixTimeoutReached = true;
      DEBUGln("gpsFixTimeoutReached !!!! ");
      gpsTimeToFixMs = millis() - timerStart;
      DEBUG("gpsTimeToFixMs: ");
      DEBUGln(gpsTimeToFixMs);      
      return false;
    } 
  }
}

void printGPSData() {

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

//boolean checkGPSOn() {
  
//  fixTimeoutMs = millis() + GPS_TIMEOUT;
//  DEBUG("GPS fixTimeoutMs: ");
//  DEBUGln(fixTimeoutMs);
//  nmeaOutput = false;
  
 // while ( !gpsfixTimeoutReached && !gpsFix ) {
    // do while we do not have a fix
    // OR timedout
    
 //   DEBUG("millis(): ");
 //   DEBUGln(millis());

//    drainNmea();

//    if ( fixTimeoutMs <= millis() ) {
//      gpsfixTimeoutReached = true;
//      DEBUGln("gpsfixTimeoutReached is now true");
//      needToSendMessage = 1;
      
//      if ( messageStr.length() > 0 ) {
//        messageStr.concat(", ");  
//      }

      // add No GPS Fix message
//      messageStr.concat("NO GPS Fix.  Timed out.");     
//    }
    
//    if ( nmea.location.isValid() ) {
//      gpsFix = true;
//      DEBUGln("gpFix is now true");
//      Serial.print(nmea.location.lat(), 6);
//      Serial.print(nmea.location.lng(), 6);
//      DEBUG("hdop is: ");
//      DEBUGln(nmea.hdop.value());

//      if ( messageStr.length() > 0 ) {
//       messageStr.concat(", ");  
//      }

//      char fixMessage[50];
      // add No GPS Fix message
//      messageStr.concat("Lat: ");
      
      //}
      
//    }
//  }
  
//  DEBUG("millis(): ");
//  DEBUGln(millis());
//}

boolean drainNmea() {

  // grab all avaliable data and feed it to the gps nmea parser

  DEBUGln("drainNmea()");
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

  DEBUG("serial1Output is:");
  DEBUGln(serial1Output);
  DEBUG("nmeaOutput is:");
  DEBUGln(nmeaOutput);  
  return serial1Output;
  /*
   * nmea.hdop.value()
   * nmea.hdop.value() < 150  && nmea.hdop.value() != 0
   * nmea.location.lat()
   * nmea.location.lng()
   * nmea.location.isValid()
   */
}
