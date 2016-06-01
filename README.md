ABoatMon
========

An Arduno (hence the A) Boat Monitoring solution

Intro
=====

A reimplementation, and extension of https://github.com/gregcope/piboatmon, as the original consumed too much power.  This is designed with the following goals;
* Be very low power and
* Chargable from 12V systems.

The present version is made from COTS parts to make life easy at the deign/proto stage.  However the code should be transferable.

Parts
=====
* Adafruit Ultimate GPS - any 3.3v UART GPS would do
* Adafruit fona - any GSM modem would do, although we do use the battery charging on the fona
* 3.7v LIPO battery

Classes
=======
* Device - A base, or standlone class to control power to devices via a Digital pin

HowTo
=====

1. Open a command terminal, and change cd to where your Ardunio IDE stores code
```cd Documents/Arduino```
2. Clone the repo
```git clone https://github.com/gregcope/ABoatMon.git```
3. Open Ardunio IDE and open ```ABoatMon.ino```

http://arduino.stackexchange.com/questions/348/how-to-organize-my-arduino-projects-structure-for-easy-source-control
