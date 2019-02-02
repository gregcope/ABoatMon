ABoatMon
========

An Arduno clone based (hence the A) Boat Monitoring solution!

Intro
=====

A reimplementation, and extension of https://github.com/gregcope/piboatmon, as the original consumed too much power.  This is designed with the following goals;
* Be very, very low power and
* Monitor and charge from 12-24V

The present version is made from COTS parts to make life easy at the deign/proto stage.  However the code should be transferable.

Parts
=====
* [Adafruit Ultimate GPS](https://www.adafruit.com/product/746) - any 3.3v UART GPS would do, however needs an enable pin and vbat GPS almanac backup
* [Adafruit Fona](https://www.adafruit.com/product/1946) - any 3.3v UART GSM modem would do, although we do use the battery charging on the fona
* [Low power Atemega 1280 Moteino Mega](https://lowpowerlab.com/shop/product/119) This might fit into lower spec Atmegs, but I have no idea... The [Moteino M0 would work well](https://lowpowerlab.com/shop/product/184), although pin outs need to change.
* [Mini Step Down Regulator Voltag Power Supply Module 4.5v~55v to 5v](https://www.ebay.co.uk/sch/i.html?_osacat=0&_odkw=Mini+Step+Down+Regulator+Voltag+Power+Supply+Module+4.5v~55v+to+5v&_from=R40&_trksid=p2334524.m570.l1313.TR0.TRC0.H0.XMini+Step+Down+Regulator+Voltag+Power+Supply+Module+4.5v~55v+to+5v+600ma.TRS0&_nkw=Mini+Step+Down+Regulator+Voltag+Power+Supply+Module+4.5v~55v+to+5v+600ma&_sacat=0).  Specifically the one with an enable pin and low power in off mode (<10ua).  Needs to supply 500ma+ (600ma is fine) and deal with 11-30v on the input to work on 12V or 24V systems.  Needs to supply 5V for the lipo charger.  Efficiency is not a massive issue as the regulator is off most of the time, only on to charge occassionally.
* [DS18B20 Waterproof Digital Probe Temperature Sensor with Silicone Cable (higher temp) Thermometer](https://www.ebay.co.uk/sch/i.html?_from=R40&_trksid=p2380057.m570.l1313.TR0.TRC0.H0.Xvermont+l+tent.TRS0&_nkw=DS18B20+Waterproof+Digital+Probe+Temperature+Sensor+Silicone+Cable+Thermometer&_sacat=0)
* [12V 12mm LED Power Push Button Switch Momentary Waterproof Metal 4 Pin](https://www.ebay.co.uk/sch/i.html?_osacat=0&_odkw=waterproof+LED+switch+12mm&_from=R40&_trksid=p2334524.m570.l1313.TR0.TRC0.H0.Xwaterproof+LED+switch+12mm+momentary.TRS0&_nkw=waterproof+LED+switch+12mm+momentary&_sacat=0)
* 1200mah LIPO Battery (any single cell LIPO with a JST would do)
* Bilge Switch (any normally closed, simple switch would do)

External Libs
=======

* [Tinygps++](https://github.com/mikalhart/TinyGPSPlus) - A class to deal with GPS
* [Lowpower](https://github.com/LowPowerLab/LowPower) - A class to help with Sleep modes
* Eeprom - Save Eeprom state
* [OneWire](https://github.com/PaulStoffregen/OneWire) - DS18B20 Temp sensor lib

Internal Classes
========
* Config - A class to provide a config object to store Eeprom state
* CRC8 - CRC checksum (for config saving)
* Sleep - Simple wrapper to LowPower sleep
* Buton - A simple button monitor
* Fona - Drive a fona to send data
* Gps - Drive a GPS and feed a TinyGPS++ object with other helper methods
* Led - A simple LED driver
* Lipo - A class to ADC a Lipo value
* Temp - A class with simple, non-sleeping temp measurements
* Vcc - A class to ADC the VCC, as well as enable/disable a regulartor (that drives the 5v Lipo charger)

High Level Logic
================

* Switches is on setup configures all the required parts
* Loop every 8 secs and checks;
   * Start the temp conversion 
   * Lipo volts
   * VCC (and enable regulator if Vcc > 13 and switch off if low)
   * Bilge switch
   * read temp (blocks)
* Every Hour;
   * Checks the above
   * Checks location
* If out of band, send a message as an alarm
* Daily
   * Send a message

VCC logic - Sacrifical Charging
================
The idea is that if VCC is 13v plus then a charge source is on and we can hence enable the regulator.  If the VCC is above REG_ON_VOLTS (13v) enable the regulator to start the LIPO charger.  If below REG_OFF_VOLTS (12.8) stop regulator

Message Sending
================
To save possible expensive bandwidth the message is compressed.  Ideally it would be JSON.  However it is optimised for speed and bandwidth costs.  It needs to be parsed on arrival.

HowTo
=====

1. Open a command terminal, and change cd to where your Ardunio IDE stores code
```cd Documents/Arduino```
2. Clone the repo
```git clone https://github.com/gregcope/ABoatMon.git```
3. Open Ardunio IDE and open ```ABoatMon.ino```

http://arduino.stackexchange.com/questions/348/how-to-organize-my-arduino-projects-structure-for-easy-source-control
