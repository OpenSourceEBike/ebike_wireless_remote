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
#include "ant_lev.h"
#include "pins.h"
#include "app_button.h"
#include "nrf_drv_clock.h"
#include "sdk_config.h"
#include "ant_state_indicator.h"
#include "bsp_btn_ant.h"
#include "antplus_controls.h"

#define NRFX_CLOCK_ENABLED 1
#define NRF_CLOCK_ENABLED 1
#define APP_TIMER_ENABLED 1

#define BUTTON_DETECTION_DELAY APP_TIMER_TICKS(50)           /**< Delay from a GPIOTE event until a button is reported as pushed (in number of timer ticks). */
#define BUTTON_PRESS_TIMEOUT APP_TIMER_TICKS(60 * 60 * 1000) // 1h
#define BUTTON_LONG_PRESS_TIMEOUT APP_TIMER_TICKS(1000)      // 1 seconds

APP_TIMER_DEF(m_lev_send);
APP_TIMER_DEF(m_timer_button_press_timeout);
APP_TIMER_DEF(m_timer_button_long_press_timeout);
APP_TIMER_DEF(m_antplus_controls_send);

bool m_buttons_sent_wait = false;
button_pins_t m_buttons_wait_to_send = 0;
bool m_timer_buttons_send_running = false;
bool m_button_long_press = false;

void shutdown(void);
#define CONTROLS_HW_REVISION 2
#define CONTROLS_MANUFACTURER_ID 255
#define CONTROLS_MODEL_NUMBER 2
#define CONTROLS_SW_REVISION_MAJOR 2
#define CONTROLS_SW_REVISION_MINOR 2
#define CONTROLS_SERIAL_NUMBER 3241
#define CONTROLS_CHANNEL_NUM 1 // ?? seems can be any value from 0 to 8
#define ANT_LEV_ANT_OBSERVER_PRIO 1

#define LEV_HW_REVISION 1
#define LEV_MANUFACTURER_ID 254
#define LEV_MODEL_NUMBER 1
#define LEV_SW_REVISION_MAJOR 1
#define LEV_SW_REVISION_MINOR 1
#define LEV_SERIAL_NUMBER 1234
#define LEV_CHANNEL_NUM 0             // ?? seems can be any value from 0 to 8
#define CONTROLS_CHAN_ID_TRANS_TYPE 0 // LEV table 4.2
#define LEV_CHAN_ID_TRANS_TYPE 0
#define CONTROLS_CHAN_ID_DEV_NUM 0 // [1 - 65535], LEV table 4.2 0 for wildcard
#define LEV_CHAN_ID_DEV_NUM 0
#define LEV_ANTPLUS_NETWORK_NUM 0
#define CONTROLS_ANTPLUS_NETWORK_NUM 1
#define ANTPLUS_CONTROLS_ANT_OBSERVER_PRIO 1
// @snippet [ANT LEV RX Instance]
void ant_lev_evt_handler(ant_lev_profile_t *p_profile, ant_lev_evt_t event);
void antplus_controls_evt_handler(antplus_controls_profile_t *p_profile, antplus_controls_evt_t event);

LEV_DISP_CHANNEL_CONFIG_DEF(m_ant_lev,
                            LEV_CHANNEL_NUM,
                            LEV_CHAN_ID_TRANS_TYPE,
                            LEV_CHAN_ID_DEV_NUM,
                            LEV_ANTPLUS_NETWORK_NUM,
                            LEV_MSG_PERIOD_4Hz);

CONTROLS_SENS_CHANNEL_CONFIG_DEF(m_antplus_controls,
                                 CONTROLS_CHANNEL_NUM,
                                 CONTROLS_CHAN_ID_TRANS_TYPE,
                                 CONTROLS_CHAN_ID_DEV_NUM,
                                 CONTROLS_ANTPLUS_NETWORK_NUM);

CONTROLS_SENS_PROFILE_CONFIG_DEF(m_antplus_controls,
                                 antplus_controls_evt_handler);

