ABoatMon
========

An Arduno clone based (hence the A) Boat Monitoring solution!

Intro
=====

A reimplementation, and extension of https://github.com/gregcope/piboatmon, as the original consumed too much power.  This is designed with the following goals;
* Be very, very low power and
* Chargable from 12V system
* Has a rechargable battery

The present version is made from COTS parts to make life easy at the deign/proto stage.  However the code should be transferable.

Parts
=====
* Adafruit Ultimate GPS - any 3.3v UART GPS would do
* Adafruit Fona - any GSM modem would do, although we do use the battery charging on the fona
* 3.7v LIPO battery
* Low power Atemega 1280.  This might fit into lower spec Atmegs no idea... 

Classes
=======
* Device - A base, or standlone class to control power to devices via a Digital pin
* Tinggps++ - A class to deal with GPS

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
