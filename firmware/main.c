/*
 * TSDZ2 EBike wireless firmware
 *
 * Copyright (C) Casainho, 2020
 *
 * Released under the GPL License, Version 3
 */

#include <stdio.h>
#include "nrf.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "hardfault.h"
#include "app_error.h"
#include "app_timer.h"
#include "app_button.h"
#include "nrf_drv_clock.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ant.h"
#include "ant_key_manager.h"
#include "antplus_controls.h"
#include "pins.h"

#define BUTTON_DETECTION_DELAY APP_TIMER_TICKS(50) /**< Delay from a GPIOTE event until a button is reported as pushed (in number of timer ticks). */
#define BUTTON_PRESS_TIMEOUT APP_TIMER_TICKS(60 * 60 * 1000) // 1h

APP_TIMER_DEF(m_antplus_controls_send);
APP_TIMER_DEF(m_timer_button_press_timeout);

bool m_buttons_sent_wait = false;
button_pins_t m_buttons_wait_to_send = 0;
bool m_timer_buttons_send_running = false;

void shutdown(void);

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

void antplus_controls_evt_handler(antplus_controls_profile_t * p_profile, antplus_controls_evt_t event)
{
  nrf_pwr_mgmt_feed();

  switch (event)
  {
    case ANTPLUS_CONTROLS_PAGE_73_UPDATED:
      break;

    default:
      break;
  }
}

static void start_timer_antplus_controls_send(void) {
  ret_code_t err_code;

  err_code = app_timer_start(m_antplus_controls_send,
              APP_TIMER_TICKS(250), NULL);
  APP_ERROR_CHECK(err_code);
}

static void timer_button_press_timeout_handler(void * p_context)
{
  UNUSED_PARAMETER(p_context);

  // enter ultra low power mode
  shutdown();
}

static void timer_antplus_controls_send_handler(void * p_context)
{
  UNUSED_PARAMETER(p_context);

  // if there is a button page wait to be sent, sent it
  if (m_buttons_wait_to_send) {
    buttons_send_pag73(&m_antplus_controls, m_buttons_wait_to_send);
    m_buttons_wait_to_send = 0;
  }

  // if we sent a button page, start again the timer
  if (m_buttons_wait_to_send) {
    start_timer_antplus_controls_send();
    m_buttons_wait_to_send = 0;
  } else {
    m_buttons_sent_wait = false;
    m_timer_buttons_send_running = false;
  }
}

static void button_event_handler(uint8_t pin_no, uint8_t button_action)
{
  button_pins_t button_pin = (button_pins_t) pin_no;

  if (button_action) { // when change to enable state only
    if (m_buttons_sent_wait == false) { // sent right now
      buttons_send_pag73(&m_antplus_controls, button_pin);
      m_buttons_sent_wait = true;
    } else {
      m_buttons_wait_to_send = button_pin;
    }

    if (m_timer_buttons_send_running == false) {
      m_timer_buttons_send_running = true;
      start_timer_antplus_controls_send();
    }

    // restart button press timeout timer
    ret_code_t err_code;
    err_code = app_timer_stop(m_timer_button_press_timeout);
    APP_ERROR_CHECK(err_code);
    err_code = app_timer_start(m_timer_button_press_timeout, BUTTON_PRESS_TIMEOUT, NULL);
    APP_ERROR_CHECK(err_code);
  }
}

void buttons_init(void)
{
  ret_code_t err_code;

  //The array must be static because a pointer to it will be saved in the button handler module.
  static app_button_cfg_t buttons[4] =
  {
    {(uint8_t) PLUS__PIN, APP_BUTTON_ACTIVE_LOW, 1, GPIO_PIN_CNF_PULL_Pullup, button_event_handler},
    {(uint8_t) MINUS__PIN, APP_BUTTON_ACTIVE_LOW, 1, GPIO_PIN_CNF_PULL_Pullup, button_event_handler},
    {(uint8_t) ENTER__PIN, APP_BUTTON_ACTIVE_LOW, 1, GPIO_PIN_CNF_PULL_Pullup, button_event_handler},
    {(uint8_t) STANDBY__PIN, APP_BUTTON_ACTIVE_LOW, 1, GPIO_PIN_CNF_PULL_Pullup, button_event_handler}
  };

  err_code = app_button_init(buttons, ARRAY_SIZE(buttons), BUTTON_DETECTION_DELAY);
  // this will enable wakeup from ultra low power mode (any button press)
  nrf_gpio_cfg_sense_input(PLUS__PIN, GPIO_PIN_CNF_PULL_Pullup, GPIO_PIN_CNF_SENSE_Low);
  nrf_gpio_cfg_sense_input(MINUS__PIN, GPIO_PIN_CNF_PULL_Pullup, GPIO_PIN_CNF_SENSE_Low);
  nrf_gpio_cfg_sense_input(ENTER__PIN, GPIO_PIN_CNF_PULL_Pullup, GPIO_PIN_CNF_SENSE_Low);
  nrf_gpio_cfg_sense_input(STANDBY__PIN, GPIO_PIN_CNF_PULL_Pullup, GPIO_PIN_CNF_SENSE_Low);

  if (err_code == NRF_SUCCESS)
  {
    err_code = app_button_enable();
  }
  APP_ERROR_CHECK(err_code);

  err_code = app_timer_create(&m_timer_button_press_timeout,
                  APP_TIMER_MODE_SINGLE_SHOT,
                  timer_button_press_timeout_handler);
  APP_ERROR_CHECK(err_code);
  err_code = app_timer_start(m_timer_button_press_timeout, BUTTON_PRESS_TIMEOUT, NULL);
  APP_ERROR_CHECK(err_code);
}

void shutdown(void)
{
  // // all pins must be disabled or system will wakeup, similar to a reset after enter ultra low power mode
  // // debug pins
  // nrf_gpio_cfg_default(10);
  // nrf_gpio_cfg_default(9);

  // enter in ultra low power mode
  nrf_pwr_mgmt_shutdown(NRF_PWR_MGMT_SHUTDOWN_GOTO_SYSOFF);
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

static void lfclk_config(void)
{
  ret_code_t err_code = nrf_drv_clock_init();
  APP_ERROR_CHECK(err_code);

  nrf_drv_clock_lfclk_request(NULL);
}

static void timer_init(void)
{
  ret_code_t err_code = app_timer_init();
  APP_ERROR_CHECK(err_code);
}

int main(void)
{
  lfclk_config();

  // init power management library
  ret_code_t err_code = nrf_pwr_mgmt_init();
  APP_ERROR_CHECK(err_code);

  // // for debug only
  // nrf_gpio_pin_clear(10);
  // nrf_gpio_cfg_output(10);
  //   // for debug only
  // nrf_gpio_pin_clear(9);
  // nrf_gpio_cfg_output(9);
  // nrf_gpio_pin_set(9);

  timer_init();
  buttons_init();
  softdevice_setup();
  profile_setup();

  err_code = app_timer_create(&m_antplus_controls_send,
                  APP_TIMER_MODE_SINGLE_SHOT,
                  timer_antplus_controls_send_handler);
  APP_ERROR_CHECK(err_code);
    
  for (;;)
  {
    // enter System On sleep mode and manage extra features if they are enable
    nrf_pwr_mgmt_run();

    // // for debug only
    // static bool pin_state = 0;
    // pin_state = !pin_state;
    // if (pin_state)
    //   nrf_gpio_pin_clear(10);
    // else
    //   nrf_gpio_pin_set(10);
  }
}

