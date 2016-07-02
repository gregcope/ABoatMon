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

#include <Arduino.h>
#include <EEPROM.h>
#include "CRC8.h"

class Config
{
  public:
    Config(int offset);
    boolean load(void);
    boolean save(void);
  private:
    boolean _needSave; // flag to save
    int _offset; // Eeprom offset

    // config goes in here ....
    struct _config {
      // config is versioned as the struct is fixed
      unsigned int _version;
      // 20 chars, plus a few for luck, the leading +, and null char ending
      char gsmPhone[24];
      // FIX probably not the best name
      double savedLat;
      double savedlon;
      // how many writes
      unsigned int serial;
      // checksum?
      unsigned int crc;
    } _config = {
      0,
      // this need double quotes?  gcc parsing bug?
      "+441234567890",
      100,
      100,
      1,
      0 
    };
};

#endif
