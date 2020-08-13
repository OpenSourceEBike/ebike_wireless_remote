/*
 * TSDZ2 EBike wireless firmware
 *
 * Copyright (C) Casainho, 2020
 *
 * Released under the GPL License, Version 3
 */

#ifndef ANTPLUS_CONTROLS_H__
#define ANTPLUS_CONTROLS_H__

#include <stdint.h>
#include <stdbool.h>
#include "ant_parameters.h"
#include "nrf_sdh_ant.h"
#include "ant_channel_config.h"
#include "ant_request_controller.h"
#include "antplus_controls_pages.h"
#include "sdk_errors.h"

#define CONTROLS_DEVICE_TYPE       0x14u // ANT+ LEV datasheet: Device type reserved for ANT+ LEV.
#define CONTROLS_ANTPLUS_RF_FREQ   0x39u // ANT+ LEV datasheet: Frequency, decimal 57 / 0x39 (2457 MHz).

#define CONTROLS_MSG_PERIOD_4Hz    0x2000u // Message period, decimal 8192 (4.00 Hz).

#define CONTROLS_EXT_ASSIGN        0x00 //< ANT ext assign (see Ext. Assign Channel Parameters in ant_parameters.h: @ref ant_parameters).
#define CONTROLS_SENS_CHANNEL_TYPE CHANNEL_TYPE_MASTER // TX LEV channel type.

#define CONTROLS_SENS_CHANNEL_CONFIG_DEF(NAME,                                       \
                                    CHANNEL_NUMBER,                             \
                                    TRANSMISSION_TYPE,                          \
                                    DEVICE_NUMBER,                              \
                                    NETWORK_NUMBER)                             \
static const ant_channel_config_t   CONCAT_2(NAME,_channel_CONTROLS_SENS_config) =   \
    {                                                                           \
        .channel_number     = (CHANNEL_NUMBER),                                 \
        .channel_type       = CONTROLS_SENS_CHANNEL_TYPE,                          \
        .ext_assign         = CONTROLS_EXT_ASSIGN,                                   \
        .rf_freq            = CONTROLS_ANTPLUS_RF_FREQ,                              \
        .transmission_type  = (TRANSMISSION_TYPE),                              \
        .device_type        = CONTROLS_DEVICE_TYPE,                                  \
        .device_number      = (DEVICE_NUMBER),                                  \
        .channel_period     = CONTROLS_MSG_PERIOD_4Hz,                               \
        .network_number     = (NETWORK_NUMBER),                                 \
    }
#define CONTROLS_SENS_CHANNEL_CONFIG(NAME) &CONCAT_2(NAME,_channel_CONTROLS_SENS_config)

#define CONTROLS_SENS_PROFILE_CONFIG_DEF(NAME,                                       \
                                    EVT_HANDLER)                                \
static antplus_controls_sens_cb_t            CONCAT_2(NAME,_CONTROLS_SENS_cb);                \
static const antplus_controls_sens_config_t  CONCAT_2(NAME,_profile_CONTROLS_SENS_config) =   \
    {                                                                           \
        .p_cb                       = &CONCAT_2(NAME,_CONTROLS_SENS_cb),             \
        .evt_handler                = (EVT_HANDLER),                            \
    }
#define CONTROLS_SENS_PROFILE_CONFIG(NAME) &CONCAT_2(NAME,_profile_CONTROLS_SENS_config)

typedef enum{
    ANTPLUS_CONTROLS_PAGE_17  = 17,
    ANTPLUS_CONTROLS_PAGE_82  = 82,
    ANTPLUS_CONTROLS_PAGE_70 = ANT_COMMON_PAGE_70,
    ANTPLUS_CONTROLS_PAGE_80 = ANT_COMMON_PAGE_80,
    ANTPLUS_CONTROLS_PAGE_81 = ANT_COMMON_PAGE_81,
} antplus_controls_page_t;

typedef enum{
    ANTPLUS_CONTROLS_PAGE_17_UPDATED  = ANTPLUS_CONTROLS_PAGE_17,
    ANTPLUS_CONTROLS_PAGE_82_UPDATED  = ANTPLUS_CONTROLS_PAGE_82,
    ANTPLUS_CONTROLS_PAGE_80_UPDATED = ANTPLUS_CONTROLS_PAGE_80,
    ANTPLUS_CONTROLS_PAGE_81_UPDATED = ANTPLUS_CONTROLS_PAGE_81,
    ANTPLUS_CONTROLS_PAGE_REQUEST_SUCCESS,
    ANTPLUS_CONTROLS_PAGE_REQUEST_FAILED,
} antplus_controls_evt_t;

// Forward declaration of the antplus_controls_profile_t type.
typedef struct antplus_controls_profile_s antplus_controls_profile_t;

/**@brief SDM event handler type. */
typedef void (* antplus_controls_evt_handler_t) (antplus_controls_profile_t *, antplus_controls_evt_t);

#include "antplus_controls_local.h"

#ifdef __cplusplus
extern "C" {
#endif

/**@brief LEV Sensor configuration structure. */
typedef struct
{
    antplus_controls_sens_cb_t     * p_cb;                       ///< Pointer to the data buffer for internal use.
    antplus_controls_evt_handler_t   evt_handler;                ///< Event handler to be called for handling events in the SDM profile.
} antplus_controls_sens_config_t;

struct antplus_controls_profile_s
{
    uint8_t                     channel_number; ///< Channel number assigned to the profile.
    union {
        antplus_controls_sens_cb_t * p_sens_cb;
    } _cb;                                      ///< Pointer to internal control block.
    antplus_controls_evt_handler_t  evt_handler;    ///< Event handler to be called for handling events in the LEV profile.
    antplus_controls_page_17_data_t page_17;         ///< Page 17.
    antplus_controls_page_82_data_t page_82;        ///< Page 82.
    antplus_common_page80_data_t    page_80;        ///< Page 80.
    antplus_common_page81_data_t    page_81;        ///< Page 81.
};

ret_code_t antplus_controls_sens_init(antplus_controls_profile_t           * p_profile,
                             ant_channel_config_t const  * p_channel_config,
                             antplus_controls_sens_config_t const * p_sens_config);

ret_code_t antplus_controls_sens_open(antplus_controls_profile_t * p_profile);

ret_code_t antplus_controls_page_request(antplus_controls_profile_t * p_profile, antplus_common_page70_data_t * p_page_70);

void antplus_controls_sens_evt_handler(ant_evt_t * p_ant_evt, void * p_context);

#ifdef __cplusplus
}
#endif

#endif

