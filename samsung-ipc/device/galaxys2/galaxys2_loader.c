/*
 * Firmware loader for Samsung I9100 (galaxys2)
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

#include "galaxys2_loader.h"
#include "xmm6260_loader.h"
#include "xmm6260_modemctl.h"
#include "modem_prj.h"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

/*
 * Locations of the firmware components in the Samsung firmware
 */

struct xmm6260_radio_part galaxys2_radio_parts[] = {
    [PSI] = {
        .offset = 0,
        .length = 0xf000,
    },
    [EBL] = {
        .offset = 0xf000,
        .length = 0x19000,
    },
    [SECURE_IMAGE] = {
        .offset = 0x9ff800,
        .length = 0x800,
    },
    [FIRMWARE] = {
        .offset = 0x28000,
        .length = 0x9d8000,
    },
    [NVDATA] = {
        .offset = 0xa00000,
        .length = 2 << 20,
    }
};

struct galaxys2_boot_cmd_desc galaxys2_boot_cmd_desc[] = {
    [SetPortConf] = {
        .code = 0x86,
        .data_size = 0x800,
        .need_ack = 1,
    },
    [ReqSecStart] = {
        .code = 0x204,
        .data_size = 0x4000,
        .need_ack = 1,
    },
    [ReqSecEnd] = {
        .code = 0x205,
        .data_size = 0x4000,
        .need_ack = 1,
    },
    [ReqForceHwReset] = {
        .code = 0x208,
        .data_size = 0x4000,
        .need_ack = 0,
    },
    [ReqFlashSetAddress] = {
        .code = 0x802,
        .data_size = 0x4000,
        .need_ack = 1,
    },
    [ReqFlashWriteBlock] = {
        .code = 0x804,
        .data_size = 0x4000,
        .need_ack = 0,
    }
};

static int galaxys2_send_image(struct ipc_client *client,
    struct modemctl_io_data *io_data, enum xmm6260_image type)
{
    int ret = -1;

    if (type >= ARRAY_SIZE(galaxys2_radio_parts)) {
        ipc_client_log(client, "Error: bad image type %x", type);
        goto fail;
    }
    size_t length = galaxys2_radio_parts[type].length;
    size_t offset = galaxys2_radio_parts[type].offset;

    size_t start = offset;
    size_t end = length + start;

    //dump some image bytes
    ipc_client_log(client, "image start");

    while (start < end) {
        ret = write(io_data->boot_fd, io_data->radio_data + start, end - start);
        if (ret < 0) {
            ipc_client_log(client, "failed to write image chunk");
            goto fail;
        }
        start += ret;
    }

    unsigned char crc = xmm6260_crc_calculate(io_data->radio_data, offset, length);

    if ((ret = write(io_data->boot_fd, &crc, 1)) < 1) {
        ipc_client_log(client, "failed to write CRC");
        goto fail;
    }
    else {
        ipc_client_log(client, "wrote CRC %x", crc);
    }

    return 0;

fail:
    return ret;
}

static int galaxys2_send_psi(struct ipc_client *client, struct modemctl_io_data *io_data)
{
    size_t length = galaxys2_radio_parts[PSI].length;

    struct galaxys2_psi_header hdr = {
        .magic = XMM_PSI_MAGIC,
        .length = length,
        .padding = 0xff,
    };
    int ret = -1;

    if ((ret = write(io_data->boot_fd, &hdr, sizeof(hdr))) != sizeof(hdr)) {
        ipc_client_log(client, "%s: failed to write header, ret %d", __func__, ret);
        goto fail;
    }

    if ((ret = galaxys2_send_image(client, io_data, PSI)) < 0) {
        ipc_client_log(client, "Error: failed to send PSI image");
        goto fail;
    }

    int i;
    for (i = 0; i < 22; i++) {
        char ack;
        if (expect_read(io_data->boot_fd, &ack, 1) < 1) {
            ipc_client_log(client, "failed to read ACK byte %d", i);
            goto fail;
        }
    }

    if ((ret = expect_data(io_data->boot_fd, "\x1", 1)) < 0) {
        ipc_client_log(client, "failed to wait for first ACK");
        goto fail;
    }

    if ((ret = expect_data(io_data->boot_fd, "\x1", 1)) < 0) {
        ipc_client_log(client, "failed to wait for second ACK");
        goto fail;
    }

    if ((ret = expect_data(io_data->boot_fd, PSI_ACK_MAGIC, 2)) < 0) {
        ipc_client_log(client, "Error: failed to receive PSI ACK");
        goto fail;
    }
    else {
        ipc_client_log(client, "received PSI ACK");
    }

    return 0;

fail:
    return ret;
}

