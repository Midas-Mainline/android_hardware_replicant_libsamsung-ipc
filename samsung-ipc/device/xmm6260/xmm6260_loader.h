/*
 * XMM6260 Firmware loader functions
 * Copyright (C) 2012 Alexander Tarasikov <alexander.tarasikov@gmail.com>
 * Copyright (C) 2012 Paul Kocialkowski <contact@paulk.fr>
 *
 * based on the incomplete C++ implementation which is
 * Copyright (C) 2012 Sergey Gridasov <grindars@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
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

#ifndef __XMM6260_LOADER_H__
#define __XMM6260_LOADER_H__

#define RADIO_MAP_SIZE (16 << 20)
#define DEFAULT_TIMEOUT 50

/*
 * Offset and length to describe a part of XMM6260 firmware
 */
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

/*
 * @brief Calculate the checksum for the XMM6260 bootloader protocol
 *
 * @param data [in] the data to calculate the checksum for
 * @param offset [in] number of bytes to skip
 * @param length [in] length of data in bytes
 * @return checksum value
 */
unsigned char xmm6260_crc_calculate(void* data, size_t offset, size_t length);

/* 
 * @brief Waits for fd to become available for reading
 *
 * @param fd [in] File descriptor of the socket
 * @param timeout [in] Timeout in milliseconds
 * @return Negative value indicating error code
 * @return Available socket number - 1, as select()
 */
int expect(int fd, unsigned timeout);

/* 
 * @brief Waits for data available and reads it to the buffer
 *
 * @param fd [in] File descriptor of the socket
 * @param buf Buffer to hold data
 * @param size [in] The number of bytes to read
 * @return Negative value indicating error code
 * @return The size of data received
 */
int expect_read(int fd, void *buf, size_t size);

/* 
 * @brief Receives data and compares with the pattern in memory
 *
 * @param fd [in] File descriptor of the socket
 * @param data [in] The pattern to compare to
 * @param size [in] The length of data to read in bytes
 * @return Negative value indicating error code
 * @return Available socket number - 1, as select()
 */
int expect_data(int fd, void *data, size_t size);

#endif
