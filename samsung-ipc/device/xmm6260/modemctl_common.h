/*
 * Firmware loader for Samsung I9100 and I9250
 * Copyright (C) 2012 Alexander Tarasikov <alexander.tarasikov@gmail.com>
 *
 * based on the incomplete C++ implementation which is
 * Copyright (C) 2012 Sergey Gridasov <grindars@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __MODEMCTL_COMMON_H__
#define __MODEMCTL_COMMON_H__

#include <radio.h>

#include "common.h"
#include "log.h"
#include "io_helpers.h"

//Samsung IOCTLs
#include "modem_prj.h"

#define MODEM_DEVICE(x) ("/dev/" #x)
#define LINK_PM MODEM_DEVICE(link_pm)
#define MODEM_DEV MODEM_DEVICE(modem_br)
#define BOOT_DEV MODEM_DEVICE(umts_boot0)
#define IPC_DEV MODEM_DEVICE(umts_ipc0)
#define RFS_DEV MODEM_DEVICE(umts_rfs0)

#define LINK_POLL_DELAY_US (50 * 1000)
#define LINK_TIMEOUT_MS 2000

#define RADIO_MAP_SIZE (16 << 20)

struct xmm6260_radio_part {
    size_t offset;
    size_t length;
};

/*
 * Components of the Samsung XMM6260 firmware
 */
enum xmm6260_image {
    PSI,
    EBL,
    SECURE_IMAGE,
    FIRMWARE,
    NVDATA,
};

/*
 * Bootloader control interface definitions
 */

enum xmm6260_boot_cmd {
    SetPortConf,

    ReqSecStart,
    ReqSecEnd,
    ReqForceHwReset,

    ReqFlashSetAddress,
    ReqFlashWriteBlock,
};

struct modemctl_io_data {
    int link_fd;
    int boot_fd;

    int radio_fd;
    char *radio_data;
    struct stat radio_stat;

    struct xmm6260_radio_part *radio_parts;
    int radio_parts_count;
};

/*
 * Function prototypes
 */

/*
 * @brief Activates the modem <-> cpu link data transfer
 *
 * @param client [in] ipc client
 * @param io_data [in] modemctl-specific data
 * @param enabled [in] whether to enable or disable link data transport
 * @return Negative value indicating error code
 * @return ioctl call result
 */
int modemctl_link_set_active(struct ipc_client *client,
    struct modemctl_io_data *io_data, bool enabled);

/*
 * @brief Activates the modem <-> cpu link connection
 *
 * @param client [in] ipc client
 * @param io_data [in] modemctl-specific data
 * @param enabled [in] the state to set link to
 * @return Negative value indicating error code
 * @return ioctl call result
 */
int modemctl_link_set_enabled(struct ipc_client *client,
    struct modemctl_io_data *io_data, bool enabled);

/*
 * @brief Poll the link until it gets ready or times out
 *
 * @param client [in] ipc client
 * @param io_data [in] modemctl-specific data
 * @return Negative value indicating error code
 * @return ioctl call result
 */
int modemctl_wait_link_ready(struct ipc_client *client,
    struct modemctl_io_data *io_data);

/*
 * @brief Poll the modem until it gets online or times out
 *
 * @param client [in] ipc client
 * @param io_data [in] modemctl-specific data
 * @return Negative value indicating error code
 * @return ioctl call result
 */
int modemctl_wait_modem_online(struct ipc_client *client,
    struct modemctl_io_data *io_data);

/*
 * @brief Sets the modem power
 *
 * @param client [in] ipc client
 * @param io_data [in] modemctl-specific data
 * @param enabled [in] whether to enable or disable modem power
 * @return Negative value indicating error code
 * @return ioctl call result
 */
int modemctl_modem_power(struct ipc_client *client,
    struct modemctl_io_data *io_data, bool enabled);

/*
 * @brief Sets the modem bootloader power/UART configuration
 *
 * @param client [in] ipc client
 * @param io_data [in] modemctl-specific data
 * @param enabled [in] whether to enable or disable power
 * @return Negative value indicating error code
 * @return ioctl call result
 */
int modemctl_modem_boot_power(struct ipc_client *client,
    struct modemctl_io_data *io_data, bool enabled);

/*
 * @brief Calculate the checksum for the XMM6260 bootloader protocol
 *
 * @param data [in] the data to calculate the checksum for
 * @param offset [in] number of bytes to skip
 * @param length [in] length of data in bytes
 * @return checksum value
 */
unsigned char calculateCRC(void* data, size_t offset, size_t length);

#endif //__MODEMCTL_COMMON_H__

// vim:ts=4:sw=4:expandtab
