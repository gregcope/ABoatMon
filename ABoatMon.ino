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

// internal classes/includes
//#include "Device.h"
//#include "Config.h"
#include "Button.h"
#include "Sleep.h"
#include "Gps.h"
#include "Lipo.h"
#include "Vcc.h"
#include "Fona.h"
#include "Led.h"
#include "Temp.h"

// PIN defines
#define MEGA_LED 15
#define D12_GPS_ENABLE 12
#define BUTTON_LED 26
#define BUTTON 25
#define BILGE_SWITCH 3 // Other line from BilgeSwitch to GND

//#define LIPO_VOLTAGE_DIVIDER 24 // D24 is same as A0
const int LIPO_VOLTAGE_DIVIDER = 0;

#define TEMP_POWER 27 // PA3
#define TEMP_DATA 28 // PA4
// Red connects to 27, Blue/Black connects to ground and Yellow/White (data) to 28

#define VCC_12V_24V_VOLTAGE_DIVIDER A5 
#define VCC_12V_24V_ENABLE 19

#define FONA_TX 8  // serial
#define FONA_RX 9  // serial
#define FONA_POWER 30
#define FONA_KEY 14
#define FONA_PS 18
#define FONA_NETSTAT 13

// Static defines
#define FOURMIN_CYCLES 30 // 8 sec sleep * 30 cycles = 240 secs or 4 mins
#define HOUR_CYCLES 450 // 8 sec sleep * 450 cyles == 3600 secs or 1 hour

//#define INITIAL_GPS_FIX_TIMEOUT_MSECS 300000 // time to try and get a fix in msecs is 300 secs, or 5 mins
//#define INITIAL_GPS_FIX_TIMEOUT_MSECS 600000 // time to try and get a fix in msecs is 600 secs, or 10 mins
#define INITIAL_GPS_FIX_TIMEOUT_MSECS 900000 // time to try and get a fix in msecs is 900 secs, or 15 mins

//#define UPDATE_GPS_FIX_TIMEOUT_MSECS 15000 // 15 secs
#define UPDATE_GPS_FIX_TIMEOUT_MSECS 60000 // 60 secs
#define UPDATE_GPS_NUMBER_OF_FIXES 15 // 10 secs

// Temp alarm settings
#define LOW_TEMP_ALARM 2 // or could be 0 or -1
#define HIGH_TEMP_ALARM 85

// debug functions
#define DEBUG(input)   {Serial.print(input); Serial.flush();}
#define DEBUGln(input) {Serial.println(input); Serial.flush();}

// Objects

// tinyGPS is a nmea feed parser
//TinyGPSPlus nmea;
Sleep sleep;
Gps gps(D12_GPS_ENABLE);
Lipo lipo(LIPO_VOLTAGE_DIVIDER);
Vcc vcc(VCC_12V_24V_ENABLE, VCC_12V_24V_VOLTAGE_DIVIDER);
Fona fona(FONA_POWER);
Led megaLed(MEGA_LED);
Led switchLed(BUTTON_LED);
Temp temp(TEMP_POWER, TEMP_DATA);
Button bilgeSwitch(BILGE_SWITCH);

// Cycle Vars
unsigned long cycleCount = 0;
//byte fourMinCycleCount = 0;
//unsigned int hourCycleCount = 0;
//unsigned long NOW = 0;

// GPS / Nmea vars
//unsigned long nmeaSentenceTimeOutMs = 0;
//boolean nmeaSentenceTimeoutReached = false;
double fixLat = 181; // invalid Lat
double fixLng = 181; // invalid Lat
double alarmLat = 181; // invalid Lat
double alarmLng = 181; // invalid Lat

// LipoBattery
float lipoBatteryAlarm = 3.1;
float lipoBatteryVolts = 0;
char lipoBatteryVoltsString[10]; //longest battery voltage reading message = 9chars

// Vcc
float vccVoltage = 0;

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
  DEBUGln("setup Start 12");
  temp.init();
  //yep burn CPU for 1/2 sec... to let stuff settle
  delay(500);
  //gps.init();
  //gps.getInitialFix(INITIAL_GPS_FIX_TIMEOUT_MSECS);
  DEBUGln("setup Done");
}

void loop() {

  // start of loop
  megaLed.on();
  DEBUGln("loop ...");

  // do checks
  doShortChecks();
  doLongChecks();
  doDailyChecks();

  // done
  megaLed.off();
  sleep.kip8Secs();
}

boolean doShortChecks(void) {

  // function to do short checks each time
  // returns wether to send a message
  DEBUGln("doShortChecks");
  
  // kick off temp convert
  temp.startConvert();

  // check bilge switch
  if ( bilgeSwitch.isClosed() ) {
    // oh no ....
    DEBUGln("bilge switch is closed... oh uh!"); 
  }

  // read battery
  //lipoBatteryVolts = lipo.read();

  // read Vcc and enable regulator
  vccVoltage = vcc.read();
  if ( vccVoltage > 13 ) {
    //enable regulator
  } else {
    // disbale regulator 
  }

  // read temp
  DEBUG("Temp is: ");
  float tempInC = temp.read();
  DEBUG(tempInC);
  DEBUG(", ");
  if ( tempInC >= HIGH_TEMP_ALARM ) {
    DEBUGln("HIGH TEMP ALARM"); 
  } else if ( tempInC <= LOW_TEMP_ALARM ) {
    DEBUGln("LOW TEMP ALARM");  
  }

  // finish
  cycleCount++;
  return true;  
}

boolean doLongChecks(void) {

  // function to do long checks
  // returns weather to send a message

  if ( cycleCount <= FOURMIN_CYCLES ) {
    // if not time yet, return false
    return false;
  }

  // otherwise time to do long checks
  DEBUGln("doLongChecks: ")
  //gps.getUpdatedFix(UPDATE_GPS_FIX_TIMEOUT_MSECS, UPDATE_GPS_NUMBER_OF_FIXES);  
  return true;
}

boolean doHourlyChecks(void) {
  // function to do hourly checks
  // returns wether to send a message

  if ( cycleCount <= HOUR_CYCLES ) {
    // if not time yet, return false
    return false;
  }
  // time to do hour checks
  DEBUGln("doHourlyChecks: ")

  gps.getUpdatedFix(UPDATE_GPS_FIX_TIMEOUT_MSECS, UPDATE_GPS_NUMBER_OF_FIXES);
    
  // TODO: calc number of cycles left to get to hourly checks

  return true;
}

boolean doDailyChecks(void) {

  // function to do daily checks
  // always returns true as we want to send a message
  DEBUGln("doDailyChecks: ")
  return true;
}
