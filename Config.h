#ifndef Config_h
#define Config_h

/*
 * Config
 * 
 * Library for a Config object that loads/saves state to eeprom
 * 
 * Greg Cope <greg.cope@gmail.com>
 * 02-Jul-2016 Initial version
 * 
 */

#include "Arduino.h"

class Config
{
  public:
    Config(int offset);
    boolean load(void);
    boolean save(void);
  private:
    boolean _needSave;
    int _offset;
    //boolean isOn;
};

#endif
