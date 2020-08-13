/*
 * TSDZ2 EBike wireless firmware
 *
 * Copyright (C) Casainho, 2020
 *
 * Released under the GPL License, Version 3
 */

#include "sdk_common.h"

#include "nrf_assert.h"
#include "app_error.h"
#include "ant_interface.h"
#include "antplus_controls_pages.h"
#include "antplus_controls.h"
#include "app_error.h"

#define COMMON_DATA_INTERVAL 20          /**< Common data page is sent every 20th message. */

typedef struct
{
    antplus_controls_page_t  page_number;
    uint8_t         page_payload[7];
} antplus_controls_message_layout_t;

static ret_code_t antplus_controls_init(antplus_controls_profile_t          * p_profile,
                               ant_channel_config_t const * p_channel_config)
{
    p_profile->channel_number = p_channel_config->channel_number;

    p_profile->page_17 = DEFAULT_ANTPLUS_CONTROLS_PAGE17();
    p_profile->page_17 = DEFAULT_ANTPLUS_CONTROLS_PAGE82();
    p_profile->page_80 = DEFAULT_ANT_COMMON_page80();
    p_profile->page_81 = DEFAULT_ANT_COMMON_page81();

    return ant_channel_init(p_channel_config);
}

ret_code_t antplus_controls_sens_init(antplus_controls_profile_t * p_profile,
                             ant_channel_config_t const  * p_channel_config,
                             antplus_controls_sens_config_t const * p_sens_config)
{
    ASSERT(p_profile != NULL);
    ASSERT(p_channel_config != NULL);
    ASSERT(p_sens_config != NULL);
    ASSERT(p_sens_config->p_cb != NULL);
    ASSERT(p_sens_config->evt_handler != NULL);

    p_profile->evt_handler    = p_sens_config->evt_handler;
    p_profile->_cb.p_sens_cb = p_sens_config->p_cb;
    ant_request_controller_init(&(p_profile->_cb.p_sens_cb->req_controller));

    p_profile->_cb.p_sens_cb->block_cnt    = 0;
    p_profile->_cb.p_sens_cb->pag_2_34    = 0;
    p_profile->_cb.p_sens_cb->pag_4_5    = 0;
    p_profile->_cb.p_sens_cb->message_counter = 0;
    p_profile->_cb.p_sens_cb->common_page_number = ANTPLUS_CONTROLS_PAGE_80;

    return antplus_controls_init(p_profile, p_channel_config);
}

static void sens_message_encode(antplus_controls_profile_t * p_profile, uint8_t * p_message_payload)
{
    antplus_controls_message_layout_t * p_CONTROLS_message_payload =
        (antplus_controls_message_layout_t *)p_message_payload;

    switch (p_CONTROLS_message_payload->page_number)
    {
        case ANTPLUS_CONTROLS_PAGE_17:
            antplus_controls_page_17_encode(p_CONTROLS_message_payload->page_payload, &(p_profile->page_17),
                                  &(p_profile->common));
            break;

        case ANTPLUS_CONTROLS_PAGE_80:
            antplus_common_page_80_encode(p_CONTROLS_message_payload->page_payload, &(p_profile->page_80));
            break;

        case ANTPLUS_CONTROLS_PAGE_81:
            antplus_common_page_81_encode(p_CONTROLS_message_payload->page_payload, &(p_profile->page_81));
            break;

        default:
            return;
    }

    p_profile->evt_handler(p_profile, (antplus_controls_evt_t)p_CONTROLS_message_payload->page_number);
}

static void disp_message_decode(antplus_controls_profile_t * p_profile, uint8_t * p_message_payload)
{
    const antplus_controls_message_layout_t * p_CONTROLS_message_payload =
        (antplus_controls_message_layout_t *)p_message_payload;

    switch (p_CONTROLS_message_payload->page_number)
    {
        case ANTPLUS_CONTROLS_PAGE_16:
            antplus_controls_page_16_decode(p_CONTROLS_message_payload->page_payload,
                                  &(p_profile->page_16));
            break;

        default:
            return;
    }

    p_profile->evt_handler(p_profile, (antplus_controls_evt_t)p_CONTROLS_message_payload->page_number);
}

void antplus_controls_sens_evt_handler(ant_evt_t * p_ant_evt, void * p_context)
{
    ASSERT(p_context   != NULL);
    ASSERT(p_ant_evt != NULL);
    antplus_controls_profile_t * p_profile = (antplus_controls_profile_t *)p_context;

    if (p_ant_evt->channel == p_profile->channel_number)
    {
        uint32_t err_code;
        uint8_t p_message_payload[ANT_STANDARD_DATA_PAYLOAD_SIZE];
        antplus_controls_sens_cb_t * p_CONTROLS_cb = p_profile->_cb.p_sens_cb;
        ant_request_controller_sens_evt_handler(&(p_CONTROLS_cb->req_controller), p_ant_evt);

        switch (p_ant_evt->event)
        {
            case EVENT_TX:
            case EVENT_TRANSFER_TX_FAILED:
            case EVENT_TRANSFER_TX_COMPLETED:
                sens_message_encode(p_profile, p_message_payload);
                if (ant_request_controller_ack_needed(&(p_CONTROLS_cb->req_controller)))
                {
                    err_code = sd_ant_acknowledge_message_tx(p_profile->channel_number,
                                                             sizeof(p_message_payload),
                                                             p_message_payload);
                }
                else
                {
                    err_code = sd_ant_broadcast_message_tx(p_profile->channel_number,
                                                           sizeof(p_message_payload),
                                                           p_message_payload);
                }
                APP_ERROR_CHECK(err_code);
                break;

            case EVENT_RX:
                if (p_ant_evt->message.ANT_MESSAGE_ucMesgID == MESG_BROADCAST_DATA_ID
                 || p_ant_evt->message.ANT_MESSAGE_ucMesgID == MESG_ACKNOWLEDGED_DATA_ID
                 || p_ant_evt->message.ANT_MESSAGE_ucMesgID == MESG_BURST_DATA_ID)
                {
                    disp_message_decode(p_profile, p_ant_evt->message.ANT_MESSAGE_aucPayload);
                }
                break;

            default:
                break;
        }
    }
}

ret_code_t antplus_controls_sens_open(antplus_controls_profile_t * p_profile)
{
  // send nothing as controls profile should send only on a request

  (void) p_profile;
  return NRF_SUCCESS;
}