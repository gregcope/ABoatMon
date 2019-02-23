#ifndef Gns_h
#define Gns_h

/*
 * gps
 * 
 * Library to manage a gns device with NeoGPS
 * 
 * Greg Cope <greg.cope@gmail.com>
 * 
 */

#include <Arduino.h>
//https://github.com/SlashDevin/NeoGPS/blob/master/extras/doc/Installing.md
#include <TinyGPS++.h>
#include "Sleep.h"

// debug functions
#define DEBUG(input)   {Serial.print(input); Serial.flush();}
#define DEBUGln(input) {Serial.println(input); Serial.flush();}

//#define ACCEPTABLE_GPS_HDOP_FOR_FIX 200 // was 160
//#define GOOD_GPS_HDOP_FOR_FIX 145

#define ACCEPTABLE_GPS_HDOP_FOR_FIX 400 // was 160
#define GOOD_GPS_HDOP_FOR_FIX 200

class Gns
{
  public:
    Gns(byte pin);
    void init(void);
    boolean on(void);
    void off(void);
    boolean isOn(void);
    //boolean updateFix(unsigned long, int);
    //unsigned long getInitialFix(unsigned long);
    //boolean getUpdatedFix(unsigned long, int);
    //boolean drainNmea(void);
    //char* getdateTime(void);
    //void getLocation(double&, double&);
    //double distanceMoved(double, double);
    //void distanceMoved(double&, double&, double&);
    //double haversine(double, double, double, double);
  private:
    int _powerPin;
    boolean _powerState;
    //boolean nmeaOutput;
    //int nmeaUpdates;
    //unsigned long nmeaTimeoutMs;
    //boolean serial1Output;
    //unsigned long gpsFixTimeoutMs;
    //unsigned long now;
    //unsigned long gpsTimerStart;
    //unsigned long gpsTimeToFixMs;
    //boolean gpsFixTimeoutReached;
    //void setupGPS(void);
    //void printGPSData(void);
    //int initialHDOP;
    //int finalHDOP;
    //int hdop; 
    //TinyGPSPlus nmea;
    //TinyGPSCustom antenna;
    //TinyGPSCustom fixqual;
    Sleep sleep;
    //char _dateTime[17];
    //int nmeaUpdated;
    //double _distance;
};



#endif
