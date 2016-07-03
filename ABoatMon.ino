/*
 * Greg Cope <gregcope@gmail.com>
 * see; https://github.com/gregcope/ABoatMon
 * 
 * Allot of code pinched from;
 * https://github.com/LowPowerLab/RFM69/blob/master/Examples/MotionMote/MotionMote.ino
 */ 

// External includes
// https://github.com/mikalhart/TinyGPSPlus/releases 
#include <TinyGPS++.h>
// https://github.com/LowPowerLab/LowPower
#include <LowPower.h>
// http://www.pjrc.com/teensy/td_libs_OneWire.html
#include <OneWire.h>

// internal classes/includes
#include "Device.h"
#include "Config.h"

// PIN defines
#define GPS_POWER 31
#define GSM_TX 8  // serial
#define GSM_RX 9  // serial
#define GPS_TX 10 // serial1
#define GPS_RX 11 // serial1

#define BUTTON_LED 12
#define BILGE_SWITCH 13
#define BATT_MONITOR A0
#define TEMP_POWER 30

// TBC
#define CHARGER_POWER 99
#define FONA_POWER 99

// Static defines
#define FOURMIN_CYCLES 5 // 8 sec sleep * 30 cycles = 240 secs or 4 mins
#define HOUR_CYCLES 450 // 8 sec sleep * 450 cyles == 3600 secs or 1 hour

// 3.3 (Vcc) / 1024 (ADC precision) =  0.00322
// Motineo example 1m+470k = 470 / ( 1000 +470 ) = 0.32 rounded up
// Ours 1.8m + 3.3m = 3300000 / ( 1800000 + 3300000 ) = 0.647 or 1.55 (inverse)
#define BATT_FORMULA(reading) reading * 0.00322 * 1.5455 // >>> fine tune this parameter to match your voltage when fully charged
                                                       // details on how this works: https://lowpowerlab.com/forum/index.php/topic,1206.0.html

#define GPS_TIMEOUT 120000 // time to try and get a fix in msecs (120 secs)
// #define GPS_TIMEOUT 800


// debug functions
#define DEBUG(input)   {Serial.print(input); Serial.flush();}
#define DEBUGln(input) {Serial.println(input); Serial.flush();}

// Objects
TinyGPSPlus nmea;

Device gpsDevice(GPS_POWER);
Device buttonLed(BUTTON_LED);
Device tempSensor(TEMP_POWER);

Config config;

//Device charger(CHARGER_POWER);
//Device fona(FONA_POWER);
//Device tempSensor(TEMP_POWER);

// Variables
byte bilgeSwitchPosition = 0;

// Cycle Vars
unsigned long cycleCount = 0;
byte fourMinCycleCount = 0;
unsigned int hourCycleCount = 0;
unsigned long NOW = 0;

// GPS / Nmea vars
unsigned long fixTimeoutMs = 0;
boolean gpsFix = false;
boolean gpsfixTimeoutReached = false;
double fixLat = 100; // invalid Lat
double fixLng = 100; // invalid Lat
double alarmLat = 100; // invalid Lat
double alarmLng = 100; // invalid Lat

// LipoBattery
float lipoBatteryAlarm = 4.4;
float lipoBatteryVolts= 4;
unsigned int lipoBatteryReadings = 0;
char lipoBatteryVoltsString[10]; //longest battery voltage reading message = 9chars

// Message vars
String messageStr = "";
byte needToSendMessage = 0;
byte batMessageSent = 0;
byte bilgeMessageSent = 0;
byte gpsNoFixMessageSent = 0;


void setup() {

  // as we are on, fire up LED
  buttonLed.on();  

  // Clear the serial ...
  Serial.begin(9600); 
  Serial.flush();
  delay(1000);
  DEBUGln("setup start");

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
  
 
  delay(100000);

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
  sleep8Secs();
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
    analogRead(BATT_MONITOR);
    for (byte i=0; i<10; i++) {
      //take 10 samples, and average
      lipoBatteryReadings+=analogRead(BATT_MONITOR);
    }
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
  pinMode (BILGE_SWITCH, INPUT_PULLUP);
  DEBUGln("setupBilgeSwitch");  
}

void setupGSM() {
  DEBUGln("setupGSM");  
}

void setupGPS() {

  DEBUGln("setupGPS");
  // Setup the GPS from first use or hardreset

  // power up
  gpsDevice.on();
  // configure GPS device
  configureGPS();
  //gpsDevice.off();

}

void configureGPS() {

  DEBUGln("configureGPS()");
    
  // Configure GPS settings at startUp/Power up
  Serial1.begin(9600); 
  Serial1.flush();
  
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

  // set timeout
  fixTimeoutMs = millis() + GPS_TIMEOUT;
  DEBUG("fixTimeoutMs: ");
  DEBUGln(fixTimeoutMs);
  while ( !gpsfixTimeoutReached && !gpsFix ) {
    // do stuff
    
    DEBUG("millis(): ");
    DEBUGln(millis());

    drainNmea();

    if ( fixTimeoutMs <= millis() ) {
      gpsfixTimeoutReached = true;
      DEBUGln("gpsfixTimeoutReached is now true");
      needToSendMessage = 1;
      
      if ( messageStr.length() > 0 ) {
        messageStr.concat(", ");  
      }

      // add No GPS Fix message
      messageStr.concat("NO GPS Fix.  Timed out.");     
    }
    
    if ( nmea.location.isValid() ) {
      gpsFix = true;
      DEBUGln("gpFix is now true");
      Serial.print(nmea.location.lat(), 6);
      Serial.print(nmea.location.lng(), 6);
      DEBUG("hdop is: ");
      DEBUGln(nmea.hdop.value());

      if ( messageStr.length() > 0 ) {
        messageStr.concat(", ");  
      }

      char fixMessage[50];
      // add No GPS Fix message
      messageStr.concat("Lat: ");
      
           
      //}
      
    }
  }
  
  DEBUG("millis(): ");
  DEBUGln(millis());
}

void drainNmea() {

  // grab all avaliable data and feed it to the gps nmea parser

  DEBUGln("drainNmea()");
    
  while (Serial1.available() > 0) {
    if (nmea.encode(Serial1.read())) {
      buttonLed.on();
      DEBUGln("got gps summat");
      buttonLed.off();
    }
  }
  DEBUG("nmea chars processed: ");
  DEBUGln(nmea.charsProcessed());

  /*
   * nmea.hdop.value()
   * nmea.hdop.value() < 150  && nmea.hdop.value() != 0
   * nmea.location.lat()
   * nmea.location.lng()
   * nmea.location.isValid()
   */
}

void sleep8Secs() {
  // going to kip 8 Secs
  DEBUGln("8 ");
  Serial.flush();
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
}

void sleep2Secs() {
  // going to kip 2 Secs
  DEBUGln("2 ");
  Serial.flush();
  LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);
}
