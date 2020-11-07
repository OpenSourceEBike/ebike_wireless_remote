/**
 * Copyright (c) 2015 - 2019, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifndef ANT_KEY_MANAGER_CONFIG_H__
#define ANT_KEY_MANAGER_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif
#warning THE PUBLIC ANT+ NETWORK KEYS ARE CURRENTLY BEING APPLIED!! \n
#warning YOU NEED TO REGISTER AS A DEVELOPER WITH GARMIN TO OBTAIN THESE KEYS\n
#warning THE ANT+ LEV and ANT+ CONTROLS PROFILES WILL NOT FUNCTION WITHOUT THESE KEYS\n
#warning DEVELOPMENT AND USAGE IS WITHOUT ANY COSTS IF USED FOR NON-COMMERCIAL PURPOSES\n
#warning FOR MORE INFORMATION AND TO REGISTER, SEE:\n
#warning https://www.thisisant.com/developer/resources/tech-bulletin/network-keys-and-the-ant-managed-network\n
#warning ONCE YOU HAVE THE KEYS, PLEASE ADD THEM TO THE FILE LOCATED HERE:\n
#warning ./nRF5_SDK_16.0.0/components/ant/ant_key_manager/config/ant_key_manager_config.h\n
/**
 * @addtogroup ant_key_manager
 * @{
 */

#ifndef ANT_PLUS_NETWORK_KEY
    #define ANT_PLUS_NETWORK_KEY    {0, 0, 0, 0, 0, 0, 0, 0}            /**< The ANT+ network key. */
#endif //ANT_PLUS_NETWORK_KEY

#ifndef ANT_FS_NETWORK_KEY
    #define ANT_FS_NETWORK_KEY      {0, 0, 0, 0, 0, 0, 0, 0}           /**< The ANT-FS network key. */
#endif // ANT_FS_NETWORK_KEY

/**
 * @}
 */


#ifdef __cplusplus
}
#endif

#endif // ANT_KEY_MANAGER_CONFIG_H__
