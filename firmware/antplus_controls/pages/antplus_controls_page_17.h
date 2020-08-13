/*
 * TSDZ2 EBike wireless firmware
 *
 * Copyright (C) Casainho, 2020
 *
 * Released under the GPL License, Version 3
 */

#ifndef ANTPLUS_CONTROLS_PAGE_17_H__
#define ANTPLUS_CONTROLS_PAGE_17_H__

#include <stdint.h>

typedef struct
{
    uint8_t temperature_state;
    uint8_t error_message;
} antplus_controls_page_17_data_t;

#define DEFAULT_ANTPLUS_CONTROLS_PAGE17(antplus_controls_page_17_data_t)      \
    {                           \
        .temperature_state = 0,    \
        .error_message     = 0,    \
    }

void antplus_controls_page_17_encode(uint8_t * p_page_buffer,
              antplus_controls_page_17_data_t const * p_page_data);

#endif