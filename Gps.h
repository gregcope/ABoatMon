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
#define ACCEPTABLE_GPS_HDOP_FOR_FIX 200 // was 160
#define GOOD_GPS_HDOP_FOR_FIX 145

class Gps
{
  public:
    Gps(byte pin);
    boolean on(void);
    void off(void);
    boolean isOn(void);
    boolean updateFix(unsigned long, int);
    unsigned long getInitialFix(unsigned long);
    boolean getUpdatedFix(unsigned long, int);
    boolean drainNmea(void);
    void init(void);
  private:
    int _powerPin;
    boolean _powerState;
    boolean nmeaOutput;
    int nmeaUpdates;
    unsigned long nmeaTimeoutMs;
    boolean serial1Output;
    unsigned long gpsFixTimeoutMs;
    unsigned long now;
    unsigned long gpsTimerStart;
    unsigned long gpsTimeToFixMs;
    boolean gpsFixTimeoutReached;
    void setupGPS(void);
    void printGPSData(void);
    int initialHDOP;
    int finalHDOP;
    int hdop; 
    TinyGPSPlus nmea;
    TinyGPSCustom antenna;
    TinyGPSCustom fixqual;
    Sleep sleep;
    //int nmeaUpdated;
};

#endif
