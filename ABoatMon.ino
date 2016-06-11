/*
 * Greg Cope <gregcope@gmail.com>
 * see; https://github.com/gregcope/ABoatMon
 * 
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

#define CHARGER_POWER 99
#define FONA_POWER 99
#define TEMP_POWER 99

// Static defines
#define CYCLES;

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
byte switchPosition = 0;
//unsigned long loopCount;
float batteryVolts = 4;

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
  DEBUGln("loop");

  // running so put LED on
  buttonLed.on();

  // check bildge Switch
  if ( checkBilgeSwitch() > 0 ) {
    DEBUGln("Panic!!!!");
  }

  // finished so goto sleep
  buttonLed.off();
  sleep8Secs();
}

byte checkBilgeSwitch() {

  // Function to return state of bilgeSwitch
  // We do not debounce, as we just want to know state there and then
  // If it is bouncing we will pick it up next loop
  // http://www.gammon.com.au/switches

  if (digitalRead (BILGE_SWITCH) == LOW) {
    // bilgeSwitch if on!!!!
    switchPosition = 1;
    DEBUGln("setupBilgeSwitch is on!!!!");
  } else {
    // bilgeSwitch must be off...
    switchPosition = 0;
    DEBUGln("setupBilgeSwitch is off!!!!");
  }
  return switchPosition;
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
  sleep8Secs();

  // power up
  gpsDevice.on();
  // configure GPS device
  configureGPS();
    
  DEBUGln("sleep8Sec()");

  sleep8Secs();
  sleep8Secs();
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
  DEBUGln(". ");
  Serial.flush();
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
}

void sleep2Secs() {
  // going to kip
  DEBUGln(". ");
  Serial.flush();
  LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);
}
