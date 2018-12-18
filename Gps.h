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
    void on(void);
    void off(void);
    boolean isOn(void);
    unsigned long getFix(unsigned long);
    TinyGPSPlus nmea;
    Sleep sleep;
  private:
    int _powerPin;
    boolean _powerState;
    boolean nmeaOutput;
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
};

#endif
