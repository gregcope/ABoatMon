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
#include "Gps.h"
#include "Lipo.h"

// PIN defines
#define D12_GPS_ENABLE 12
//#define GPS_TX 10 // serial1
//#define GPS_RX 11 // serial1

//#define BUTTON_LED 26
#define BUTTON_LED 15
#define BUTTON 25
 
#define BILGE_SWITCH 3 // Other line from BilgeSwitch to GND

//#define LIPO_VOLTAGE_DIVIDER 0
const int LIPO_VOLTAGE_DIVIDER = 0;

#define TEMP_POWER 27 // PA3
#define TEMP_DATA 28 // PA4
// Red connects to 27, Blue/Black connects to ground and Yellow/White (data) to 28

#define VCC_12V-24V_VOLTAGE_DIVIDER A5 
#define VCC_12V-24V_ENABLE 19

#define FONA_TX 8  // serial
#define FONA_RX 9  // serial
#define FONA_POWER 30
#define FONA_KEY 14
#define FONA_PS 18
#define FONA_NETSTAT 13

// Static defines
#define FOURMIN_CYCLES 5 // 8 sec sleep * 30 cycles = 240 secs or 4 mins
#define HOUR_CYCLES 450 // 8 sec sleep * 450 cyles == 3600 secs or 1 hour

#define GPS_FIX_TIMEOUT_MSECS 300000 // time to try and get a fix in msecs is 300 secs, or 5 mins

// debug functions
#define DEBUG(input)   {Serial.print(input); Serial.flush();}
#define DEBUGln(input) {Serial.println(input); Serial.flush();}

// Objects

// tinyGPS is a nmea feed parser
TinyGPSPlus nmea;
Sleep sleep;
Gps gps(D12_GPS_ENABLE);
Lipo lipo(LIPO_VOLTAGE_DIVIDER);
// these are devices, and have physical on/off/interface things
//Device gpsDevice(D12_GPS_ENABLE);
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
//boolean nmeaSentenceTimeoutReached = false;
double fixLat = 100; // invalid Lat
double fixLng = 100; // invalid Lat
double alarmLat = 100; // invalid Lat
double alarmLng = 100; // invalid Lat

// LipoBattery
float lipoBatteryAlarm = 3.1;
float lipoBatteryVolts = 0;
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
  Serial.flush();
  DEBUGln("setup Start");
  //yep burn CPU for 1 sec... to let stuff settle
  delay(1000);
  gps.getFix(GPS_FIX_TIMEOUT_MSECS);
  DEBUG("Lipo volts: ");
  DEBUG(lipo.read());
  DEBUGln(".");
  DEBUGln("setup Done");
}

void loop() {
  DEBUGln("loop ...");
  gps.getFix(GPS_FIX_TIMEOUT_MSECS);
  DEBUG("Lipo volts: ");
  DEBUG(lipo.read());
  DEBUGln(".");
  sleep.kip8Secs();
}
