#include "Arduino.h"
#include "Config.h"

Config::Config(int offset) {
  // Constructor
  // Takes an int as offset
  _offset = offset;
  _needSave = false;
}

boolean Config::load(void) {
  // load config

}

boolean Config::save(void) {
  // config 
  if ( _needSave = true ) {
    // do saving  
  }
}
