#Remote Configuration Options
The device configuration of the remote can be set via a bluetooth characteristic.
Once set, the configuration options are permanently stored in flash memory.
Long press the PLUS button to turn on bluettoth to allow the configuration to be set.
The currently implemented configuration options are:
1. Setting the ANT+ Device number for the ANT LEV ebike. This ensures the remote will only pair to a specific ebike. valid inputs are hexadecimal 0x01 - 0x59 (decimal 01-89)
Please note that setting the device number to 0x00 is a 'wild card' setting that will cause the remote to pair with ANY ebike device number.
2. Turn on/off ANT+ LEV control. 
 (input hex 0x90 to turn on, hex 0x91 to turn off) 
This allows the remote to control only a garmin bike computer or any other device that implement the ANT+ Controls profile
4. Turn on/off ANT+ CONTROLS  
 (input hex 0x92 to turn on, hex 0x93 to turn off)  
 This allows the remote to turn on/off the ANT+ CONTROLS capability depending on the use case. ie: the user may not have a garmin bike computer that needs to be controlled.
5. Turn on/off brake control depending on the use case
 (input hex 0x94 to turn on, hex 0x95 to turn off)  
7. Boot the remote control into Device Firmware Update (DFU) mode by inputting 0x99 to allow the remote firrmware to be updated wirelessly using bluetooth.
(input hex 0x99 to boot into DFU)  
A video showing a DFU process can be seen here:
[![video](https://img.youtube.com/vi/va3LJoiosoc/hqdefault.jpg)](https://youtu.be/va3LJoiosoc) 
   
##Configuration Display
The currently selected configuration options can be displayed using the on board led by long pressing the ENTER button on the remote.
The LED will continue a visual cycle spanning 2 seconds when the button is held down as follows:

RED LED (0n for 0.5 seconds) - ANT+ LEV active
GREEN LED (0n for 0.5 seconds) - ANT+ CONTROLS active
BLUE LED (0n for 0.5 seconds) - brake control active
NO LED for 0.5 seconds
##[back](../README.md)
