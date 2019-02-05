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
//#include <stdlib.h>

// internal classes/includes
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

// DS18B20
// Red connects to 27, Blue/Black connects to ground and Yellow/White (data) to 28
#define TEMP_POWER 27 // PA3
#define TEMP_DATA 28 // PA4

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
#define DAILY_CYCLES 10800 // 8 sec sleep * 10800 cycles == 86400 secs or 1 day

//#define INITIAL_GPS_FIX_TIMEOUT_MSECS 300000 // time to try and get a fix in msecs is 300 secs, or 5 mins
//#define INITIAL_GPS_FIX_TIMEOUT_MSECS 600000 // time to try and get a fix in msecs is 600 secs, or 10 mins
#define INITIAL_GPS_FIX_TIMEOUT_MSECS 900000 // time to try and get a fix in msecs is 900 secs, or 15 mins
#define INITIAL_GPS_FIX_TIMEOUT_MSECS 1200000 // time to try and get a fix in msecs is 1200 secs, or 20 mins


//#define UPDATE_GPS_FIX_TIMEOUT_MSECS 15000 // 15 secs
#define UPDATE_GPS_FIX_TIMEOUT_MSECS 60000 // 60 secs
#define UPDATE_GPS_NUMBER_OF_FIXES 15 // 10 secs

// Alarm settings
#define LOW_TEMP_ALARM 2 // or could be 0 or -1
#define HIGH_TEMP_ALARM 85
#define REG_OFF_VOLTS 12.8
#define REG_ON_VOLTS 13
#define VCC_LOW_ALARM 11.2
#define LIPO_BATTERY_ALARM 3.1


// debug functions
#define DEBUG(input)   {Serial.print(input); Serial.flush();}
#define DEBUGln(input) {Serial.println(input); Serial.flush();}

// Objects

// tinyGPS is a nmea feed parser
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
//double fixLat = 181; // invalid Lat
//double fixLng = 181; // invalid Lat
//double alarmLat = 181; // invalid Lat
//double alarmLng = 181; // invalid Lat
double orgLat = 0; // Last recorded Lat
double orgLon = 0; // Last recorded lon
double newLat = 0;
double newLon = 0;
double distance = 10000; // distance moved (default)

// LipoBattery, Vcc and Temp variables & buffer strings
float lipoVolts = 0;
char lipoStr[10];
float vccVolts= 0;
char vccStr[10];
char tempStr[11];
float tempInC;
char bilgeStr[12];
char latStr[30]; // lat string
char lonStr[30]; // lon string
char distanceStr[30];
//char disStr[6]; // in 99999m distance in 24hrs ... would not work in a fast yacht!


// Message varriables
//String messageStr = "";
char messageStr[200];
char dateTimeStr[19];

boolean sendLipoMessage = false;
boolean sendHighTempMessage = false;
boolean sendLowTempMessage = false;
boolean sendBilgeMessage = false;
boolean sendNoGpsFixMessage = false;
boolean sendGeoFenceMessage = false;
boolean sendVccMessage = false;
boolean sendDailyMessageFlag = false;

//
// Code from here on ...
//
void setup() {
  Serial.begin(9600);
  Serial.flush();
  DEBUGln("setup Start 14");
  temp.init();
  //yep burn CPU for 1/2 sec... to let stuff settle
  delay(500);
  gps.init();
  gps.getInitialFix(INITIAL_GPS_FIX_TIMEOUT_MSECS);
  DEBUGln("setup Done");
}

void loop() {

  // start of loop
  megaLed.on();
  DEBUGln("loop ...");

  // assign some defaults
  sprintf(lipoStr, "LIPO:5.0v");
  sprintf(vccStr, "VCC:50.0v");
  sprintf(tempStr, "TEMP:99.9c");
  sprintf(bilgeStr, "BILGE:OK");
  tempInC = -100;

  // do checks
  doShortChecks();
  doHourlyChecks();
  sendDailyMessage();

  // if we need to
  sendMessage();

  // done
  megaLed.off();
  sleep.kip8Secs();
}

void checkLipo(void) {

  // read lipo battery
  lipoVolts = lipo.read();

  // check if alarm
  if ( lipoVolts <= LIPO_BATTERY_ALARM ) {
    sendLipoMessage = true;
  }
  // format string ... even if we do not need it 
  dtostrf(lipoVolts,3,1,lipoStr);
  //DEBUG("lipoVolts: ");
  //DEBUGln(lipoVolts);
}

void checkVcc() {
  // read Vcc and enable regulator
  // do this each short iteration as we
  // will have to wait for temp anyway
  // so might either micro sleep or do something useful!
  vccVolts = vcc.read();
  //DEBUG("vcc is: ");
  //DEBUG(vccVolts);
  //DEBUGln("V");

  // check reg
  if ( vccVolts > REG_ON_VOLTS ) {
   vcc.regOn();
  } else if (vccVolts < REG_OFF_VOLTS ) {
   vcc.regOff(); 
  }
  if (vccVolts < VCC_LOW_ALARM ) {
   //DEBUGln("Vcc is too low!!!");
   sendVccMessage = true;
    // disbale regulator 
  }
   // format string ... even if we do not need it 
  dtostrf(vccVolts,3,1,vccStr);
  //DEBUG("vccVolts: ");
  //DEBUGln(vccVolts);
}

