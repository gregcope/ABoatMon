

// includes
#include "TinyGPS++.h"
#include "Device.h"

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

void setup() {
  // put your setup code here, to run once:
  setupGPS();
}

void loop() {
  // put your main code here, to run repeatedly:

}

void setupGPS() {

  // power up and configure GPS device
  gpsDevice.on();
  Serial1.begin(9600); 
  // http://forums.adafruit.com/viewtopic.php?f=19&p=143502
  Serial1.print("$PMTK313,1*2E\r\n");  // Enable to search a SBAS satellite
  Serial1.print("$PMTK301,2*2E\r\n");  // Enable WAAS as DGPS Source
  // https://code.google.com/p/ardupilot-mega/source/browse/libraries/AP_GPS/AP_GPS_NMEA.h?spec=svneff9f9d0906c1e3cd4e217b0363d0f9d44394e75&name=f401de6d52&r=eff9f9d0906c1e3cd4e217b0363d0f9d44394e75
  Serial1.print("$PMTK220,1000*1F\r\n"); // 1HZ
  //Serial1.print("$PSRF151,1*3F\r\n"); // WAAS_ON
  //Serial1.print("$PMTK513,1*28\r\n"); // Search for SBAS Sat
  DEBUGln("setupGPS");
}
