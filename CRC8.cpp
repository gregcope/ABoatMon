#include "Arduino.h"
#include "CRC8.h"

CRC8::CRC8(void) {
  // constructor
}

byte CRC8::checksum(String thingToCRC) {
  
    // reset the following
    _crc8 = 0;
    _strLength = thingToCRC.length();

    thingToCRC.toCharArray(_crcBuffer, _strLength+1);
    
    for (byte x = 0; x<_strLength+1; x++){ // XOR every character in between '$' and '*'
      _crc8 = _crc8 ^ _crcBuffer[x] ;
    }
    
    return _crc8;
}
