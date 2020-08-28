# EBike remote wireless

This projects implements a wireless remote button for EBikes and GPS units like the Garmin Edge. You can buy on the market a ready product like this one - see the [Garmin EBike Remote for about 80€](https://buy.garmin.com/en-US/US/p/545795/pn/010-12094-30).

See in action on this video:

[![video](https://img.youtube.com/vi/O67TSVkCuRM/hqdefault.jpg)](https://www.youtube.com/watch?v=O67TSVkCuRM)


## Current state
2020.08.28:
* The firmware works well as Edge Remote Buttons (tested on a Garmin Edge 830). Press UP or DOWN buttons to switch to next / previous page.
* The system enters in ultra low power mode if it fails to detect within ~30 seconds the master device / Edge or if the user do not press any button for 1 hour. The system wakes up from ultra low power wheen user press any button.
* What is missing:
  * no battery voltage measurement on the hardware, firmware and so it is not reported to master / Edge
  * design a part for 3D printing, that can reuse the VLCD5 remote keypad circuit board and the touch pad

**Components you will need:**
* [nRF52840 MDK USB Dongle](https://makerdiary.com/products/nrf52840-mdk-usb-dongle): (costs 11€) this is this wireless board
* [CRC2032 button cell](https://en.wikipedia.org/wiki/Button_cell): (costs 1€)
* [VLCD5 handle bar remote keypad](https://www.aliexpress.com/wholesale?catId=0&initiative_id=SB_20200828081711&origin=y&SearchText=LCD+controller+of+VLCD5+display+for+TSDZ2+electric): (costs 6€) can be any keypad that works in the similar way

## Schematic
![schematic](hardware/ebike_remote_wireless-v1.png)

**Explanation:**
* The CRC2032 button cell powers up the wireless board. Connect the cell positive to the pin 10 / VIN and the negative to the pin 8 / GND.
* VLCD5 handle bar remote keypad has 4 buttons and 5 wires (1 wire for each button and 1 common wire to all buttons). Connect the wires directly to the wireless board.
* For flashing the firmware, that you will need to do only one time, solder the 4 wires to the STLinkV2 (I use a cheap 1€ STLinkV2 clone that I bought on EBay).

## Firmware
Flash the firmware with any tool that works for you. The firmware HEX file to flash is this one: [ebike_wireless_remote_with_sd-v1.hex](firmware/release/ebike_wireless_remote_with_sd-v1.hex)

