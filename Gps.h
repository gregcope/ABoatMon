#ifndef Gps_h
#define Gps_h
/*
 * gps
 * 
 * Library to manage a gps device
 * 
 * Greg Cope <greg.cope@gmail.com>
 * 
 */

#include <Arduino.h>
// https://github.com/mikalhart/TinyGPSPlus/releases 
#include <TinyGPS++.h>
#include "Sleep.h"

// debug functions
#define DEBUG(input)   {Serial.print(input); Serial.flush();}
#define DEBUGln(input) {Serial.println(input); Serial.flush();}
#define ACCEPTABLE_GPS_HDOP_FOR_FIX 160
#define GOOD_GPS_HDOP_FOR_FIX 145

class Gps
{
  public:
    Gps(byte pin);
    boolean on(void);
    void off(void);
    boolean isOn(void);
    boolean updateFix(unsigned long);
    unsigned long getInitialFix(unsigned long);
   // Sleep sleep;
  private:
    int _powerPin;
    boolean _powerState;
    boolean nmeaOutput;
    int nmeaUpdated;
    unsigned long nmeaTimeoutMs;
    boolean serial1Output;
    unsigned long gpsFixTimeoutMs;
    unsigned long gpsTimerStart;
    unsigned long gpsTimeToFixMs;
    boolean gpsFix;
    boolean gpsFixTimeoutReached;
    void setupGPS(void);
    boolean drainNmea(void);
    void printGPSData(void);
    int initialHDOP;
    int finalHDOP;
    int hdop; 
    TinyGPSPlus nmea;
    Sleep sleep;
    //int nmeaUpdated;
};

#endif
