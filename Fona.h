#ifndef Fona_h
#define Fona_h

/*
 * Fona
 * 
 * Library to wrap Fona phone board
 * 
 * Greg Cope <greg.cope@gmail.com>
 * 
 */

#include <Arduino.h>

#define DEBUG(input)   {Serial.print(input); Serial.flush();}
#define DEBUGln(input) {Serial.println(input); Serial.flush();}

class Fona
{
  public:
    Fona(int enablepin);
    boolean on(void);
    void off(void);
    boolean isOn(void);
  private:
    int _enablePin;
    boolean _powerState;
};

#endif
