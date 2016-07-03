#include <Arduino.h>
#include "Config.h"

Config::Config() {
  // Constructor
  // Takes an int as offset
  _needSave = false;
}

void Config::setGSMPhone(const char* gsmPhone) {
  strcpy( _config.gsmPhone, gsmPhone);
  Serial.print("_config.gsmPhone is: '");
  Serial.print(_config.gsmPhone);
  Serial.println("'");
  _needSave = true;
}

char* Config::getGSMPhone() {
  return _config.gsmPhone;  
}

void Config::setSavedLat(const double* Lat) {
  _config.savedLat = *Lat;
  _needSave = true;
}
double* Config::getSavedLat(void) {
  return &_config.savedLat;
}

void Config::setSavedLng(const double* Lng) {
  _needSave = true;
  _config.savedLng = *Lng;
}

double* Config::getSavedLng(void) {
  return &_config.savedLng;
}

boolean Config::load(void) {
  // load config
  _NOW = millis();
//  _crcChecksum = 0;
//  _tempBuffer = "PMTK313,1";
//  Serial.print("chars from eeProm: '");

  if (EEPROM.read(CONFIG_START + 0) == CONFIG_VERSION[0] &&
      EEPROM.read(CONFIG_START + 1) == CONFIG_VERSION[1] &&
      EEPROM.read(CONFIG_START + 2) == CONFIG_VERSION[2])
    for (unsigned int t = 0; t < sizeof(_config); t++) {
 
      *((char*)&_config + t) = EEPROM.read(CONFIG_START + t);
      Serial.print("'");
      // http://stackoverflow.com/questions/17158890/transform-char-array-into-string
//      _tempBuffer += EEPROM.read(CONFIG_START + t);
      Serial.print(EEPROM.read(CONFIG_START + t));
  } else {
    Serial.print("LOAD ERROR");  
  }
  Serial.println("'");

//  _crcChecksum = crc.checksum(_tempBuffer);
//  Serial.println(_tempBuffer);
//  Serial.print("checksum is: ");
//  Serial.println(_crcChecksum, HEX);
    
//  if ( _crcChecksum == _config.crc ) {
//    // what was loaded is what what saved...
//    Serial.print("serial is: ");
//    Serial.println(_config.crc, HEX);
//    Serial.print("checksum is: ");
//    Serial.println(_crcChecksum, HEX);
//    return true;
//  } else {
//    // Ops summat went wrong with load or save!!!!!
//    Serial.println("Something went wrong with the checksum");
//    return false;    
//  }
  _timeTaken = millis() - _NOW;
  Serial.print("Time taken to load: ");
  Serial.println(_timeTaken);
}

boolean Config::save(void) {
  // config 
  if ( _needSave == false ) {
    return true;
  }
  // ok, we need to save something
  _NOW = millis();
  _config.serial ++;
  Serial.print("_config.serial is: ");
  Serial.println(_config.serial);
  Serial.print("Size of _config: ");
  Serial.println(sizeof(_config));
  Serial.print("About to write: '");
  //unsigned int t = 0;
  byte c;
  for (unsigned int t = 0; t < sizeof(_config); t++) {
    //EEPROM.write(CONFIG_START + t, *((char*)&_config + t));
  //for (unsigned int t = 0; t < sizeof(_config); t++)
    // EEPROM.write does not return anything
    // so we need re-read in to check if it wrote ... grh
    Serial.print("'");
    c = *((char*)&_config + t);
//    EEPROM.write(CONFIG_START + t, *((char*)&_config + t));
    EEPROM.write(CONFIG_START + t, c);
    _tempBuffer += c;
//    Serial.print(c);
  }
  Serial.println("'");
  Serial.print("_tempBuffer is: ");
  Serial.println(_tempBuffer);

  //TODO read config and check sum to turn true

  // timer
  _timeTaken = millis() - _NOW;
  Serial.print("Time taken go save: ");
  Serial.println(_timeTaken);

  return true;
}