static ant_lev_profile_t m_ant_lev;
static antplus_controls_profile_t m_antplus_controls;

NRF_SDH_ANT_OBSERVER(m_ant_observer, ANT_LEV_ANT_OBSERVER_PRIO, ant_lev_disp_evt_handler, &m_ant_lev);
NRF_SDH_ANT_OBSERVER(m_ant_observer_control, ANTPLUS_CONTROLS_ANT_OBSERVER_PRIO, antplus_controls_sens_evt_handler, &m_antplus_controls);

uint16_t cnt_1;

void antplus_controls_evt_handler(antplus_controls_profile_t *p_profile, antplus_controls_evt_t event)
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

void ant_lev_evt_handler(ant_lev_profile_t *p_profile, ant_lev_evt_t event)
{
  nrf_pwr_mgmt_feed();

  switch (event)
  {
  case ANT_LEV_PAGE_1_UPDATED:
      //  p_profile->page_16.travel_mode = p_profile->common.travel_mode_state;
      ;
    break;

  case ANT_LEV_PAGE_2_UPDATED:
    break;

  case ANT_LEV_PAGE_3_UPDATED:

    break;

  case ANT_LEV_PAGE_4_UPDATED:
    break;

  case ANT_LEV_PAGE_5_UPDATED:
    break;

  case ANT_LEV_PAGE_34_UPDATED:

    break;

  case ANT_LEV_PAGE_16_UPDATED:

    break;

  case ANT_LEV_PAGE_80_UPDATED:

    break;

  case ANT_LEV_PAGE_81_UPDATED:

    break;

  case ANT_LEV_PAGE_REQUEST_SUCCESS:
    break;

  case ANT_LEV_PAGE_REQUEST_FAILED:
    break;

  default:
    break;
  }
}
static void start_timer_antplus_controls_send(void)
{
  ret_code_t err_code;

  err_code = app_timer_start(m_antplus_controls_send,
                             APP_TIMER_TICKS(0), NULL); //0 works best (no wait)
  APP_ERROR_CHECK(err_code);
}
static void start_timer_lev_send(void)
{
  ret_code_t err_code;

  err_code = app_timer_start(m_lev_send,
                             APP_TIMER_TICKS(0), NULL); //250ms sec wait timer(250) works best with 0
  APP_ERROR_CHECK(err_code);
}

static void timer_button_press_timeout_handler(void *p_context)
{
  UNUSED_PARAMETER(p_context);

  // enter ultra low power mode
  shutdown();
}
static void timer_button_long_press_timeout_handler(void *p_context)
{
  UNUSED_PARAMETER(p_context);

  m_button_long_press = true;
}

static void timer_antplus_controls_send_handler(void *p_context)
{
  UNUSED_PARAMETER(p_context);

  // if there is a button page wait to be sent, sent it
  if (m_buttons_wait_to_send)
  {
    buttons_send_pag73(&m_antplus_controls, m_buttons_wait_to_send);
    m_buttons_wait_to_send = 0;
  }

  // if we sent a button page, start again the timer
  if (m_buttons_wait_to_send)
  {
    start_timer_antplus_controls_send();
    m_buttons_wait_to_send = 0;
  }
  else
  {
    m_buttons_sent_wait = false;
    m_timer_buttons_send_running = false;
  }
}

static void timer_lev_send_handler(void *p_context)
{
  UNUSED_PARAMETER(p_context);

  // if there is a button page wait to be sent, sent it
  if (m_buttons_wait_to_send)
  {
    buttons_send_page16(&m_ant_lev, m_buttons_wait_to_send, m_button_long_press);
    m_buttons_wait_to_send = 0;
    m_button_long_press = false;
  }

  // if we sent a button page, start again the timer
  if (m_buttons_wait_to_send)
  {
    start_timer_lev_send();
    m_buttons_wait_to_send = 0;
  }
  else
  {
    m_buttons_sent_wait = false;
    m_timer_buttons_send_running = false;
  }
}

