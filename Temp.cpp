#include <Arduino.h>
#include <OneWire.h>
// https://lowpowerlab.com/forum/moteino/improvedoptimized-ds18b201wire-read/
#include "Temp.h"
// http://www.pjrc.com/teensy/td_libs_OneWire.html


Temp::Temp(int power, int pin) : myds(pin) {
  // Constructor
  // Takes an int as pin to power up, and data pin
  _powerPin = power;
  _dataPin = pin;
  myds = OneWire(_dataPin);
}

void Temp::init(void) {
  // find ds18B20
  on();
  //myds(_dataPin);
  getFirstDsAdd(myds, _dsAddr);
  off();
}

void Temp::startConvert(void) {
  // issue a read command to let the uC do other stuff
  DEBUG("temp startRead: ");
  on();
  dsSetResolution(myds);
  dsConvertCommand(myds);
  // leave it on ...
}

float Temp::read(void) {
  // this blocks till a read or timeout reached
  // returns signed float in C
  //DEBUG("temp read: ");
  //_tempInC = 20.7;
  //DEBUGln(_tempInC);

  byte present = 0;
  int i;
  byte data[12];
  unsigned int raw;
  
  //type_s = 0;
  
  present = myds.reset();
  myds.select(_dsAddr);    
  myds.write(0xBE);         // Read Scratchpad

  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = myds.read();
  }

  // convert the data to actual temperature

  raw = (data[1] << 8) | data[0];
  raw = raw << 3; // 9 bit resolution default
  
  _tempInC = (float)raw / 16.0;
  Serial.print("Temp (C): ");
  //Serial.println(celsius);
  DEBUG("Temp (C): ");
  DEBUGln(_tempInC);
//  return celsius;
  off();
  return _tempInC;

}








void Temp::on(void) {
  // Switch device on by putting pin HIGH
  digitalWrite(_powerPin, HIGH);
  Serial.println("Temp device on!");
  Serial.flush();
}

void Temp::off(void) {
  // turn off the Device by putting pin LOW
  digitalWrite(_powerPin, LOW);
  //delay(500);
  Serial.println("Temp device off!");
}

void Temp::dsConvertCommand(OneWire myds) {
  myds.reset();
  myds.select(_dsAddr);
  myds.write(0x44);         // start conversion, with parasite power on at the end
}

void Temp::dsSetResolution(OneWire myds) {
  
  // Set resolution
  myds.reset();
  myds.select(_dsAddr);
  myds.write(0x4E);         // Write scratchpad
  myds.write(0);            // TL
  myds.write(0);            // TH
  // 0x1F == 9 bytes resolution ... I think
  myds.write(0x1F);         // Configuration Register
  myds.write(0x48);         // Copy Scratchpad
}

void Temp::getFirstDsAdd(OneWire myds, byte firstadd[]){
  byte i;
  //byte present = 0;
  byte addr[8];
  //float celsius;
  
  //int length = 8;
  
  DEBUGln("Looking for 1-Wire devices...");
  while(myds.search(addr)) {
    DEBUGln("Found \'1-Wire\' device with address: ");
    for( i = 0; i < 8; i++) {
      firstadd[i]=addr[i];
      DEBUG("0x");
      if (addr[i] < 16) {
        DEBUG('0');
      }
      DEBUG(addr[i]);
      DEBUG(HEX);
      if (i < 7) {
        DEBUG(", ");
      }
    }
    if ( OneWire::crc8( addr, 7) != addr[7]) {
        DEBUGln("CRC is not valid!");
        return;
    }
     // the first ROM byte indicates which chip

    DEBUGln("address:");
    DEBUG(addr[0]);
    
    return;
  } 
}
