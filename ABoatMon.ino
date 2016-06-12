/*
 * Greg Cope <gregcope@gmail.com>
 * see; https://github.com/gregcope/ABoatMon
 * 
 * Allot of code pinched from;
 * https://github.com/LowPowerLab/RFM69/blob/master/Examples/MotionMote/MotionMote.ino
 */ 

// includes
// External
// https://github.com/mikalhart/TinyGPSPlus/releases 
#include "TinyGPS++.h"
// https://github.com/LowPowerLab/LowPower
#include <LowPower.h>
// http://www.pjrc.com/teensy/td_libs_OneWire.html

// internal classes/includes
#include "Device.h"

// PIN defines
#define GPS_POWER 31
#define BUTTON_LED 12
#define BILGE_SWITCH 13
#define BATT_MONITOR A0

#define CHARGER_POWER 99
#define FONA_POWER 99
#define TEMP_POWER 99

// Static defines
#define FOURMIN_CYCLES 5 // 8 sec sleep * 30 cycles = 240 secs or 4 mins
#define HOUR_CYCLES 450 // 8 sec sleep * 450 cyles == 3600 secs or 1 hour

// 3.3 (Vcc) / 1024 (ADC precision) =  0.00322
// Motineo example 1m+470k = 470 / ( 1000 +470 ) = 0.32 rounded up
// Ours 1.8m + 3.3m = 3300000 / ( 1800000 + 3300000 ) = 0.647 or 1.55 (inverse)
#define BATT_FORMULA(reading) reading * 0.00322 * 1.5455 // >>> fine tune this parameter to match your voltage when fully charged
                                                       // details on how this works: https://lowpowerlab.com/forum/index.php/topic,1206.0.html
// debug functions
#define DEBUG(input)   {Serial.print(input); Serial.flush();}
#define DEBUGln(input) {Serial.println(input); Serial.flush();}

// Objects
TinyGPSPlus gps;
Device gpsDevice(GPS_POWER);
Device buttonLed(BUTTON_LED);
//Device charger(CHARGER_POWER);
//Device led(BUTTON_LED);
//Device fona(FONA_POWER);
//Device tempSensor(TEMP_POWER);

// Variables
byte bilgeSwitchPosition = 0;

unsigned long cycleCount = 0;
byte fourMinCycleCount = 0;
unsigned int hourCycleCount = 0;

float BatteryAlarm = 4.4;
float batteryVolts = 4;
unsigned int batteryReadings = 0;
char BatteryVoltsString[10]; //longest battery voltage reading message = 9chars

// Message vars
String messageStr = "";
byte needToSendMessage = 0;
byte batMessageSent = 0;
byte bilgeMessageSent = 0;

void setup() {

  // as we are on, fire up LED
  buttonLed.on();  
  Serial.begin(9600); 
  Serial.flush();
  DEBUGln("setup start");

  // let everything settle
  sleep2Secs();

  // For sanity have setup functions per thing
  setupBilgeSwitch();
  setupGPS();
  setupGSM();
  buttonLed.off();
  DEBUGln("setup Done");
}

void loop() {
  // put your main code here, to run repeatedly:
  DEBUG("loop: ");

  // up cycle Count
  cycleCount ++;
  DEBUGln(cycleCount);
  
  // running so put LED on
  buttonLed.on();

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
  
  if ( fourMinCycleCount++ == FOURMIN_CYCLES) {
    DEBUG("4 mins check, fourMinCycleCount: ");
    DEBUGln(fourMinCycleCount);
    
    checkBattery();
    //flipCharger();
    
    // reset counter
    fourMinCycleCount = 0;
  } 

  if ( hourCycleCount++ == HOUR_CYCLES) {
    DEBUGln("Top of the hour ...");

    checkBattery();
    //checkPosition();

    // reset counter
    hourCycleCount = 0;
  }
  
   // do we need to send a message
  if ( needToSendMessage == 1 ) {
    DEBUGln("Need to send massage");
    sendMessage();
  }

  // finished so goto sleep
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

void checkBattery() {
    DEBUGln("checkBattery() .. battery readings: ");
    batteryReadings = 0 ;
    // read it first and throw it away
    analogRead(BATT_MONITOR);
    for (byte i=0; i<10; i++) {
      //take 10 samples, and average
      batteryReadings+=analogRead(BATT_MONITOR);
    }
    //DEBUGln(batteryReadings);
    // work out the volts from the ADC value
    batteryVolts = BATT_FORMULA(batteryReadings / 10.0);
    DEBUGln(batteryVolts);

    // work out if we need to bleat
    if ( batteryVolts < BatteryAlarm ) {
      dtostrf(batteryVolts, 3,2, BatteryVoltsString); //update the BATStr which gets sent every BATT_CYCLES or along with the MOTION message
      needToSendMessage = 1;
      messageStr.concat("BattLow: ");
      messageStr.concat(BatteryVoltsString);
      messageStr.concat("V");
      //messageStr = "BattLow: " + BatteryVoltsString + "V";
    }
}

byte checkBilgeSwitch() {

  // Function to return state of bilgeSwitch
  // We do not debounce, as we just want to know state there and then
  // If it is bouncing we will pick it up next loop
  // http://www.gammon.com.au/switches

  if (digitalRead (BILGE_SWITCH) == LOW) {
    // bilgeSwitch if on!!!!
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
  sleep2Secs();

  // power up
  gpsDevice.on();
  // configure GPS device
  configureGPS();

  sleep2Secs();
  DEBUGln("gpsDevice.off()");
  gpsDevice.off();
  // start tracking
  // get A fix
  // record fix in 
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
}

void sleep8Secs() {
  // going to kip
  DEBUGln("8 ");
  Serial.flush();
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
}

void sleep2Secs() {
  // going to kip
  DEBUGln("2 ");
  Serial.flush();
  LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);
}
