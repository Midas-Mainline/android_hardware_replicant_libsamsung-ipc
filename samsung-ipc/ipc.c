/**
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2010-2011 Joerie de Gram <j.de.gram@gmail.com>
 *               2011 Simon Busch <morphis@gravedo.de>
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
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdbool.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <asm/types.h>
#include <sys/utsname.h>

#include <radio.h>

#include "ipc_private.h"
#include "ipc_devices.h"

void log_handler_default(const char *message, void *user_data)
{
    printf("%s\n", message);
}

void ipc_client_log(struct ipc_client *client, const char *message, ...)
{
    if (!message || !client || !client->log_handler) {
        return;
    }

    va_list args;
    char buffer[4096];

    va_start(args, message);
    vsprintf(buffer, message, args);
    client->log_handler(buffer, client->log_data);
    va_end(args);
}

int ipc_device_detect(void)
{
    char *board_name = NULL;
    char *kernel_version = NULL;
    int index = -1;
    int i;

#ifdef IPC_BOARD_NAME_EXPLICIT
    board_name = strdup(IPC_BOARD_NAME_EXPLICIT);
#else
    char buf[4096];

    // gather board name type from /proc/cpuinfo
    int fd = open("/proc/cpuinfo", O_RDONLY);
    int bytesread = read(fd, buf, 4096);
    close(fd);

    // match hardware name with our supported devices
    char *pch = strtok(buf, "\n");
    while (pch != NULL)
    {
        int rc;
        if ((rc = strncmp(pch, "Hardware", 9)) == 9)
        {
            char *str = (void *) (pch + 9);
            int len = strlen(str);
            char tmp;

            for (i=0 ; i < len ; i++)
            {
                tmp = (char) tolower(str[i]);
                str[i] = tmp;
            }

            board_name = strdup(pch);
        }
        pch = strtok(NULL, "\n");
    }
#endif

#ifdef IPC_KERNEL_VERSION_EXPLICIT
    kernel_version = strdup(IPC_KERNEL_VERSION_EXPLICIT);
#else
    struct utsname utsname;
    memset(&utsname, 0, sizeof(utsname));

    uname(&utsname);

    kernel_version = strdup(utsname.release);
#endif

    for (i=0 ; i < ipc_devices_count ; i++)
    {
        if (strstr(board_name, ipc_devices[i].board_name) != NULL)
        {
            if (ipc_devices[i].kernel_version != NULL)
            {
                if (strncmp(kernel_version, ipc_devices[i].kernel_version, strlen(ipc_devices[i].kernel_version)) == 0)
                {
                    index = i;
                    break;
                } else {
                    // Kernel version didn't match but it may still work
                    index = i;
                }
            } else {
                index = i;
                break;
            }
        }
    }

    if (board_name != NULL)
        free(board_name);

    if (kernel_version != NULL)
        free(kernel_version);

    return index;
}

struct ipc_client *ipc_client_new(int client_type)
{
    struct ipc_client *client;
    int device_index = -1;

    device_index = ipc_device_detect();

    if (device_index < 0 || device_index > ipc_devices_count)
        return NULL;

    if (client_type < 0 || client_type > IPC_CLIENT_TYPE_RFS)
        return NULL;

    client = (struct ipc_client *) malloc(sizeof(struct ipc_client));
    memset(client, 0, sizeof(struct ipc_client));

    client->type = client_type;

    switch (client_type)
    {
        case IPC_CLIENT_TYPE_RFS:
            client->ops = ipc_devices[device_index].rfs_ops;
            break;
        case IPC_CLIENT_TYPE_FMT:
            client->ops = ipc_devices[device_index].fmt_ops;
            break;
    }

    client->handlers = (struct ipc_handlers *) malloc(sizeof(struct ipc_handlers));
    client->gprs_specs = ipc_devices[device_index].gprs_specs;
    client->nv_data_specs = ipc_devices[device_index].nv_data_specs;

    if (ipc_devices[device_index].handlers != 0)
        memcpy(client->handlers, ipc_devices[device_index].handlers, sizeof(struct ipc_handlers));

    return client;
}

int ipc_client_free(struct ipc_client *client)
{
    free(client->handlers);
    free(client);
    client = NULL;
    return 0;
}

int ipc_client_set_log_handler(struct ipc_client *client, ipc_client_log_handler_cb log_handler_cb, void *user_data)
{
    if (client == NULL)
        return -1;

    client->log_handler = log_handler_cb;
    client->log_data = user_data;

    return 0;
}

int ipc_client_set_handlers(struct ipc_client *client, struct ipc_handlers *handlers)
{
    if (client == NULL ||
       handlers == NULL)
        return -1;

    memcpy(client->handlers, handlers, sizeof(struct ipc_handlers));

    return 0;
}

int ipc_client_set_io_handlers(struct ipc_client *client,
                               ipc_io_handler_cb read, void *read_data,
                               ipc_io_handler_cb write, void *write_data)
{
    if (client == NULL ||
       client->handlers == NULL)
        return -1;

    if (read != NULL)
        client->handlers->read = read;
    if (read_data != NULL)
        client->handlers->read_data = read_data;
    if (write != NULL)
        client->handlers->write = write;
    if (write_data != NULL)
        client->handlers->write_data = write_data;

    return 0;
}

int ipc_client_set_handlers_common_data(struct ipc_client *client, void *data)
{
    void *common_data;

    if (client == NULL ||
       client->handlers == NULL ||
       data == NULL)
        return -1;

    common_data = data;
    client->handlers->common_data = common_data;

    client->handlers->read_data = common_data;
    client->handlers->write_data = common_data;
    client->handlers->open_data = common_data;
    client->handlers->close_data = common_data;
    client->handlers->power_on_data = common_data;
    client->handlers->power_off_data = common_data;
    client->handlers->gprs_activate_data = common_data;
    client->handlers->gprs_deactivate_data = common_data;

    return 0;
}

void *ipc_client_get_handlers_common_data(struct ipc_client *client)
{
    if (client == NULL ||
       client->handlers == NULL)
        return NULL;

    return client->handlers->common_data;
}

int ipc_client_create_handlers_common_data(struct ipc_client *client)
{
    void *common_data;

    if (client == NULL ||
       client->handlers == NULL)
        return -1;

    common_data = client->handlers->common_data_create();
    client->handlers->common_data = common_data;

    client->handlers->read_data = common_data;
    client->handlers->write_data = common_data;
    client->handlers->open_data = common_data;
    client->handlers->close_data = common_data;
    client->handlers->power_on_data = common_data;
    client->handlers->power_off_data = common_data;
    client->handlers->gprs_activate_data = common_data;
    client->handlers->gprs_deactivate_data = common_data;

    return 0;
}

int ipc_client_destroy_handlers_common_data(struct ipc_client *client)
{
    void *common_data;
    int rc;

    if (client == NULL ||
       client->handlers == NULL ||
       client->handlers->common_data_destroy == NULL)
        return -1;

    rc = client->handlers->common_data_destroy(client->handlers->common_data);

    if (rc < 0)
        return -1;

    common_data = NULL;
    client->handlers->common_data = common_data;

    client->handlers->read_data = common_data;
    client->handlers->write_data = common_data;
    client->handlers->open_data = common_data;
    client->handlers->close_data = common_data;
    client->handlers->power_on_data = common_data;
    client->handlers->power_off_data = common_data;
    client->handlers->gprs_activate_data = common_data;
    client->handlers->gprs_deactivate_data = common_data;

    return 0;
}

int ipc_client_set_handlers_common_data_fd(struct ipc_client *client, int fd)
{
    if (client == NULL ||
       client->handlers == NULL ||
       client->handlers->common_data_set_fd == NULL)
        return -1;

    return client->handlers->common_data_set_fd(client->handlers->common_data, fd);
}

int ipc_client_get_handlers_common_data_fd(struct ipc_client *client)
{
    if (client == NULL ||
       client->handlers == NULL ||
       client->handlers->common_data_get_fd == NULL)
        return -1;

    return client->handlers->common_data_get_fd(client->handlers->common_data);
}


int ipc_client_bootstrap_modem(struct ipc_client *client)
{
    if (client == NULL ||
        client->ops == NULL ||
        client->ops->bootstrap == NULL)
        return -1;

    return client->ops->bootstrap(client);
}

int ipc_client_open(struct ipc_client *client)
{
    int type;
    int fd;

    if (client == NULL ||
        client->handlers == NULL ||
        client->handlers->open == NULL)
        return -1;

    type = client->type;

    return client->handlers->open(type, client->handlers->open_data);
}

int ipc_client_close(struct ipc_client *client)
{
    if (client == NULL ||
        client->handlers == NULL ||
        client->handlers->close == NULL)
        return -1;

    return client->handlers->close(client->handlers->close_data);
}

int ipc_client_power_on(struct ipc_client *client)
{
    if (client == NULL ||
        client->handlers == NULL ||
        client->handlers->power_on == NULL)
        return -1;

    return client->handlers->power_on(client->handlers->power_on_data);
}

int ipc_client_power_off(struct ipc_client *client)
{
    if (client == NULL ||
        client->handlers == NULL ||
        client->handlers->power_off == NULL)
        return -1;

    return client->handlers->power_off(client->handlers->power_off_data);
}

int ipc_client_gprs_handlers_available(struct ipc_client *client)
{
    if (client == NULL ||
        client->handlers == NULL)
        return -1;

    if(client->handlers->gprs_activate != NULL && 
        client->handlers->gprs_deactivate != NULL)
        return 1;
    else
        return 0;
}

int ipc_client_gprs_activate(struct ipc_client *client, int cid)
{
    if (client == NULL ||
        client->handlers == NULL ||
        client->handlers->gprs_activate == NULL)
        return -1;

    return client->handlers->gprs_activate(client->handlers->gprs_activate_data, cid);
}

int ipc_client_gprs_deactivate(struct ipc_client *client, int cid)
{
    if (client == NULL ||
        client->handlers == NULL ||
        client->handlers->gprs_deactivate == NULL)
        return -1;

    return client->handlers->gprs_deactivate(client->handlers->gprs_deactivate_data, cid);
}

char *ipc_client_gprs_get_iface(struct ipc_client *client, int cid)
{
    if (client == NULL ||
        client->gprs_specs == NULL ||
        client->gprs_specs->gprs_get_iface == NULL)
        return NULL;

    return client->gprs_specs->gprs_get_iface(cid);
}

int ipc_client_gprs_get_capabilities(struct ipc_client *client, struct ipc_client_gprs_capabilities *cap)
{
    if (client == NULL ||
        client->gprs_specs == NULL ||
        client->gprs_specs->gprs_get_capabilities == NULL)
        return -1;

    return client->gprs_specs->gprs_get_capabilities(cap);
}

int _ipc_client_send(struct ipc_client *client, struct ipc_message_info *request)
{
    if (client == NULL ||
        client->ops == NULL ||
        client->ops->send == NULL)
        return -1;

    return client->ops->send(client, request);
}

inline void ipc_client_send_get(struct ipc_client *client, const unsigned short command, unsigned char mseq)
{
    ipc_client_send(client, command, IPC_TYPE_GET, 0, 0, mseq);
}

inline void ipc_client_send_exec(struct ipc_client *client, const unsigned short command, unsigned char mseq)
{
    ipc_client_send(client, command, IPC_TYPE_EXEC, 0, 0, mseq);
}

void ipc_client_send(struct ipc_client *client, const unsigned short command, const char type, unsigned char *data, const int length, unsigned char mseq)
{
    struct ipc_message_info request;

    request.mseq = mseq;
    request.aseq = 0xff;
    request.group = IPC_GROUP(command);
    request.index = IPC_INDEX(command);
    request.type = type;
    request.length = length;
    request.data = data;

    _ipc_client_send(client, &request);
}

int ipc_client_recv(struct ipc_client *client, struct ipc_message_info *response)
{
    if (client == NULL ||
        client->ops == NULL ||
        client->ops->recv == NULL)
        return -1;

    return client->ops->recv(client, response);
}

void ipc_client_response_free(struct ipc_client *client, struct ipc_message_info *response)
{
    if (response->data != NULL) {
        free(response->data);
        response->data = NULL;
    }
}

// vim:ts=4:sw=4:expandtab
