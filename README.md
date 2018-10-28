ABoatMon
========

An Arduno clone based (hence the A) Boat Monitoring solution!

Intro
=====

A reimplementation, and extension of https://github.com/gregcope/piboatmon, as the original consumed too much power.  This is designed with the following goals;
* Be very, very low power and
* Chargable from 12-24V Vehicle
* Has a rechargable LIPO battery

The present version is made from COTS parts to make life easy at the deign/proto stage.  However the code should be transferable.

Parts
=====
* [Adafruit Ultimate GPS](https://www.adafruit.com/product/746) - any 3.3v UART GPS would do, however needs an enable pin and vbat GPS almanac backup
* [Adafruit Fona](https://www.adafruit.com/product/1946) - any 3.3v UART GSM modem would do, although we do use the battery charging on the fona
* [Low power Atemega 1280 Moteino Mega](https://lowpowerlab.com/shop/product/119) This might fit into lower spec Atmegs, but I have no idea... The [Moteino M0 would work well](https://lowpowerlab.com/shop/product/184), although pin outs need to change.
* [Mini Step Down Regulator Voltag Power Supply Module 4.5v~55v to 5v](https://www.ebay.co.uk/sch/i.html?_osacat=0&_odkw=Mini+Step+Down+Regulator+Voltag+Power+Supply+Module+4.5v~55v+to+5v&_from=R40&_trksid=p2334524.m570.l1313.TR0.TRC0.H0.XMini+Step+Down+Regulator+Voltag+Power+Supply+Module+4.5v~55v+to+5v+600ma.TRS0&_nkw=Mini+Step+Down+Regulator+Voltag+Power+Supply+Module+4.5v~55v+to+5v+600ma&_sacat=0)
* [DS18B20 Waterproof Digital Probe Temperature Sensor with Silicone Cable (higher temp) Thermometer](https://www.ebay.co.uk/sch/i.html?_from=R40&_trksid=p2380057.m570.l1313.TR0.TRC0.H0.Xvermont+l+tent.TRS0&_nkw=DS18B20+Waterproof+Digital+Probe+Temperature+Sensor+Silicone+Cable+Thermometer&_sacat=0)
* [12V 12mm LED Power Push Button Switch Momentary Waterproof Metal 4 Pin](https://www.ebay.co.uk/sch/i.html?_osacat=0&_odkw=waterproof+LED+switch+12mm&_from=R40&_trksid=p2334524.m570.l1313.TR0.TRC0.H0.Xwaterproof+LED+switch+12mm+momentary.TRS0&_nkw=waterproof+LED+switch+12mm+momentary&_sacat=0)
* 1200mah LIPO Battery (any single cell LIPO with a JST would do)
* Bilge Switch (any normally closed, simple switch would do)

External Libs
=======

* Tinygps++ - A class to deal with GPS
* Lowpower - A class to help with Sleep modes
* Eeprom - Save Eeprom state
* OneWire - temp sensor lib

Internal Classes
========
* Device - A base, or standlone class to control power to devices via a Digital pin
* Config - A class to provide a config object to store Eeprom state
* CRC8 - CRC checksum (for config saving)
* Sleep - Simple wrapper to LowPower sleep

High Level Logic
================

* Switches is on setup configures all the required parts
* Loop every 8 secs and checks;
* Bilge switch
* Every 4 mins checks the batteries
* and if above 13.7v (ie charging) enables the onboard lipo charger
* Reports bilge switch or low BatV
* Every Hour;
* Checkst the above
* Checks location
* If any of the above are out of band;
* Send a message

HowTo
=====

1. Open a command terminal, and change cd to where your Ardunio IDE stores code
```cd Documents/Arduino```
2. Clone the repo
```git clone https://github.com/gregcope/ABoatMon.git```
3. Open Ardunio IDE and open ```ABoatMon.ino```

http://arduino.stackexchange.com/questions/348/how-to-organize-my-arduino-projects-structure-for-easy-source-control
