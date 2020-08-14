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
#define PLUS__PIN             5
#define MINUS__PIN            6
#define ENTER__PIN            7
#define STANDBY__PIN          8
// #endif

// // NRF52840 Dongle (The Blue One)
// #ifdef BOARD_PCA10059
// #define MOTOR_POWER_ENABLE__PIN             NRF_GPIO_PIN_MAP(1,0)
// #define MOTOR_TX__PIN                       NRF_GPIO_PIN_MAP(0,22)
// #define MOTOR_RX__PIN                       NRF_GPIO_PIN_MAP(0,24)
// #endif

typedef enum
{
  BUTTON_NONE = 0,
  BUTTON_PLUS,
  BUTTON_MINUS,
  BUTTON_ENTER,
  BUTTON_STANDBY,
} button_type_t;

extern button_type_t g_buttons;

void pins_init(void);
bool button_plus_state(void);
bool button_minus_state(void);
bool button_enter_state(void);
bool button_standby_state(void);

#endif /* PINS_H_ */
