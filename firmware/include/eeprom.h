/*
 * Bafang LCD 850C firmware
 *
 * Copyright (C) Casainho, 2018.
 *
 * Released under the GPL License, Version 3
 */

#ifndef _EEPROM_H_
#define _EEPROM_H_

#pragma pack(4)
typedef struct configurations
{
    uint32_t ui8_bluetooth_flag;
    uint32_t ui8_ant_device_id;
} configurations_t;

void eeprom_init(uint8_t *ant_num, uint8_t *bluetooth);
void eeprom_write_variables(uint8_t ant_num, uint8_t bluetooth);

#endif /* _EEPROM_H_ */
