#include <Arduino.h>
#include <OneWire.h>
// https://lowpowerlab.com/forum/moteino/improvedoptimized-ds18b201wire-read/
#include "Temp.h"
// http://www.pjrc.com/teensy/td_libs_OneWire.html


Temp::Temp(int power, int pin) : myds(pin) {
  // Constructor
  // Takes an int as pin to power up, and data pin
  _powerPin = power;
  pinMode(_powerPin, OUTPUT);
  _dataPin = pin;
  pinMode(_dataPin, INPUT);
  OneWire myds(_dataPin);
}

void Temp::init(void) {
  // find ds18B20 address
  // then stop
  DEBUGln("Temp: init");
  on();
  getFirstDsAdd(myds, _dsAddr);
  //DEBUGln("_dsAddr is: ");
  //byte i;
  //for( i = 0; i < 8; i++) {
  //   Serial.print("0x");
  //   if (_dsAddr[i] < 16) {
  //      Serial.print('0');
  //   }
  //   Serial.print(_dsAddr[i], HEX);
  //   if (i < 7) {
  //      Serial.print(", ");
  //    } 
    
  //}
  off();
}

float Temp::read() {

  // block whilst we await a ready DS18B20
  
  DEBUGln("Temp: read");
  while (!myds.read()) {
    // block
    DEBUG(".");
  }
  elapsedtime = millis() - starttime;
  DEBUG("temp time: ");
  DEBUGln(elapsedtime);
  return 20.7;
  off();
}

void Temp::startConvert(void) {
  // switch DS18B20 on
  // set resolution
  // call temp convert command
  DEBUGln("Temp: startConvert");
  on();
  dsSetResolution(myds, _dsAddr);
  starttime = millis();
  dsConvertCommand(myds, _dsAddr);
}

void Temp::dsConvertCommand(OneWire myds, byte addr[8]) {
  DEBUGln("Temp: dsConvertCommand");
  myds.reset();
  myds.select(addr);
  myds.write(0x44,1);         // start conversion, with parasite power on at the end 
}

void Temp::dsSetResolution(OneWire myds, byte addr[8]) {

  DEBUGln("Temp: dsSetResolution");
  // Set 9 bit config
  myds.reset();
  myds.select(addr);
  myds.write(0x4E);         // Write scratchpad
  myds.write(0);            // TL
  myds.write(0);            // TH
  myds.write(0x1F);         // Configuration Register
  myds.write(0x48);         // Copy Scratchpad
}


void Temp::getFirstDsAdd(OneWire myds, byte firstadd[]){
  byte i;
  byte addr[8];
  
  DEBUGln("Looking for 1-Wire devices...");
  while(myds.search(addr)) {
    DEBUGln("Found \'1-Wire\' device with address: ");
    for( i = 0; i < 8; i++) {
      firstadd[i]=addr[i];
      DEBUG("0x");
      if (addr[i] < 16) {
        DEBUG('0');
      }
      Serial.print(addr[i], HEX);
      if (i < 7) {
        DEBUG(", ");
      }
    }
    if ( OneWire::crc8( addr, 7) != addr[7]) {
        DEBUGln("CRC is not valid!");
        return;
    }
     // the first ROM byte indicates which chip
    DEBUGln("");

    DEBUG("address:");
    DEBUGln(addr[0]);
    
    return;
  } 
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
