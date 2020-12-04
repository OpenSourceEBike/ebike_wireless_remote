# Getting Started
**Components you will need:**
* [nRF52840 MDK USB Dongle](https://makerdiary.com/products/nrf52840-mdk-usb-dongle): (costs 11€) this is this wireless board
* [CRC2032 button cell](https://en.wikipedia.org/wiki/Button_cell): (costs 1€)
* [VLCD5 handle bar remote keypad](https://www.aliexpress.com/wholesale?catId=0&initiative_id=SB_20200828081711&origin=y&SearchText=LCD+controller+of+VLCD5+display+for+TSDZ2+electric): (costs 6€) can be any keypad that works in the similar way
*  [STLINK V2](https://www.ebay.com/c/896036216): (costs 2€)
## [Pinout](nrf52840-mdk-usb-dongle-pinout_v1_0.pdf)

## Schematic
![schematic](ebike_remote_wireless-v1.png)

**Explanation:**
Wire up the remote per the above schematic.
* The CRC2032 button cell powers up the wireless board. Connect the cell positive to the pin 10 / VIN and the negative to the pin 8 / GND.
* VLCD5 handle bar remote keypad has 4 buttons and 5 wires (1 wire for each button and 1 common wire to all buttons). Connect the wires directly to the wireless board.
* For flashing the firmware, that you will need to do only once to get started, solder the 4 wires to the STLinkV2 Once the firmware is flashed, all future updates can be done wirelessly.

## Flashing the firmware
Flash the firmware with any tool that works for you. 
###Linux
Use OpenOCD.
Steps: 
1. Connect your ST-Link to the board, opena terminal,  and use the following OpenOCD command to flash the bootloader: "$OPENOCD_DIR/bin/openocd" -f interface/stlink.cfg -c "transport select hla_swd" -f target/nrf52.cfg -c "init; halt; program ebike_wireless_remote_with_sd_v1.0.0.hex verify; reset; exit"
2. If you see ** Verified OK **, the flashing has been successful.

###Windows
Use the STM32 ST-LINK Utility. Here the main steps:

* Target Chip erase
* Load bin file (load the firmware bin file)
* Target program


The firmware HEX file to flash is "ebike_wireless_remote_with_sd_v1.0.0.hex" located in the github releases. 


  
##[back](../README.md)