static void button_event_handler(uint8_t pin_no, uint8_t button_action)
{

  button_pins_t button_pin = (button_pins_t)pin_no;
  ret_code_t err_code;

  switch (button_action)
  {

  case APP_BUTTON_RELEASE:
  {                           // button released
    if (!m_button_long_press) //not a long press
    {

      if (m_buttons_sent_wait == false)
      {
        // sent right now
        buttons_send_page16(&m_ant_lev, button_pin, m_button_long_press);
        m_buttons_sent_wait = true;
        m_button_long_press = false;
      }
      else
      {
        m_buttons_wait_to_send = button_pin;
      }
      if (m_timer_buttons_send_running == false)
      {
        m_timer_buttons_send_running = true;
        start_timer_lev_send();
      }

      // restart button press timeout timer

      err_code = app_timer_stop(m_timer_button_press_timeout);
      APP_ERROR_CHECK(err_code);
      err_code = app_timer_start(m_timer_button_press_timeout, BUTTON_PRESS_TIMEOUT, NULL);
      APP_ERROR_CHECK(err_code);

      // stop button long press timeout timer
      err_code = app_timer_stop(m_timer_button_long_press_timeout); //stop the long press timerf
      APP_ERROR_CHECK(err_code);
      m_button_long_press = false;
      // start_timer_antplus_controls_send(); //start long press timer

      break;
    }
    else
    {                                                               //long button press
      err_code = app_timer_stop(m_timer_button_long_press_timeout); //stop the long press timerf
      APP_ERROR_CHECK(err_code);
      buttons_send_pag73(&m_antplus_controls, button_pin);
      m_button_long_press = false;
    }
  case APP_BUTTON_PUSH: //button pushed
  {

    //start long button timer
    err_code = app_timer_stop(m_timer_button_long_press_timeout); //stop the long press timerf
    APP_ERROR_CHECK(err_code);
    err_code = app_timer_start(m_timer_button_long_press_timeout, BUTTON_LONG_PRESS_TIMEOUT, NULL); //start the long press timerf
    APP_ERROR_CHECK(err_code);
    m_button_long_press = false;
    break;
  }
  }
  }
}
void buttons_init(void)
{
  ret_code_t err_code;

  //The array must be static because a pointer to it will be saved in the button handler module.
  static app_button_cfg_t buttons[4] =
      {
          {(uint8_t)PLUS__PIN, APP_BUTTON_ACTIVE_LOW, 1, GPIO_PIN_CNF_PULL_Pullup, button_event_handler},
          {(uint8_t)MINUS__PIN, APP_BUTTON_ACTIVE_LOW, 1, GPIO_PIN_CNF_PULL_Pullup, button_event_handler},
          {(uint8_t)ENTER__PIN, APP_BUTTON_ACTIVE_LOW, 1, GPIO_PIN_CNF_PULL_Pullup, button_event_handler},
          {(uint8_t)STANDBY__PIN, APP_BUTTON_ACTIVE_LOW, 1, GPIO_PIN_CNF_PULL_Pullup, button_event_handler}};

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
  err_code = app_timer_create(&m_timer_button_long_press_timeout,
                              APP_TIMER_MODE_SINGLE_SHOT,
                              timer_button_long_press_timeout_handler);
  APP_ERROR_CHECK(err_code);
  err_code = app_timer_start(m_timer_button_press_timeout, BUTTON_PRESS_TIMEOUT, NULL);
  APP_ERROR_CHECK(err_code);
  err_code = app_timer_stop(m_timer_button_long_press_timeout); //stop the long press timer
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

  m_ant_lev.page_80 = ANT_COMMON_page80(LEV_HW_REVISION,
                                        LEV_MANUFACTURER_ID,
                                        LEV_MODEL_NUMBER);
  // fill product's common data page.
  m_ant_lev.page_81 = ANT_COMMON_page81(LEV_SW_REVISION_MAJOR,
                                        LEV_SW_REVISION_MINOR,
                                        LEV_SERIAL_NUMBER);

  //@snippet [ANT LEV RX Profile Setup]
  err_code = ant_lev_disp_init(&m_ant_lev, LEV_DISP_CHANNEL_CONFIG(m_ant_lev), ant_lev_evt_handler);
  APP_ERROR_CHECK(err_code);
  err_code = antplus_controls_sens_init(&m_antplus_controls,
                                        CONTROLS_SENS_CHANNEL_CONFIG(m_antplus_controls),
                                        CONTROLS_SENS_PROFILE_CONFIG(m_antplus_controls));
  APP_ERROR_CHECK(err_code);
  // @snippet [ANT LEV RX Profile Setup]

  /*  
    //set the starting values
    m_ant_lev.page_1.error_message = 0;
    m_ant_lev.page_1.temperature_state = 0x011;
    m_ant_lev.page_2.remaining_range = 0;
    m_ant_lev.page_3.battery_soc = 100;
    m_ant_lev.page_3.assist =8;
    m_ant_lev.page_4.charging_cycle_count = 0;
    m_ant_lev.page_4.fuel_consumption = 0;
    m_ant_lev.page_4.battery_voltage = 208; // 52V / 0.25 = 208
    m_ant_lev.page_4.distance_on_current_charge = 0;
    m_ant_lev.page_5.travel_modes_supported = 0x38; // 7 assist levels
    m_ant_lev.page_5.wheel_circumference = 2050;
    m_ant_lev.page_34.fuel_consumption = 0;
   m_ant_lev.common.travel_mode_state = 8;//assist 1
    m_ant_lev.common.system_state = 0;
    m_ant_lev.common.gear_state = 0;
    m_ant_lev.common.lev_speed = 200;
    m_ant_lev.common.odometer = 0;
*/
  err_code = antplus_controls_sens_open(&m_antplus_controls);
  APP_ERROR_CHECK(err_code);
  err_code = ant_lev_disp_open(&m_ant_lev);
  APP_ERROR_CHECK(err_code);

  err_code = ant_state_indicator_channel_opened();
  APP_ERROR_CHECK(err_code);
}

