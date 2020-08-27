/*
 * TSDZ2 EBike wireless firmware
 *
 * Copyright (C) Casainho, 2020
 *
 * Released under the GPL License, Version 3
 */

#ifndef PINS_H_
#define PINS_H_

#include <stdio.h>
#include <stdbool.h>

// NRF52840 MDK Dongle
// #if defined(BOARD_CUSTOM) && defined(NRF52840_MDK_USB_DONGLE)
typedef enum {
    PLUS__PIN  = 5,
    MINUS__PIN  = 6,
    ENTER__PIN = 7,
    STANDBY__PIN = 8,
    BUTTON_PIN_ELEMENTS = 4 // must be updated when added or removed an element
} button_pins_t;
// #endif

// // NRF52840 Dongle (The Blue One)
// #ifdef BOARD_PCA10059
// #define MOTOR_POWER_ENABLE__PIN             NRF_GPIO_PIN_MAP(1,0)
// #define MOTOR_TX__PIN                       NRF_GPIO_PIN_MAP(0,22)
// #define MOTOR_RX__PIN                       NRF_GPIO_PIN_MAP(0,24)
// #endif

void pins_init(void);
bool button_plus_is_set(void);
bool button_minus_is_set(void);
bool button_enter_is_set(void);
bool button_standby_is_set(void);

#endif /* PINS_H_ */
