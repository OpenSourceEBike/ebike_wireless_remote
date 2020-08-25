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

bool button_plus_is_set(void)
{
  return (bool) !nrf_gpio_pin_read(PLUS__PIN);
}

bool button_minus_is_set(void)
{
  return (bool) !nrf_gpio_pin_read(MINUS__PIN);
}

bool button_enter_is_set(void)
{
  return (bool) !nrf_gpio_pin_read(ENTER__PIN);
}

bool button_standby_is_set(void)
{
  return (bool) !nrf_gpio_pin_read(STANDBY__PIN);
}