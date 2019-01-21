#ifndef Temp_h
#define Temp_h

/*
 * Led
 * 
 * Library to wrap an 18B20 Temp sensor
 * 
 * Greg Cope <greg.cope@gmail.com>
 * 
 */

#include <Arduino.h>
#include <OneWire.h>
// https://lowpowerlab.com/forum/moteino/improvedoptimized-ds18b201wire-read/

#define DEBUG(input)   {Serial.print(input); Serial.flush();}
#define DEBUGln(input) {Serial.println(input); Serial.flush();}

class Temp
{
  public:
    Temp(int power, int pin);
    //Temp(int power, int pin) : myds(pin);
    void init(void);
    void startConvert(void);
    float read(void);
    void on(void);
    void off(void);
  private:
    void getFirstDsAdd(OneWire myds, byte firstadd[]);
    void dsSetResolution(OneWire myds, byte addr[8]);
    void dsConvertCommand(OneWire myds, byte addr[8]);
    int _powerPin;
    int _dataPin; 
    byte _dsAddr[8];
    OneWire myds;
    unsigned long starttime;
    unsigned long elapsedtime;
    unsigned int _SignBit;
    unsigned int _TReading;
    float _celsius;
};

#endif
