/*
 * TSDZ2 EBike wireless firmware
 *
 * Copyright (C) Casainho, 2020
 *
 * Released under the GPL License, Version 3
 */

#include <stdint.h>
#include "antplus_controls_page_82.h"

typedef struct
{
    uint8_t temperature_state;
    uint8_t travel_mode_state;
    uint8_t system_state;
    uint8_t gear_state;
    uint8_t error_message;
    uint8_t CONTROLS_speed_lsb;
    uint8_t CONTROLS_speed_msn;
} antplus_controls_page_82_data_layout_t;

void antplus_controls_page_82_encode(uint8_t * p_page_buffer,
                           antplus_controls_page_82_data_t const * p_page_data)
{
    antplus_controls_page_82_data_layout_t * p_outcoming_data = (antplus_controls_page_82_data_layout_t *)p_page_buffer;

    p_outcoming_data->temperature_state = p_page_data->temperature_state;
    p_outcoming_data->error_message = p_page_data->error_message;
}
