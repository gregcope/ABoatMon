#ifndef CRC8_h
#define CRC8_h

//
// CRC8
// Library to create CRC8 checksums
// Used by NMEA
// Most of this is from;
// http://elimelecsarduinoprojects.blogspot.co.uk/2013/07/nmea-checksum-calculator.html?m=1
//
// Greg Cope <greg.cope@gmail.com>
// 17-May-2015 Initial version
// 31-May-2015 Test case example added, fixes, tested

// the #include statment and code go here...
#include "Arduino.h"

class CRC8
{
  public:
    CRC8(void);
    byte checksum(String thingToCRC);
  private:
    // _crc return byte
    byte _crc8 = 0;
    // _strLength to hold how long the string is
    int _strLength = 0;
    // char buffer to hold string
    char _crcBuffer[255];
};

#endif
