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
    uint32_t time(void);
  private:
    //unsigned long runTime;
    uint32_t _time = 0;
    uint32_t _now = 0;
};

#endif
