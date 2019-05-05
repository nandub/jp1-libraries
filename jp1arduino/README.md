## Code

  [JP1 Arduino](https://forum.arduino.cc/index.php?topic=96737.msg725922#msg725922)

## Introduction

  Tim6502
  "The early JP1 remote controls (universal device IR remote controls for your audio-visual equipment - TVs, DVDs, Tivos, Cable boxes, etc.) were programmable through the parallel port on PCs. These days, parallel ports -as well as the original JP1 remotes- are becoming increasingly rare (both being replaced by the USB kind). Since at the moment I have a few more JP1 remotes (a handful) than parallel ports in operation (zero), I had to find a way to program updates for my audio-visual equipment lineup.
 
The Arduino can be accessed via USB and can also easily talk to the EEPROM that contains the program for the JP1 remote.  The software for JP1/JP1.X programming does communicate via USB to a JP1 EEPROM programmer firmware in order to program the original JP1 remotes.  It appears that the Arduino can easily be enlisted to cover the combo of USB serial driver and JP1 EEPROM programmer. Since Kevin Timmerman's open-sourced his JP1 EEPROM Programmer software (thanks, Kevin!), here we are!"

## Compile Arduino Sketch

  - Install Arduino IDE
  - `git clone --recursive https://github.com/nandub/jp1-libraries.git`
  - `cd jp1-libraries/jp1arduino`
  - `make`

