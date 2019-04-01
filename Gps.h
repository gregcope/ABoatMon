#ifndef Gps_h
#define Gps_h
/*
 * gps
 * 
 * Library to manage a gps device with tinyGPS++
 * 
 * Greg Cope <greg.cope@gmail.com>
 * 
 */

#include <Arduino.h>
// https://github.com/mikalhart/TinyGPSPlus/releases 
#include <TinyGPS++.h>
#include "Sleep.h"

// Notes
  // converting rawLat deg / billionths into longs
  // https://www.disk91.com/2016/technology/internet-of-things-technology/simple-lora-gps-tracker-based-on-rn2483-and-l80/
  // payload testing
  // https://ukhas.org.uk/guides:common_coding_errors_payload_testing
  // simple GPS lib
  // http://www.technoblogy.com/show?10WT
  // distance using lat/lon * 1000000
  // https://forum.arduino.cc/index.php?topic=393511.msg3232854#msg3232854
  // we have billionths
  // e.g. 0.449678333*1000000000 = 449678333

// debug functions
#define DEBUG(input)   {Serial.print(input); Serial.flush();}
#define DEBUGln(input) {Serial.println(input); Serial.flush();}
//#define ACCEPTABLE_GPS_HDOP_FOR_FIX 200 // was 160
//#define GOOD_GPS_HDOP_FOR_FIX 145

#define ACCEPTABLE_GPS_HDOP_FOR_FIX 400 // was 160
#define GOOD_GPS_HDOP_FOR_FIX 200

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
    char* getdateTime(void);
    //void getdateTime(char*);
    void init(void);
    void getLocation(double&, double&);
    void getRawLocation(char &charLat, char &charLon);
    //double distanceMoved(double, double);
    void distanceMoved(double&, double&, double&);
    double haversine(double, double, double, double);
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
    char _dateTime[17];
    //int nmeaUpdated;
    double _distance;
    long billion = 1000000000L;
    long latBillionths;
    long lonBillionths;
};

#endif
