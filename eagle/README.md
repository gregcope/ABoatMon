# Eagle Files

0. Uses Sparkfun DRC file
1. ERC and DRC check clean!
2. Bottom Copper Pour

## Design

0. I may add more parts...
1. Uses a MoteinoMega, Adafruit Ultimate GPS and an Adafruit FONA GSM
2. The P-channel Mosfet is designed to power the GPS directly, avoiding the on board regulator
3. The VBAT is on the MotinoMega 3.3V rail to keep the GPS backup circuits powered
4. The JST is for a Lipo battery
5. Their is a 1.8M/3.3M VCC voltage divider to monitor the LIPO, with a 0.1uF capacitor - see [Jeelabs post here](https://jeelabs.org/2013/05/16/measuring-the-battery-without-draining-it/)
6. J1 - a 2-Screw Terminal Connector is for a simple Bilge switch for the Moteino to monitor
7. J2 - Button
8. J3 - LED
9. J4 and part of J5 DS18B20 Temp probe
10. Part of J5 is a 12V-24V VCC input
11. A 27M/3.3M voltage divider to measure 30+V on an ADC pin
12. Headers for Fona GSM module
13. A 5v pin to connect to the Fona USB charge port
14. There is a jumper to measure the uCurrent the GSM uses, and also by-pass the switching

... What's missing?

No thread is complete without pictures...

![Eagle Schematic for MoteinoMega Ultimate GPS Sheild](https://raw.githubusercontent.com/gregcope/ABoatMon/master/eagle/MoteinoMegaGPSSheild-sch.png "Eagle Schematic for MoteinoMega Ultimate GPS Sheild")

![Eagle Board for MoteinoMega Ultimate GPS Sheild](https://raw.githubusercontent.com/gregcope/ABoatMon/master/eagle/MoteinoMegaGPSSheild-brd.png "Eagle Board for MoteinoMega Ultimate GPS Sheild")
