/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2013 Paul Kocialkowski <contact@oaulk.fr>
 *
 * libsamsung-ipc is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * libsamsung-ipc is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libsamsung-ipc.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <samsung-ipc.h>

#ifndef __SAMSUNG_IPC_SVC_H__
#define __SAMSUNG_IPC_SVC_H__

/*
 * Commands
 */

#define IPC_SVC_ENTER                                           0x0B01
#define IPC_SVC_END                                             0x0B02
#define IPC_SVC_PRO_KEYCODE                                     0x0B03
#define IPC_SVC_SCREEN_CFG                                      0x0B04
#define IPC_SVC_DISPLAY_SCREEN                                  0x0B05
#define IPC_SVC_CHANGE_SVC_MODE                                 0x0B06
#define IPC_SVC_DEVICE_TEST                                     0x0B07
#define IPC_SVC_DEBUG_DUMP                                      0x0B08
#define IPC_SVC_DEBUG_STRING                                    0x0B09

/*
 * Values
 */

#define IPC_SVC_MODE_TEST_MANUAL                                0x01
#define IPC_SVC_MODE_TEST_AUTO                                  0x02
#define IPC_SVC_MODE_NAM_EDIT                                   0x03
#define IPC_SVC_MODE_MONITOR                                    0x04
#define IPC_SVC_MODE_PHONE_TEST                                 0x05

#define IPC_SVC_TYPE_ENTER                                      0x00
#define IPC_SVC_TYPE_SW_VERSION_ENTER                           0x01
#define IPC_SVC_TYPE_FTA_SW_VERSION_ENTER                       0x02
#define IPC_SVC_TYPE_FTA_HW_VERSION_ENTER                       0x03
#define IPC_SVC_TYPE_ALL_VERSION_ENTER                          0x04
#define IPC_SVC_TYPE_BATTERY_INFO_ENTER                         0x05
#define IPC_SVC_TYPE_CIPHERING_PROTECTION_ENTER                 0x06
#define IPC_SVC_TYPE_INTEGRITY_PROTECTION_ENTER                 0x07
#define IPC_SVC_TYPE_IMEI_READ_ENTER                            0x08
#define IPC_SVC_TYPE_BLUETOOTH_TEST_ENTER                       0x09
#define IPC_SVC_TYPE_VIBRATOR_TEST_ENTER                        0x0A
#define IPC_SVC_TYPE_MELODY_TEST_ENTER                          0x0B
#define IPC_SVC_TYPE_MP3_TEST_ENTER                             0x0C
#define IPC_SVC_TYPE_FACTORY_RESET_ENTER                        0x0D
#define IPC_SVC_TYPE_FACTORY_PRECONFIG_ENTER                    0x0E
#define IPC_SVC_TYPE_TFS4_EXPLORE_ENTER                         0x0F
#define IPC_SVC_TYPE_RSC_FILE_VERSION_ENTER                     0x11
#define IPC_SVC_TYPE_USB_DRIVER_ENTER                           0x12
#define IPC_SVC_TYPE_USB_UART_DIAG_CONTROL_ENTER                0x13
#define IPC_SVC_TYPE_RRC_VERSION_ENTER                          0x14
#define IPC_SVC_TYPE_GPSONE_SS_TEST_ENTER                       0x15
#define IPC_SVC_TYPE_BAND_SEL_ENTER                             0x16
#define IPC_SVC_TYPE_GCF_TESTMODE_ENTER                         0x17
#define IPC_SVC_TYPE_GSM_FACTORY_AUDIO_LB_ENTER                 0x18
#define IPC_SVC_TYPE_FACTORY_VF_TEST_ENTER                      0x19
#define IPC_SVC_TYPE_TOTAL_CALL_TIME_INFO_ENTER                 0x1A
#define IPC_SVC_TYPE_SELLOUT_SMS_ENABLE_ENTER                   0x1B
#define IPC_SVC_TYPE_SELLOUT_SMS_DISABLE_ENTER                  0x1C
#define IPC_SVC_TYPE_SELLOUT_SMS_TEST_MODE_ON                   0x1D
#define IPC_SVC_TYPE_SELLOUT_SMS_PRODUCT_MODE_ON                0x1E
#define IPC_SVC_TYPE_GET_SELLOUT_SMS_INFO_ENTER                 0x1F
#define IPC_SVC_TYPE_TST_AUTO_ANSWER_ENTER                      0x20
#define IPC_SVC_TYPE_TST_NV_RESET_ENTER                         0x21
#define IPC_SVC_TYPE_TST_FTA_SW_VERSION_ENTER                   0x22
#define IPC_SVC_TYPE_TST_FTA_HW_VERSION_ENTER                   0x23

/*
 * Structures
 */

struct ipc_svc_enter_data {
    unsigned char mode; // IPC_SVC_MODE
    unsigned char type; // IPC_SVC_TYPE
    unsigned char unknown;
} __attribute__((__packed__));

struct ipc_svc_end_data {
    unsigned char mode; // IPC_SVC_MODE
} __attribute__((__packed__));

struct ipc_svc_pro_keycode_data {
    unsigned char key;
} __attribute__((__packed__));

struct ipc_svc_screen_cfg_data {
    unsigned char count;
    unsigned char unknown;
} __attribute__((__packed__));

struct ipc_svc_display_screen_header {
    unsigned char count;
} __attribute__((__packed__));

struct ipc_svc_display_screen_data {
    unsigned short index;
    char line[32];
} __attribute__((__packed__));

struct ipc_svc_change_svc_mode_data {
    unsigned char mode; // IPC_SVC_MODE
} __attribute__((__packed__));

#endif

// vim:ts=4:sw=4:expandtab
