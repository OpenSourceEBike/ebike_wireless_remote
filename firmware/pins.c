/*
 * TSDZ2 EBike wireless firmware
 *
 * Copyright (C) Casainho, 2020
 *
 * Released under the GPL License, Version 3
 */

#include <stdio.h>
#include <stdbool.h>
#include "nrf_gpio.h"
#include "pins.h"

void pins_init(void)
{
  nrf_gpio_cfg_input(PLUS__PIN, GPIO_PIN_CNF_PULL_Pullup);
  nrf_gpio_cfg_input(MINUS__PIN, GPIO_PIN_CNF_PULL_Pullup);
  nrf_gpio_cfg_input(ENTER__PIN, GPIO_PIN_CNF_PULL_Pullup);
  nrf_gpio_cfg_input(STANDBY__PIN, GPIO_PIN_CNF_PULL_Pullup);
}

button_type_t g_buttons = BUTTON_NONE;

bool button_plus_state(void)
{
  return (bool) !nrf_gpio_pin_read(PLUS__PIN);
}

bool button_minus_state(void)
{
  return (bool) !nrf_gpio_pin_read(MINUS__PIN);
}

bool button_enter_state(void)
{
  return (bool) !nrf_gpio_pin_read(ENTER__PIN);
}

bool button_standby_state(void)
{
  return (bool) !nrf_gpio_pin_read(STANDBY__PIN);
}