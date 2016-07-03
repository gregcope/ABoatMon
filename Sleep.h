#ifndef Sleep_h
#define Sleep_h

/*
 * Device
 * 
 * Library for a Device, that has on on/off state
 * Aimed to be used for LEDs, GPS etc.. 
 * Anything that has a PIN set to OUTPUT and set to HIGH for on
 * 
 * Greg Cope <greg.cope@gmail.com>
 * 04-May-2016 Initial version
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
  private:
    unsigned long runTime;
};

#endif