static int galaxys2_send_ebl(struct ipc_client *client, struct modemctl_io_data *io_data)
{
    int ret;
    int fd = io_data->boot_fd;
    unsigned length = galaxys2_radio_parts[EBL].length;

    if ((ret = write(fd, &length, sizeof(length))) < 0) {
        ipc_client_log(client, "Error: failed to write EBL length");
        goto fail;
    }

    if ((ret = expect_data(fd, EBL_HDR_ACK_MAGIC, 2)) < 0) {
        ipc_client_log(client, "Error: failed to wait for EBL header ACK");
        goto fail;
    }

    if ((ret = galaxys2_send_image(client, io_data, EBL)) < 0) {
        ipc_client_log(client, "Error: failed to send EBL image");
        goto fail;
    }

    if ((ret = expect_data(fd, EBL_IMG_ACK_MAGIC, 2)) < 0) {
        ipc_client_log(client, "Error: failed to wait for EBL image ACK");
        goto fail;
    }

    return 0;

fail:
    return ret;
}

static int galaxys2_boot_cmd(struct ipc_client *client,
    struct modemctl_io_data *io_data, enum xmm6260_boot_cmd cmd,
    void *data, size_t data_size)
{
    int ret = 0;
    char *cmd_data = 0;
    if (cmd >= ARRAY_SIZE(galaxys2_boot_cmd_desc)) {
        ipc_client_log(client, "Error: bad command %x\n", cmd);
        goto done_or_fail;
    }

    unsigned cmd_code = galaxys2_boot_cmd_desc[cmd].code;

    uint16_t magic = (data_size & 0xffff) + cmd_code;
    unsigned char *ptr = (unsigned char*)data;
    size_t i;
    for (i = 0; i < data_size; i++) {
        magic += ptr[i];
    }

    struct galaxys2_boot_cmd header = {
        .check = magic,
        .cmd = cmd_code,
        .data_size = data_size,
    };

    size_t cmd_size = galaxys2_boot_cmd_desc[cmd].data_size;
    size_t buf_size = cmd_size + sizeof(header);

    cmd_data = (char*)malloc(buf_size);
    if (!cmd_data) {
        ipc_client_log(client, "Error: failed to allocate command buffer");
        ret = -ENOMEM;
        goto done_or_fail;
    }
    memset(cmd_data, 0, buf_size);
    memcpy(cmd_data, &header, sizeof(header));
    memcpy(cmd_data + sizeof(header), data, data_size);

    if ((ret = write(io_data->boot_fd, cmd_data, buf_size)) < 0) {
        ipc_client_log(client, "Error: failed to write command to socket");
        goto done_or_fail;
    }

    if ((unsigned)ret != buf_size) {
        ipc_client_log(client, "Error: written %d bytes of %d", ret, buf_size);
        ret = -EINVAL;
        goto done_or_fail;
    }

    if (!galaxys2_boot_cmd_desc[cmd].need_ack) {
        ret = 0;
        goto done_or_fail;
    }

    struct galaxys2_boot_cmd ack = {
        .check = 0,
    };
    if ((ret = expect_read(io_data->boot_fd, &ack, sizeof(ack))) < 0) {
        ipc_client_log(client, "Error: failed to receive ack for cmd %x", header.cmd);
        goto done_or_fail;
    }

    if (ret != sizeof(ack)) {
        ipc_client_log(client, "Error: received %x bytes of %x for ack", ret, sizeof(ack));
        ret = -EINVAL;
        goto done_or_fail;
    }

    if (ack.cmd != header.cmd) {
        ipc_client_log(client, "Error: ack cmd %x does not match request %x", ack.cmd, header.cmd);
        ret = -EINVAL;
        goto done_or_fail;
    }

    if ((ret = expect_read(io_data->boot_fd, cmd_data, cmd_size)) < 0) {
        ipc_client_log(client, "Error: failed to receive reply data");
        goto done_or_fail;
    }

    if ((unsigned)ret != cmd_size) {
        ipc_client_log(client, "Error: received %x bytes of %x for reply data", ret, cmd_size);
        ret = -EINVAL;
        goto done_or_fail;
    }

done_or_fail:

    if (cmd_data) {
        free(cmd_data);
    }

    return ret;
}

