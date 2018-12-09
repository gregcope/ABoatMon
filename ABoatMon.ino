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
#define D12_GPS_ENABLE 12
#define GPS_TX 10 // serial1
#define GPS_RX 11 // serial1

//#define BUTTON_LED 26
#define BUTTON_LED 15
#define BUTTON 25
 
#define BILGE_SWITCH 3 // Other line from BilgeSwitch to GND

#define LIPO_VOLTAGE_DIVIDER A0

#define TEMP_POWER 27 // PA3
#define TEMP_DATA 28 // PA4
// Red connects to 27, Blue/Black connects to ground and Yellow/White (data) to 28

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
Device gpsDevice(D12_GPS_ENABLE);
//Device buttonLed(BUTTON_LED);
//Device tempSensor(TEMP_POWER);
// Make one wire faster
// http://www.cupidcontrols.com/2014/10/moteino-arduino-and-1wire-optimize-your-read-for-speed/

//Config config;

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
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial.flush();
  DEBUGln("setup Start");
  sleep.kip1Sec();
  setupGPS();

  DEBUGln("setup Done,,,,,");
}

void loop() {
  //DEBUGln("loop ...");
  //sleep.kip1Sec();
   if (Serial1.available()) {
    Serial.write(Serial1.read());
  }

}

void setupGPS() {

  DEBUGln("setupGPS");
  switchOnGPS();
  if (gpsDevice.isOn()) {
      DEBUGln("gpsDevice.isOn");
  }
  // http://forums.adafruit.com/viewtopic.php?f=19&p=143502
  DEBUGln("Enable SBAS sat search");
  Serial1.print("$PMTK313,1*2E\r\n");  // Enable to search a SBAS satellite
  DEBUGln("Enable WAAS as DPGS source");
  Serial1.print("$PMTK301,2*2E\r\n");  // Enable WAAS as DGPS Source
  // https://code.google.com/p/ardupilot-mega/source/browse/libraries/AP_GPS/AP_GPS_NMEA.h?spec=svneff9f9d0906c1e3cd4e217b0363d0f9d44394e75&name=f401de6d52&r=eff9f9d0906c1e3cd4e217b0363d0f9d44394e75
  DEBUGln("Set GPS hz to 1hz");
  Serial1.print("$PMTK220,1000*1F\r\n"); // 1HZ
  // http://aprs.gids.nl/nmea/#gga
  // 0 = Invalid, 1 = GPS fix, 2 = DGPS fix
  // 1 in this example
  // $GPGGA,064951.000,2307.1256,N,12016.4438,E,1,8,0.95,39.9,M,17.8,M,,*65
  TinyGPSCustom fixqual(nmea, "GPGGA", 6); // $GPGGA sentence, 6th element
  for (byte i=0; i<NMEA_TIMEOUT_SECS; i++) {
      drainNmea();
      sleep.kip1Sec();
  }

}


// Helper function after here

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

}

void switchOnGPS() {
    gpsDevice.on();
    Serial1.begin(9600);
    Serial1.flush();
}

void switchOffGPS() {
    Serial1.flush();
    gpsDevice.off();
}
