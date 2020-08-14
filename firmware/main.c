/*
 * TSDZ2 EBike wireless firmware
 *
 * Copyright (C) Casainho, 2020
 *
 * Released under the GPL License, Version 3
 */

#include <stdio.h>
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf.h"
#include "hardfault.h"
#include "app_error.h"
#include "app_timer.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ant.h"
#include "ant_key_manager.h"
#include "antplus_controls.h"
#include "pins.h"

#define CONTROLS_HW_REVISION 1
#define CONTROLS_MANUFACTURER_ID (UINT16_MAX - 1)
#define CONTROLS_MODEL_NUMBER 1
#define CONTROLS_SW_REVISION_MAJOR 1
#define CONTROLS_SW_REVISION_MINOR 1
#define CONTROLS_SERIAL_NUMBER 1234567
#define CONTROLS_CHANNEL_NUM 0 // ?? seems can be any value from 0 to 8
#define CHAN_ID_TRANS_TYPE 0 // for pairing
#define CHAN_ID_DEV_NUM 0
#define ANTPLUS_NETWORK_NUM 0
#define ANTPLUS_CONTROLS_ANT_OBSERVER_PRIO 1

void antplus_controls_evt_handler(antplus_controls_profile_t * p_profile, antplus_controls_evt_t event);

CONTROLS_SENS_CHANNEL_CONFIG_DEF(m_antplus_controls,
                            CONTROLS_CHANNEL_NUM,
                            CHAN_ID_TRANS_TYPE,
                            CHAN_ID_DEV_NUM,
                            ANTPLUS_NETWORK_NUM);
CONTROLS_SENS_PROFILE_CONFIG_DEF(m_antplus_controls,
                            antplus_controls_evt_handler);

static antplus_controls_profile_t m_antplus_controls;

NRF_SDH_ANT_OBSERVER(m_ant_observer, ANTPLUS_CONTROLS_ANT_OBSERVER_PRIO, antplus_controls_sens_evt_handler, &m_antplus_controls);

uint16_t cnt_1;

void antplus_controls_evt_handler(antplus_controls_profile_t * p_profile, antplus_controls_evt_t event)
{
  nrf_pwr_mgmt_feed();

  static uint32_t buttons_not_set = 1;

  // reset buttons_not_set when all of them are released
  if (buttons_not_set == 0) {
    if ((button_plus_state() == false) &&
        (button_minus_state() == false) &&
        (button_enter_state() == false) &&
        (button_standby_state() == false))
      buttons_not_set = 1;
  }

  switch (event)
  {
    case ANTPLUS_CONTROLS_PAGE_73_UPDATED:
      if (buttons_not_set) {
        if (button_plus_state()) {
          g_buttons = BUTTON_PLUS;
        } else if (button_minus_state()) {
          g_buttons = BUTTON_MINUS;
        } 

        buttons_not_set = 0;
      }
      break;

    default:
      break;
  }
}

static void profile_setup(void)
{
    uint32_t err_code;

    err_code = antplus_controls_sens_init(&m_antplus_controls,
                                 CONTROLS_SENS_CHANNEL_CONFIG(m_antplus_controls),
                                 CONTROLS_SENS_PROFILE_CONFIG(m_antplus_controls));
    APP_ERROR_CHECK(err_code);

    // fill battery status data page.
    m_antplus_controls.page_82 = ANTPLUS_CONTROLS_PAGE82(295); // battery 2.95 volts, fully charged

    // fill manufacturer's common data page.
    m_antplus_controls.page_80 = ANT_COMMON_page80(CONTROLS_HW_REVISION,
                                          CONTROLS_MANUFACTURER_ID,
                                          CONTROLS_MODEL_NUMBER);
    // fill product's common data page.
    m_antplus_controls.page_81 = ANT_COMMON_page81(CONTROLS_SW_REVISION_MAJOR,
                                          CONTROLS_SW_REVISION_MINOR,
                                          CONTROLS_SERIAL_NUMBER);

    err_code = antplus_controls_sens_open(&m_antplus_controls);
    APP_ERROR_CHECK(err_code);
}

static void softdevice_setup(void)
{
    ret_code_t err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    ASSERT(nrf_sdh_is_enabled());

    err_code = nrf_sdh_ant_enable();
    APP_ERROR_CHECK(err_code);

    err_code = ant_plus_key_set(ANTPLUS_NETWORK_NUM);
    APP_ERROR_CHECK(err_code);
}

static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

int main(void)
{
    static uint16_t cnt_2;

    log_init();
    pins_init();
    softdevice_setup();
    profile_setup();

    for (;;)
    {
        cnt_1 += 3;
        cnt_2++;
        __asm__("nop");

        NRF_LOG_FLUSH();
        // nrf_pwr_mgmt_run();
    }
}

