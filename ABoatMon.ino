

// includes
#include "TinyGPS++.h"
#include "Device.h"
#include <LowPower.h>

// PIN defines
#define GPS_POWER 0

#define BUTTON_LED 13
#define CHARGER_POWER 14

// debug functions

#define DEBUG(input)   {Serial.print(input); Serial.flush();}
#define DEBUGln(input) {Serial.println(input); Serial.flush();}

// Objects
TinyGPSPlus gps;
Device gpsDevice(GPS_POWER); 
Device charger(CHARGER_POWER);
Device led(BUTTON_LED);
//Device fona
//Device tempSensor

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); 
  Serial.flush();
  sleep8Secs();
  DEBUGln("setup");
  setupGPS();
  setupGPS();
  setupGSM();
}

void loop() {
  // put your main code here, to run repeatedly:

}

void setupGSM() {
  DEBUGln("setupGSM");  
}

void setupGPS() {

  // Setup the GPS from first use or hardreset
  sleep8Secs();

  // power up
  DEBUGln("gpsDevice.on()");
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
  // Configure GPS settings at startUp/Power up
  Serial.begin(9600); 
  DEBUGln("configureGPS");
  
  // http://forums.adafruit.com/viewtopic.php?f=19&p=143502
  Serial.print("$PMTK313,1*2E\r\n");  // Enable to search a SBAS satellite
  Serial.print("$PMTK301,2*2E\r\n");  // Enable WAAS as DGPS Source
  // https://code.google.com/p/ardupilot-mega/source/browse/libraries/AP_GPS/AP_GPS_NMEA.h?spec=svneff9f9d0906c1e3cd4e217b0363d0f9d44394e75&name=f401de6d52&r=eff9f9d0906c1e3cd4e217b0363d0f9d44394e75
  Serial.print("$PMTK220,1000*1F\r\n"); // 1HZ
  //Serial1.print("$PSRF151,1*3F\r\n"); // WAAS_ON
  //Serial1.print("$PMTK513,1*28\r\n"); // Search for SBAS Sat
}

void sleep8Secs() {
  // going to kip
  DEBUG(". ");
  Serial.flush();
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
}

void sleep2Secs() {
  // going to kip
  DEBUG(". ");
  Serial.flush();
  LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);
}