static void softdevice_setup(void)
{
  ret_code_t err_code = nrf_sdh_enable_request();
  APP_ERROR_CHECK(err_code);

  ASSERT(nrf_sdh_is_enabled());

  err_code = nrf_sdh_ant_enable();
  APP_ERROR_CHECK(err_code);
  err_code = ant_plus_key_set(CONTROLS_ANTPLUS_NETWORK_NUM);
  APP_ERROR_CHECK(err_code);
  err_code = ant_plus_key_set(LEV_ANTPLUS_NETWORK_NUM);
  APP_ERROR_CHECK(err_code);
}

static void log_init(void)
{
  ret_code_t err_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(err_code);

  NRF_LOG_DEFAULT_BACKENDS_INIT();
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

  timer_init();
  lfclk_config();

  ret_code_t err_code = nrf_pwr_mgmt_init();
  APP_ERROR_CHECK(err_code);

  log_init();
  pins_init();

  err_code = ant_state_indicator_init(0, 0);
  APP_ERROR_CHECK(err_code);
  buttons_init();
  softdevice_setup();
  profile_setup();

  err_code = app_timer_create(&m_antplus_controls_send,
                              APP_TIMER_MODE_SINGLE_SHOT,
                              timer_antplus_controls_send_handler);
  APP_ERROR_CHECK(err_code);
  err_code = app_timer_create(&m_lev_send,
                              APP_TIMER_MODE_SINGLE_SHOT,
                              timer_lev_send_handler);
  APP_ERROR_CHECK(err_code);

  for (;;)
  {
    NRF_LOG_FLUSH();
    nrf_pwr_mgmt_run();
  }
}
