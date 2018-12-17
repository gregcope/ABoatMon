#ifndef Sleep_h
#define Sleep_h

/*
 * Sleep
 * 
 * Library to wrap sleep commands
 * 
 * Greg Cope <greg.cope@gmail.com>
 * 
 */

#include <Arduino.h>
// https://github.com/LowPowerLab/LowPower
#include <LowPower.h>

class Sleep
{
  public:
    Sleep();
    void kip8Secs(void);
    void kip2Secs(void);
    void kip1Sec(void);
  private:
    unsigned long runTime;
};

#endif
