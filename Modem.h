#ifndef Modem_h
#define Modem_h

/*
 * Modem
 * 
 * Library to wrap Fona phone / other boards
 * 
 * Greg Cope <greg.cope@gmail.com>
 * 
 */

#include <Arduino.h>

// https://support.giffgaff.com/t5/Help-Support/Using-Giffgaff-2G-sim-in-Adafruit-FONA/td-p/18143791
#define APN_URL mobile.o2.co.uk
#define APN_USERNAME o2web
#define APN_PASSWORD password


#define DEBUG(input)   {Serial.print(input); Serial.flush();}
#define DEBUGln(input) {Serial.println(input); Serial.flush();}

class Modem
{
  public:
    Modem(int enablepin);
    boolean on(void);
    void off(void);
    boolean isOn(void);
  private:
    int _enablePin;
    boolean _powerState;
};

#endif