void checkBilge(void) {
  // check bilge switch
  if ( bilgeSwitch.isClosed() ) {
    // oh no ....
    sendBilgeMessage = true;
    sprintf(bilgeStr, "BILGE:ALARM");
  }
  //DEBUG("bilgeStr: ");
  //DEBUGln(bilgeStr);  
}

void checkTemp(void) {

  // blocking read temp
  tempInC = temp.read();
  if ( tempInC >= HIGH_TEMP_ALARM ) {
    sendHighTempMessage = true;
    DEBUGln("HIGH TEMP ALARM");
  } else if ( tempInC <= LOW_TEMP_ALARM ) {
    sendLowTempMessage = true;
    DEBUGln("LOW TEMP ALARM");  
  }
  dtostrf(tempInC,3,1,tempStr);
  //DEBUG("tempStr: ");
  //DEBUGln(tempStr);
  
}

boolean doShortChecks(void) {

  // function to do short checks each time
  // returns wether to send a message
  DEBUGln("doShortChecks");
  
  // kick off temp convert
  temp.startConvert();

  checkLipo();
  checkVcc();
  checkBilge();
  checkTemp();
  checkLocation();
  // update cyclecount and return
  cycleCount++;
  return true;  
}

//boolean doLongChecks(void) {

  // function to do long checks
  // returns weather to send a message

  //if ( cycleCount <= FOURMIN_CYCLES ) {
    // if not time yet, return false
    //return false;
  //}

  // otherwise time to do long checks
  //DEBUGln("doLongChecks: ");

  //checkLocation();
  //gps.getUpdatedFix(UPDATE_GPS_FIX_TIMEOUT_MSECS, UPDATE_GPS_NUMBER_OF_FIXES);  

  //return true;
//}

boolean doHourlyChecks(void) {
  // function to do hourly checks
  // returns wether to send a message

  if ( cycleCount <= HOUR_CYCLES ) {
    DEBUG("cycleCount is: ");
    DEBUG(cycleCount);
    DEBUGln(", doHourlyChecks : Not running now!");
    // if not time yet, return false
    return false;
  }
  // time to do hour checks
  DEBUGln("doHourlyChecks: ");

  checkLocation();
    
  // TODO: calc number of cycles left to get to hourly checks
  
  // send a message?
  return true;
}

boolean sendDailyMessage(void) {


  if ( cycleCount <= DAILY_CYCLES ) {
    DEBUG("cycleCount is: ");
    DEBUG(cycleCount);
    DEBUGln(", sendDailyMessage : Not running now!");
    // if not time yet, return false
    return false;
  }
  // function to do daily checks
  // always returns true as we want to send a message
  DEBUGln("set sendDailyMessageFlag to true");
  // send a message?
  sendDailyMessageFlag = true;
  return true;
}

void checkLocation(void) {

  // update location
  // TODO: Logic if no updated fix
  gps.getUpdatedFix(UPDATE_GPS_FIX_TIMEOUT_MSECS, UPDATE_GPS_NUMBER_OF_FIXES);
  // check location vs last location
  // work out distance(orgLat, orgLon, lat, lon);
  newLat = gps.getLat();
  newLon = gps.getLon();
//  distance = gps.distanceMoved(orgLat, orgLon);
  distance = gps.haversine(newLat, newLon, orgLat, orgLon);
  // save where we are now
  orgLat = newLat;
  orgLon = newLon;

  // convert Double to string
  // https://ukhas.org.uk/guides:common_coding_errors_payload_testing
  dtostrf(newLat, 10, 6, latStr);  //first number is length, last is numbers after decimal
  dtostrf(newLon, 10, 6, lonStr);

  dtostrf(distance, 9,2, distanceStr);
}

void sendMessage(void) {
  // check to send a message?
  // basically ANY alarms OR daily message
  if ( !sendLipoMessage || !sendHighTempMessage || !sendLowTempMessage || !sendBilgeMessage || !sendNoGpsFixMessage || !sendGeoFenceMessage || !sendVccMessage || !sendDailyMessageFlag ) {
    // no need to send a message
    //return;  
  }

  // send a message!!!!
  // like 20190127T20:20:20-LIPO:5.0v-VCC:50.0v-TEMP:99.9c,BILGE:OK,LAT:NNNNN:LON:YYYYYY:DIS:NNNNNm
  
  //Get a dateTime String
  sprintf(dateTimeStr, gps.getdateTime());

  // going to try and pinch some power;
  vcc.regOn();

  //Put the message together
  sprintf(messageStr, "'%s,%sv,%sv,%sc,%s,%s,%s,%sm'", dateTimeStr, lipoStr, vccStr, tempStr, bilgeStr, latStr, lonStr, distanceStr);  
  DEBUG("Message is: ");
  DEBUGln(messageStr);
  
  // message sent tidy up
  // done swich off regulator
  vcc.regOff();
  
  // clear flags
  sendLipoMessage = false;
  sendHighTempMessage = false;
  sendLowTempMessage = false;
  sendBilgeMessage = false;
  sendNoGpsFixMessage = false;
  sendGeoFenceMessage = false;
  sendVccMessage = false;
  sendDailyMessageFlag = false;
  // done
}
