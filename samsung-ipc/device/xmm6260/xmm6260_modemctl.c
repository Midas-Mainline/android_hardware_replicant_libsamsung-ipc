/*
 * XMM6260 Modem Control functions
 * Copyright (C) 2012 Alexander Tarasikov <alexander.tarasikov@gmail.com>
 * Copyright (C) 2012 Paul Kocialkowski <contact@paulk.fr>
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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <getopt.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>

//for timeval
#include <sys/time.h>

//for mmap
#include <sys/mman.h>
#include <sys/stat.h>

#include "ipc_private.h"

#include "xmm6260_loader.h"
#include "xmm6260_modemctl.h"
#include "modem_prj.h"

/*
 * modemctl generic functions
 */

int modemctl_link_set_active(struct ipc_client *client,
    struct modemctl_io_data *io_data, bool enabled) {
    unsigned status = enabled;
    int ret;
    unsigned long ioctl_code;

    ioctl_code = IOCTL_LINK_CONTROL_ACTIVE;
    ret = ioctl(io_data->link_fd, ioctl_code, &status);

    if (ret < 0) {
        ipc_client_log(client, "failed to set link active to %d", enabled);
        goto fail;
    }

    return 0;
fail:
    return ret;
}

int modemctl_link_set_enabled(struct ipc_client *client,
    struct modemctl_io_data *io_data, bool enabled) {
    unsigned status = enabled;
    int ret;
    unsigned long ioctl_code;

    ioctl_code = IOCTL_LINK_CONTROL_ENABLE;
    ret = ioctl(io_data->link_fd, ioctl_code, &status);

    if (ret < 0) {
        ipc_client_log(client, "failed to set link state to %d", enabled);
        goto fail;
    }

    return 0;
fail:
    return ret;
}

int modemctl_wait_link_ready(struct ipc_client *client,
    struct modemctl_io_data *io_data) {
    int ret;

    struct timeval tv_start = {};
    struct timeval tv_end = {};

    gettimeofday(&tv_start, 0);;

    //link wakeup timeout in milliseconds
    long diff = 0;

    do {
        ret = ioctl(io_data->link_fd, IOCTL_LINK_CONNECTED, 0);
        if (ret < 0) {
            goto fail;
        }

        if (ret == 1) {
            return 0;
        }

        usleep(LINK_POLL_DELAY_US);
        gettimeofday(&tv_end, 0);;

        diff = (tv_end.tv_sec - tv_start.tv_sec) * 1000;
        diff += (tv_end.tv_usec - tv_start.tv_usec) / 1000;
    } while (diff < LINK_TIMEOUT_MS);

    ret = -ETIMEDOUT;

fail:
    return ret;
}

int modemctl_wait_modem_online(struct ipc_client *client,
    struct modemctl_io_data *io_data) {
    int ret;

    struct timeval tv_start = {};
    struct timeval tv_end = {};

    gettimeofday(&tv_start, 0);;

    //link wakeup timeout in milliseconds
    long diff = 0;

    do {
        ret = ioctl(io_data->boot_fd, IOCTL_MODEM_STATUS, 0);
        if (ret < 0) {
            goto fail;
        }

        if (ret == STATE_ONLINE) {
            return 0;
        }

        usleep(LINK_POLL_DELAY_US);
        gettimeofday(&tv_end, 0);;

        diff = (tv_end.tv_sec - tv_start.tv_sec) * 1000;
        diff += (tv_end.tv_usec - tv_start.tv_usec) / 1000;
    } while (diff < LINK_TIMEOUT_MS);

    ret = -ETIMEDOUT;

fail:
    return ret;
}

int modemctl_modem_power(struct ipc_client *client,
    struct modemctl_io_data *io_data, bool enabled) {
    if (enabled) {
        return ioctl(io_data->boot_fd, IOCTL_MODEM_ON, 0);
    }
    else {
        return ioctl(io_data->boot_fd, IOCTL_MODEM_OFF, 0);
    }
    return -1;
}

int modemctl_modem_boot_power(struct ipc_client *client,
    struct modemctl_io_data *io_data, bool enabled) {
    if (enabled) {
        return ioctl(io_data->boot_fd, IOCTL_MODEM_BOOT_ON, 0);
    }
    else {
        return ioctl(io_data->boot_fd, IOCTL_MODEM_BOOT_OFF, 0);
    }
    return -1;
}

// vim:ts=4:sw=4:expandtab