static int galaxys2_boot_info_ack(struct ipc_client *client,
    struct modemctl_io_data *io_data)
{
    int ret;
    struct galaxys2_boot_info info;

    if ((ret = expect_read(io_data->boot_fd, &info, sizeof(info))) != sizeof(info)) {
        ipc_client_log(client, "Error: failed to receive Boot Info ret=%d", ret);
        ret = -1;
        goto fail;
    }
    else {
        ipc_client_log(client, "received Boot Info");
    }

    if ((ret = galaxys2_boot_cmd(client, io_data, SetPortConf, &info, sizeof(info))) < 0) {
        ipc_client_log(client, "Error: failed to send SetPortConf command");
        goto fail;
    }
    else {
        ipc_client_log(client, "sent SetPortConf command");
    }

    return 0;

fail:
    return ret;
}

static int galaxys2_send_image_data(struct ipc_client *client,
    struct modemctl_io_data *io_data, uint32_t addr,
    void *data, int data_len)
{
    int ret = 0;
    int count = 0;
    char *data_p = (char *) data;

    if ((ret = galaxys2_boot_cmd(client, io_data, ReqFlashSetAddress, &addr, 4)) < 0) {
        ipc_client_log(client, "Error: failed to send ReqFlashSetAddress");
        goto fail;
    }
    else {
        ipc_client_log(client, "sent ReqFlashSetAddress");
    }

    while (count < data_len) {
        int rest = data_len - count;
        int chunk = rest < SEC_DOWNLOAD_CHUNK ? rest : SEC_DOWNLOAD_CHUNK;

        ret = galaxys2_boot_cmd(client, io_data, ReqFlashWriteBlock, data_p, chunk);
        if (ret < 0) {
            ipc_client_log(client, "Error: failed to send data chunk");
            goto fail;
        }

        data_p += chunk;
        count += chunk;
    }

    usleep(SEC_DOWNLOAD_DELAY_US);

fail:
    return ret;
}

static int galaxys2_send_image_addr(struct ipc_client *client,
    struct modemctl_io_data *io_data, uint32_t addr, enum xmm6260_image type)
{
    uint32_t offset = galaxys2_radio_parts[type].offset;
    uint32_t length = galaxys2_radio_parts[type].length;
    char *start = io_data->radio_data + offset;
    int ret = 0;

    ret = galaxys2_send_image_data(client, io_data, addr, start, length);

    return ret;
}

static int galaxys2_send_secure_images(struct ipc_client *client,
    struct modemctl_io_data *io_data)
{
    int ret = 0;

    uint32_t sec_off = galaxys2_radio_parts[SECURE_IMAGE].offset;
    uint32_t sec_len = galaxys2_radio_parts[SECURE_IMAGE].length;
    void *sec_img = io_data->radio_data + sec_off;
    void *nv_data = NULL;

    if ((ret = galaxys2_boot_cmd(client, io_data, ReqSecStart, sec_img, sec_len)) < 0) {
        ipc_client_log(client, "Error: failed to write ReqSecStart");
        goto fail;
    }
    else {
        ipc_client_log(client, "sent ReqSecStart");
    }

    if ((ret = galaxys2_send_image_addr(client, io_data, FW_LOAD_ADDR, FIRMWARE)) < 0) {
        ipc_client_log(client, "Error: failed to send FIRMWARE image");
        goto fail;
    }
    else {
        ipc_client_log(client, "sent FIRMWARE image");
    }

    nv_data_check(client);
    nv_data_md5_check(client);

    nv_data = ipc_file_read(client, nv_data_path(client), 2 << 20, 1024);
    if (nv_data == NULL) {
        ipc_client_log(client, "Error: failed to read NVDATA image");
        goto fail;
    }

    if ((ret = galaxys2_send_image_data(client, io_data, NVDATA_LOAD_ADDR, nv_data, 2 << 20)) < 0) {
        ipc_client_log(client, "Error: failed to send NVDATA image");
        goto fail;
    }
    else {
        ipc_client_log(client, "sent NVDATA image");
    }

    free(nv_data);

    if ((ret = galaxys2_boot_cmd(client, io_data, ReqSecEnd,
        BL_END_MAGIC, BL_END_MAGIC_LEN)) < 0)
    {
        ipc_client_log(client, "Error: failed to write ReqSecEnd");
        goto fail;
    }
    else {
        ipc_client_log(client, "sent ReqSecEnd");
    }

