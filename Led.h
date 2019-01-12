#ifndef Led_h
#define Led_h

/*
 * Led
 * 
 * Library to wrap an Led
 * 
 * Greg Cope <greg.cope@gmail.com>
 * 
 */

#include <Arduino.h>

#define DEBUG(input)   {Serial.print(input); Serial.flush();}
#define DEBUGln(input) {Serial.println(input); Serial.flush();}

// Moteino MEGAs have LEDs on D15
// ABoatMon Board has another LED switch on pin D26

class Led
{
  public:
    Led(int pin);
    void on(void);
    void off(void);
    void blink(int DELAY_MS, byte loops);
  private:
    int _ledPin; 
};

#endif
