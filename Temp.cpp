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
    //DEBUG(".");
  }

  byte present = 0;
  int i;
  byte data[12];
  byte type_s;

  type_s = 0;
  
  present = myds.reset();
  myds.select(_dsAddr);    
  myds.write(0xBE);         // Read Scratchpad

  //Serial.print("  Data = ");
  //Serial.print(present,HEX);
//  Serial.println("Raw Scratchpad Data: ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = myds.read();
//    Serial.print(data[i], HEX);
//    Serial.print(" ");
  }
  //Serial.print(" CRC=");
  //Serial.print(OneWire::crc8(data, 8), HEX);
//  Serial.println();

  // convert the data to actual temperature

  //unsigned int raw = (data[1] << 8) | data[0];
//  if (type_s) {
//   DEBUGln("type S!");
//    raw = raw << 3; // 9 bit resolution default
//    if (data[7] == 0x10) {
//      DEBUGln("data[7] == 0x10");
      // count remain gives full 12 bit resolution
 //     raw = (raw & 0xFFF0) + 12 - data[6];
//    } else {
//      byte cfg = (data[4] & 0x60);
//      DEBUGln("byte cfg = (data[4] & 0x60);");
//      if (cfg == 0x00) { 
//        DEBUGln("cfg == 0x00");
//        raw = raw << 3; 
 //       }  // 9 bit resolution, 93.75 ms
 //       else if (cfg == 0x20) { 
//          DEBUGln("cfg == 0x20");
//          raw = raw << 2; // 10 bit res, 187.5 ms
//        }
//        else if (cfg == 0x40) {
//          DEBUGln("cfg == 0x40");
//          raw = raw << 1; // 11 bit res, 375 ms
//        }
        // default is 12 bit resolution, 750 ms conversion time
//    }
//  }
  //raw = raw << 3;

  unsigned int _TReading = (data[1] << 8) + data[0];
  unsigned int _SignBit = _TReading & 0x8000;  // test most sig bit
  if (_SignBit) // negative
  {
    _TReading = (_TReading ^ 0xffff) + 1; // 2's comp
  }
  _celsius = float(_TReading)/16;
  
  if (_SignBit){
    _celsius = _celsius * -1;
  }
 
  elapsedtime = millis() - starttime;
  DEBUG("temp time: ");
  DEBUGln(elapsedtime);
  //return 20.7;
  off();
  return _celsius;
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