    ret = galaxys2_boot_cmd(client, io_data, ReqForceHwReset,
        BL_RESET_MAGIC, BL_RESET_MAGIC_LEN);
    if (ret < 0) {
        ipc_client_log(client, "Error: failed to write ReqForceHwReset");
        goto fail;
    }
    else {
        ipc_client_log(client, "sent ReqForceHwReset");
    }

fail:
    return ret;
}

/*
 * i9200 (Galaxy S2) board-specific code
 */

/*
 * Power management
 */
static int galaxys2_ehci_setpower(struct ipc_client *client, bool enabled) {
    int ret = -1;

    ipc_client_log(client, "%s: enabled=%d", __func__, enabled);

    int ehci_fd = open(I9100_EHCI_PATH, O_RDWR);
    if (ehci_fd < 0) {
        ipc_client_log(client, "Error: failed to open EHCI fd");
        ret = -ENODEV;
        goto fail;
    }
    else {
        ipc_client_log(client, "opened EHCI %s: fd=%d", I9100_EHCI_PATH, ehci_fd);
    }

    ret = write(ehci_fd, enabled ? "1" : "0", 1);

    //must write exactly one byte
    if (ret <= 0) {
        ipc_client_log(client, "Error: failed to set EHCI power");
    }
    else {
        ipc_client_log(client, "set EHCI power");
    }

fail:
    if (ehci_fd >= 0) {
        close(ehci_fd);
    }

    return ret;
}

static int galaxys2_modem_reboot(struct ipc_client *client,
    struct modemctl_io_data *io_data, bool hard) {
    int ret;

    //wait for link to become ready before redetection
    if (!hard) {
        if ((ret = modemctl_wait_link_ready(client, io_data)) < 0) {
            ipc_client_log(client, "Error: failed to wait for link to get ready for redetection");
            goto fail;
        }
        else {
            ipc_client_log(client, "link ready for redetection");
        }
    }

    /*
     * Disable the hardware to ensure consistent state
     */
    if (hard) {
        if ((ret = modemctl_modem_power(client, io_data, false)) < 0) {
            ipc_client_log(client, "Error: failed to disable xmm6260 power");
            goto fail;
        }
        else {
            ipc_client_log(client, "disabled xmm6260 power");
        }
    }

    if ((ret = modemctl_link_set_enabled(client, io_data, false)) < 0) {
        ipc_client_log(client, "Error: failed to disable I9100 HSIC link");
        goto fail;
    }
    else {
        ipc_client_log(client, "disabled I9100 HSIC link");
    }

    if ((ret = galaxys2_ehci_setpower(client, false)) < 0) {
        ipc_client_log(client, "Error: failed to disable I9100 EHCI");
        goto fail;
    }
    else {
        ipc_client_log(client, "disabled I9100 EHCI");
    }

    if ((ret = modemctl_link_set_active(client, io_data, false)) < 0) {
        ipc_client_log(client, "Error: failed to deactivate I9100 HSIC link");
        goto fail;
    }
    else {
        ipc_client_log(client, "deactivated I9100 HSIC link");
    }

    /*
     * Now, initialize the hardware
     */

    if ((ret = modemctl_link_set_enabled(client, io_data, true)) < 0) {
        ipc_client_log(client, "Error: failed to enable I9100 HSIC link");
        goto fail;
    }
    else {
        ipc_client_log(client, "enabled I9100 HSIC link");
    }

    if ((ret = galaxys2_ehci_setpower(client, true)) < 0) {
        ipc_client_log(client, "Error: failed to enable I9100 EHCI");
        goto fail;
    }
    else {
        ipc_client_log(client, "enabled I9100 EHCI");
    }

    if ((ret = modemctl_link_set_active(client, io_data, true)) < 0) {
        ipc_client_log(client, "Error: failed to activate I9100 HSIC link");
        goto fail;
    }
    else {
        ipc_client_log(client, "activated I9100 HSIC link");
    }

    if (hard) {
        if ((ret = modemctl_modem_power(client, io_data, true)) < 0) {
            ipc_client_log(client, "Error: failed to enable xmm6260 power");
            goto fail;
        }
        else {
            ipc_client_log(client, "enabled xmm6260 power");
        }
    }

    if ((ret = modemctl_wait_link_ready(client, io_data)) < 0) {
        ipc_client_log(client, "Error: failed to wait for link to get ready");
        goto fail;
    }
    else {
        ipc_client_log(client, "link ready");
    }

fail:
    return ret;
}

