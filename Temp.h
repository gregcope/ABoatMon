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

#define DEBUG(input)   {Serial.print(input); Serial.flush();}
#define DEBUGln(input) {Serial.println(input); Serial.flush();}

class Temp
{
  public:
    Temp(int power, int pin);
    void startRead(void);
    float read(void);
  private:
    int _powerPin;
    int _dataPin; 
    float _tempInC;
};

#endif
