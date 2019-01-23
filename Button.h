#ifndef Button_h
#define Button_h

/*
 * Device
 * 
 * Library for a Button
 * 
 * Switch should be connected to a digital pin one side
 * GND the other
 * 
 * Greg Cope <greg.cope@gmail.com>
 * 04-May-2016 Initial version
 * 
 */

// based on http://www.gammon.com.au/forum/?id=11955
#include <Arduino.h>

class Button
{
  public:
    Button(byte pin);
    boolean isClosed(void);
    void off(void);
  private:
    int _buttonPin;
};

#endif
