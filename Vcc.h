#ifndef Vcc_h
#define Vcc_h

/*
 * regulator
 * 
 * Library to wrap regulator
 * 
 * Greg Cope <greg.cope@gmail.com>
 * 
 */

#include <Arduino.h>

#define DEBUG(input)   {Serial.print(input); Serial.flush();}
#define DEBUGln(input) {Serial.println(input); Serial.flush();}

// 3.3 (Vcc) / 1024 (ADC precision) =  0.0032258
// Motineo example 1m+470k = 470 / ( 1000 +470 ) = 0.32 rounded up
// Ours 1.8m + 3.3m = 3300000 / ( 1800000 + 3300000 ) = 0.64705 or 1.55 (inverse)
// Ours 27M + 3.3M = 27000000 / ( 3300000 + 27000000 ) = 0.8910891089 or 1.1222222222
//#define VCC_FORMULA(reading) reading * 0.0032258 * 1.1222222222 // >>> fine tune this parameter to match your voltage when fully charged
                                                       // details on how this works: https://lowpowerlab.com/forum/index.php/topic,1206.0.html
// measured                                                
#define VCC_FORMULA(reading) reading * 0.003620064444

class Vcc
{
  public:
    Vcc(int pin0, int pin1);
    float read(void);
    boolean on(void);
    void off(void);
    boolean isOn(void);
  private:
    unsigned long _onTimeMs;
    int _powerPin;
    int _voltageDividerPin;
    boolean _powerState;
    int vccReadings;
    float vccVolts;
};

#endif