int galaxys2_modem_bootstrap(struct ipc_client *client) {
    int ret = 0;
    struct modemctl_io_data io_data;
    memset(&io_data, 0, sizeof(client, io_data));

    io_data.radio_fd = open(RADIO_IMAGE, O_RDONLY);
    if (io_data.radio_fd < 0) {
        ipc_client_log(client, "Error: failed to open radio firmware");
        goto fail;
    }
    else {
        ipc_client_log(client, "opened radio image %s, fd=%d", RADIO_IMAGE, io_data.radio_fd);
    }

    if (fstat(io_data.radio_fd, &io_data.radio_stat) < 0) {
        ipc_client_log(client, "Error: failed to stat radio image, error %s", strerror(errno));
        goto fail;
    }

    io_data.radio_data = mmap(0, RADIO_MAP_SIZE, PROT_READ, MAP_SHARED,
        io_data.radio_fd, 0);
    if (io_data.radio_data == MAP_FAILED) {
        ipc_client_log(client, "Error: failed to mmap radio image, error %s", strerror(errno));
        goto fail;
    }

    io_data.boot_fd = open(BOOT_DEV, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (io_data.boot_fd < 0) {
        ipc_client_log(client, "Error: failed to open boot device");
        goto fail;
    }
    else {
        ipc_client_log(client, "opened boot device %s, fd=%d", BOOT_DEV, io_data.boot_fd);
    }

    io_data.link_fd = open(LINK_PM, O_RDWR);
    if (io_data.link_fd < 0) {
        ipc_client_log(client, "Error: failed to open link device");
        goto fail;
    }
    else {
        ipc_client_log(client, "opened link device %s, fd=%d", LINK_PM, io_data.link_fd);
    }

    if (galaxys2_modem_reboot(client, &io_data, true)) {
        ipc_client_log(client, "Error: failed to hard reset modem");
        goto fail;
    }
    else {
        ipc_client_log(client, "modem hard reset done");
    }

    /*
     * Now, actually load the firmware
     */
    if (write(io_data.boot_fd, "ATAT", 4) != 4) {
        ipc_client_log(client, "Error: failed to write ATAT to boot socket");
        goto fail;
    }
    else {
        ipc_client_log(client, "written ATAT to boot socket, waiting for ACK");
    }

    char buf[2];
    if (expect_read(io_data.boot_fd, buf, 1) < 0) {
        ipc_client_log(client, "Error: failed to receive bootloader ACK");
        goto fail;
    }
    if (expect_read(io_data.boot_fd, buf + 1, 1) < 0) {
        ipc_client_log(client, "Error: failed to receive chip IP ACK");
        goto fail;
    }
    ipc_client_log(client, "receive ID: [%02x %02x]", buf[0], buf[1]);

    if ((ret = galaxys2_send_psi(client, &io_data)) < 0) {
        ipc_client_log(client, "Error: failed to upload PSI");
        goto fail;
    }
    else {
        ipc_client_log(client, "PSI download complete");
    }

    if ((ret = galaxys2_send_ebl(client, &io_data)) < 0) {
        ipc_client_log(client, "Error: failed to upload EBL");
        goto fail;
    }
    else {
        ipc_client_log(client, "EBL download complete");
    }

    if ((ret = galaxys2_boot_info_ack(client, &io_data)) < 0) {
        ipc_client_log(client, "Error: failed to receive Boot Info");
        goto fail;
    }
    else {
        ipc_client_log(client, "Boot Info ACK done");
    }

    if ((ret = galaxys2_send_secure_images(client, &io_data)) < 0) {
        ipc_client_log(client, "Error: failed to upload Secure Image");
        goto fail;
    }
    else {
        ipc_client_log(client, "Secure Image download complete");
    }

    usleep(POST_BOOT_TIMEOUT_US);

    if ((ret = galaxys2_modem_reboot(client, &io_data, false))) {
        ipc_client_log(client, "Error: failed to soft reset modem");
        goto fail;
    }
    else {
        ipc_client_log(client, "modem soft reset done");
    }

    ipc_client_log(client, "Modem is online!");
    ret = 0;

fail:
    if (io_data.radio_data != MAP_FAILED) {
        munmap(io_data.radio_data, RADIO_MAP_SIZE);
    }

    if (io_data.link_fd >= 0) {
        close(io_data.link_fd);
    }

    if (io_data.radio_fd >= 0) {
        close(io_data.radio_fd);
    }

    if (io_data.boot_fd >= 0) {
        close(io_data.boot_fd);
    }

    return ret;
}

// vim:ts=4:sw=4:expandtab
