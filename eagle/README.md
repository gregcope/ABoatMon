# Eagle Files

0. Uses Sparkfun DRC file
1. ERC and DRC check clean!
2. Bottom Copper Pour

## Design

0. I intend to add more parts, hence the board is left large.
1. Uses a MoteinoMega and Adafruit Ultimate GPS
2. The P-channel Mosfet is designed to power the GPS directly, avoiding the on board regulator
3. The VBAT is on the MotinoMega 3.3V rail to keep the GPS backup circuits powered
4. The JST is for a Lipo battery
5. The J1 - a 2-Screw Terminal Connector is for a simple switch for the Moteino to monitor
6. Their is a 1.8M/3.3M VCC voltage divider to monitor the LIPO, with a 0.1uF capacitor - see [Jeelabs post here](https://jeelabs.org/2013/05/16/measuring-the-battery-without-draining-it/)

No thread is complete without pictures...

![Eagle Schematic for MoteinoMega Ultimate GPS Sheild](https://raw.githubusercontent.com/gregcope/ABoatMon/master/eagle/MoteinoMegaGPSSheild-sch.png "Eagle Schematic for MoteinoMega Ultimate GPS Sheild")

![Eagle Board for MoteinoMega Ultimate GPS Sheild](https://raw.githubusercontent.com/gregcope/ABoatMon/master/eagle/MoteinoMegaGPSSheild-brd.png "Eagle Board for MoteinoMega Ultimate GPS Sheild")
