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

#define CONFIG_START 0
#define CONFIG_VERSION "001"

class Config
{
  public:
    Config();
    boolean load(void);
    boolean save(void);
    void setGSMPhone(const char*);
    char* getGSMPhone(void);
    void setSavedLat(const double*);
    double* getSavedLat(void);
    void setSavedLng(const double*);
    double* getSavedLng(void);
  private:
    CRC8 crc;
    boolean _needSave; // flag to save
    byte _crcChecksum; // checksum holder
    String _tempBuffer; // checksum buffer holder
    unsigned long _NOW = 0; // time placeholder
    unsigned long _timeTaken = 0; //var to hold time taken
    
    // config goes in here ....
    struct ConfigStruct {
      // config is versioned as the struct is fixed
      char version[4];
      // 20 chars, plus a few for luck, the leading +, and null char ending
      char gsmPhone[24];
      // FIX probably not the best name
      double savedLat;
      double savedLng;
      // how many writes
      unsigned int serial;
      // checksum?
      unsigned int crc;
    } _config = {
      CONFIG_VERSION,
      // this need double quotes?  gcc parsing bug?
      "+441234567890",
      100,
      100,
      0,
      0 
    };
};

#endif
