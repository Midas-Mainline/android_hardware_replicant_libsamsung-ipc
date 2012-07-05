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

#include "modemctl_common.h"
#include "ipc_private.h"
#include "fwloader_i9100.h"

/*
 * Locations of the firmware components in the Samsung firmware
 */

struct xmm6260_radio_part i9100_radio_parts[] = {
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

struct i9100_boot_cmd_desc i9100_boot_cmd_desc[] = {
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

static int send_image(struct ipc_client *client,
    struct modemctl_io_data *io_data, enum xmm6260_image type) {
    int ret = -1;

    if (type >= io_data->radio_parts_count) {
        _e("bad image type %x", type);
        goto fail;
    }

    size_t length = io_data->radio_parts[type].length;
    size_t offset = io_data->radio_parts[type].offset;

    size_t start = offset;
    size_t end = length + start;

    //dump some image bytes
    _d("image start");
//    hexdump(io_data->radio_data + start, length);

    while (start < end) {
        ret = write(io_data->boot_fd, io_data->radio_data + start, end - start);
        if (ret < 0) {
            _d("failed to write image chunk");
            goto fail;
        }
        start += ret;
    }

    unsigned char crc = calculateCRC(io_data->radio_data, offset, length);

    if ((ret = write(io_data->boot_fd, &crc, 1)) < 1) {
        _d("failed to write CRC");
        goto fail;
    }
    else {
        _d("wrote CRC %x", crc);
    }

    return 0;

fail:
    return ret;
}

static int send_PSI(struct ipc_client *client, struct modemctl_io_data *io_data) {
    size_t length = i9100_radio_parts[PSI].length;

    struct i9100_psi_header hdr = {
        .magic = XMM_PSI_MAGIC,
        .length = length,
        .padding = 0xff,
    };
    int ret = -1;

    if ((ret = write(io_data->boot_fd, &hdr, sizeof(hdr))) != sizeof(hdr)) {
        _d("%s: failed to write header, ret %d", __func__, ret);
        goto fail;
    }

    if ((ret = send_image(client, io_data, PSI)) < 0) {
        _e("failed to send PSI image");
        goto fail;
    }

    int i;
    for (i = 0; i < 22; i++) {
        char ack;
        if (receive(io_data->boot_fd, &ack, 1) < 1) {
            _d("failed to read ACK byte %d", i);
            goto fail;
        }
        _d("%02x ", ack);
    }

    if ((ret = expect_data(io_data->boot_fd, "\x1", 1)) < 0) {
        _d("failed to wait for first ACK");
        goto fail;
    }

    if ((ret = expect_data(io_data->boot_fd, "\x1", 1)) < 0) {
        _d("failed to wait for second ACK");
        goto fail;
    }

    if ((ret = expect_data(io_data->boot_fd, PSI_ACK_MAGIC, 2)) < 0) {
        _e("failed to receive PSI ACK");
        goto fail;
    }
    else {
        _d("received PSI ACK");
    }

    return 0;

fail:
    return ret;
}

static int send_EBL(struct ipc_client *client, struct modemctl_io_data *io_data) {
    int ret;
    int fd = io_data->boot_fd;
    unsigned length = i9100_radio_parts[EBL].length;

    if ((ret = write(fd, &length, sizeof(length))) < 0) {
        _e("failed to write EBL length");
        goto fail;
    }

    if ((ret = expect_data(fd, EBL_HDR_ACK_MAGIC, 2)) < 0) {
        _e("failed to wait for EBL header ACK");
        goto fail;
    }

    if ((ret = send_image(client, io_data, EBL)) < 0) {
        _e("failed to send EBL image");
        goto fail;
    }

    if ((ret = expect_data(fd, EBL_IMG_ACK_MAGIC, 2)) < 0) {
        _e("failed to wait for EBL image ACK");
        goto fail;
    }

    return 0;

fail:
    return ret;
}

static int bootloader_cmd(struct ipc_client *client,
    struct modemctl_io_data *io_data, enum xmm6260_boot_cmd cmd,
    void *data, size_t data_size)
{
    int ret = 0;
    char *cmd_data = 0;
    if (cmd >= ARRAY_SIZE(i9100_boot_cmd_desc)) {
        _e("bad command %x\n", cmd);
        goto done_or_fail;
    }

    unsigned cmd_code = i9100_boot_cmd_desc[cmd].code;

    uint16_t magic = (data_size & 0xffff) + cmd_code;
    unsigned char *ptr = (unsigned char*)data;
    size_t i;
    for (i = 0; i < data_size; i++) {
        magic += ptr[i];
    }

    struct i9100_boot_cmd header = {
        .check = magic,
        .cmd = cmd_code,
        .data_size = data_size,
    };

    size_t cmd_size = i9100_boot_cmd_desc[cmd].data_size;
    size_t buf_size = cmd_size + sizeof(header);

    cmd_data = (char*)malloc(buf_size);
    if (!cmd_data) {
        _e("failed to allocate command buffer");
        ret = -ENOMEM;
        goto done_or_fail;
    }
    memset(cmd_data, 0, buf_size);
    memcpy(cmd_data, &header, sizeof(header));
    memcpy(cmd_data + sizeof(header), data, data_size);

    _d("bootloader cmd packet");
    hexdump(cmd_data, buf_size);

    if ((ret = write(io_data->boot_fd, cmd_data, buf_size)) < 0) {
        _e("failed to write command to socket");
        goto done_or_fail;
    }

    if ((unsigned)ret != buf_size) {
        _e("written %d bytes of %d", ret, buf_size);
        ret = -EINVAL;
        goto done_or_fail;
    }

    _d("sent command %x magic=%x", header.cmd, header.check);

    if (!i9100_boot_cmd_desc[cmd].need_ack) {
        ret = 0;
        goto done_or_fail;
    }

    struct i9100_boot_cmd ack = {
        .check = 0,
    };
    if ((ret = receive(io_data->boot_fd, &ack, sizeof(ack))) < 0) {
        _e("failed to receive ack for cmd %x", header.cmd);
        goto done_or_fail;
    }

    if (ret != sizeof(ack)) {
        _e("received %x bytes of %x for ack", ret, sizeof(ack));
        ret = -EINVAL;
        goto done_or_fail;
    }

    hexdump(&ack, sizeof(ack));

    if (ack.cmd != header.cmd) {
        _e("ack cmd %x does not match request %x", ack.cmd, header.cmd);
        ret = -EINVAL;
        goto done_or_fail;
    }

    if ((ret = receive(io_data->boot_fd, cmd_data, cmd_size)) < 0) {
        _e("failed to receive reply data");
        goto done_or_fail;
    }

    if ((unsigned)ret != cmd_size) {
        _e("received %x bytes of %x for reply data", ret, cmd_size);
        ret = -EINVAL;
        goto done_or_fail;
    }
    hexdump(cmd_data, cmd_size);

done_or_fail:

    if (cmd_data) {
        free(cmd_data);
    }

    return ret;
}

static int ack_BootInfo(struct ipc_client *client,
    struct modemctl_io_data *io_data) {
    int ret;
    struct i9100_boot_info info;

    if ((ret = receive(io_data->boot_fd, &info, sizeof(info))) != sizeof(info)) {
        _e("failed to receive Boot Info ret=%d", ret);
        ret = -1;
        goto fail;
    }
    else {
        _d("received Boot Info");
        hexdump(&info, sizeof(info));
    }

    if ((ret = bootloader_cmd(client, io_data, SetPortConf, &info, sizeof(info))) < 0) {
        _e("failed to send SetPortConf command");
        goto fail;
    }
    else {
        _d("sent SetPortConf command");
    }

    return 0;

fail:
    return ret;
}

static int send_image_data(struct ipc_client *client,
    struct modemctl_io_data *io_data, uint32_t addr,
    void *data, int data_len)
{
    int ret = 0;
    int count = 0;
    char *data_p = (char *) data;

    if ((ret = bootloader_cmd(client, io_data, ReqFlashSetAddress, &addr, 4)) < 0) {
        _e("failed to send ReqFlashSetAddress");
        goto fail;
    }
    else {
        _d("sent ReqFlashSetAddress");
    }

    while (count < data_len) {
        int rest = data_len - count;
        int chunk = rest < SEC_DOWNLOAD_CHUNK ? rest : SEC_DOWNLOAD_CHUNK;

        ret = bootloader_cmd(client, io_data, ReqFlashWriteBlock, data_p, chunk);
        if (ret < 0) {
            _e("failed to send data chunk");
            goto fail;
        }

        data_p += chunk;
        count += chunk;
    }

    usleep(SEC_DOWNLOAD_DELAY_US);

fail:
    return ret;
}

static int send_image_addr(struct ipc_client *client,
    struct modemctl_io_data *io_data, uint32_t addr, enum xmm6260_image type)
{
    uint32_t offset = i9100_radio_parts[type].offset;
    uint32_t length = i9100_radio_parts[type].length;
    char *start = io_data->radio_data + offset;
    int ret = 0;

    ret = send_image_data(client, io_data, addr, start, length);

    return ret;
}

static int send_SecureImage(struct ipc_client *client,
    struct modemctl_io_data *io_data) {
    int ret = 0;

    uint32_t sec_off = i9100_radio_parts[SECURE_IMAGE].offset;
    uint32_t sec_len = i9100_radio_parts[SECURE_IMAGE].length;
    void *sec_img = io_data->radio_data + sec_off;
    void *nv_data = NULL;

    if ((ret = bootloader_cmd(client, io_data, ReqSecStart, sec_img, sec_len)) < 0) {
        _e("failed to write ReqSecStart");
        goto fail;
    }
    else {
        _d("sent ReqSecStart");
    }

    if ((ret = send_image_addr(client, io_data, FW_LOAD_ADDR, FIRMWARE)) < 0) {
        _e("failed to send FIRMWARE image");
        goto fail;
    }
    else {
        _d("sent FIRMWARE image");
    }

    nv_data_check(client);
    nv_data_md5_check(client);

    nv_data = ipc_file_read(client, nv_data_path(client), 2 << 20, 1024);
    if (nv_data == NULL) {
        _e("failed to read NVDATA image");
        goto fail;
    }

    if ((ret = send_image_data(client, io_data, NVDATA_LOAD_ADDR, nv_data, 2 << 20)) < 0) {
        _e("failed to send NVDATA image");
        goto fail;
    }
    else {
        _d("sent NVDATA image");
    }

    free(nv_data);

    if ((ret = bootloader_cmd(client, io_data, ReqSecEnd,
        BL_END_MAGIC, BL_END_MAGIC_LEN)) < 0)
    {
        _e("failed to write ReqSecEnd");
        goto fail;
    }
    else {
        _d("sent ReqSecEnd");
    }

    ret = bootloader_cmd(client, io_data, ReqForceHwReset,
        BL_RESET_MAGIC, BL_RESET_MAGIC_LEN);
    if (ret < 0) {
        _e("failed to write ReqForceHwReset");
        goto fail;
    }
    else {
        _d("sent ReqForceHwReset");
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
static int i9100_ehci_setpower(struct ipc_client *client, bool enabled) {
    int ret = -1;

    _d("%s: enabled=%d", __func__, enabled);

    int ehci_fd = open(I9100_EHCI_PATH, O_RDWR);
    if (ehci_fd < 0) {
        _e("failed to open EHCI fd");
        ret = -ENODEV;
        goto fail;
    }
    else {
        _d("opened EHCI %s: fd=%d", I9100_EHCI_PATH, ehci_fd);
    }

    ret = write(ehci_fd, enabled ? "1" : "0", 1);

    //must write exactly one byte
    if (ret <= 0) {
        _e("failed to set EHCI power");
    }
    else {
        _d("set EHCI power");
    }

fail:
    if (ehci_fd >= 0) {
        close(ehci_fd);
    }

    return ret;
}

static int reboot_modem_i9100(struct ipc_client *client,
    struct modemctl_io_data *io_data, bool hard) {
    int ret;

    //wait for link to become ready before redetection
    if (!hard) {
        if ((ret = modemctl_wait_link_ready(client, io_data)) < 0) {
            _e("failed to wait for link to get ready for redetection");
            goto fail;
        }
        else {
            _d("link ready for redetection");
        }
    }

    /*
     * Disable the hardware to ensure consistent state
     */
    if (hard) {
        if ((ret = modemctl_modem_power(client, io_data, false)) < 0) {
            _e("failed to disable xmm6260 power");
            goto fail;
        }
        else {
            _d("disabled xmm6260 power");
        }
    }

    if ((ret = modemctl_link_set_enabled(client, io_data, false)) < 0) {
        _e("failed to disable I9100 HSIC link");
        goto fail;
    }
    else {
        _d("disabled I9100 HSIC link");
    }

    if ((ret = i9100_ehci_setpower(client, false)) < 0) {
        _e("failed to disable I9100 EHCI");
        goto fail;
    }
    else {
        _d("disabled I9100 EHCI");
    }

    if ((ret = modemctl_link_set_active(client, io_data, false)) < 0) {
        _e("failed to deactivate I9100 HSIC link");
        goto fail;
    }
    else {
        _d("deactivated I9100 HSIC link");
    }

    /*
     * Now, initialize the hardware
     */

    if ((ret = modemctl_link_set_enabled(client, io_data, true)) < 0) {
        _e("failed to enable I9100 HSIC link");
        goto fail;
    }
    else {
        _d("enabled I9100 HSIC link");
    }

    if ((ret = i9100_ehci_setpower(client, true)) < 0) {
        _e("failed to enable I9100 EHCI");
        goto fail;
    }
    else {
        _d("enabled I9100 EHCI");
    }

    if ((ret = modemctl_link_set_active(client, io_data, true)) < 0) {
        _e("failed to activate I9100 HSIC link");
        goto fail;
    }
    else {
        _d("activated I9100 HSIC link");
    }

    if (hard) {
        if ((ret = modemctl_modem_power(client, io_data, true)) < 0) {
            _e("failed to enable xmm6260 power");
            goto fail;
        }
        else {
            _d("enabled xmm6260 power");
        }
    }

    if ((ret = modemctl_wait_link_ready(client, io_data)) < 0) {
        _e("failed to wait for link to get ready");
        goto fail;
    }
    else {
        _d("link ready");
    }

fail:
    return ret;
}

int boot_modem_i9100(struct ipc_client *client) {
    int ret = 0;
    struct modemctl_io_data io_data;
    memset(&io_data, 0, sizeof(client, io_data));

    io_data.radio_parts = i9100_radio_parts;
    io_data.radio_parts_count = ARRAY_SIZE(i9100_radio_parts);

    io_data.radio_fd = open(RADIO_IMAGE, O_RDONLY);
    if (io_data.radio_fd < 0) {
        _e("failed to open radio firmware");
        goto fail;
    }
    else {
        _d("opened radio image %s, fd=%d", RADIO_IMAGE, io_data.radio_fd);
    }

    if (fstat(io_data.radio_fd, &io_data.radio_stat) < 0) {
        _e("failed to stat radio image, error %s", strerror(errno));
        goto fail;
    }

    io_data.radio_data = mmap(0, RADIO_MAP_SIZE, PROT_READ, MAP_SHARED,
        io_data.radio_fd, 0);
    if (io_data.radio_data == MAP_FAILED) {
        _e("failed to mmap radio image, error %s", strerror(errno));
        goto fail;
    }

    io_data.boot_fd = open(BOOT_DEV, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (io_data.boot_fd < 0) {
        _e("failed to open boot device");
        goto fail;
    }
    else {
        _d("opened boot device %s, fd=%d", BOOT_DEV, io_data.boot_fd);
    }

    io_data.link_fd = open(LINK_PM, O_RDWR);
    if (io_data.link_fd < 0) {
        _e("failed to open link device");
        goto fail;
    }
    else {
        _d("opened link device %s, fd=%d", LINK_PM, io_data.link_fd);
    }

    if (reboot_modem_i9100(client, &io_data, true)) {
        _e("failed to hard reset modem");
        goto fail;
    }
    else {
        _d("modem hard reset done");
    }

    /*
     * Now, actually load the firmware
     */
    if (write(io_data.boot_fd, "ATAT", 4) != 4) {
        _e("failed to write ATAT to boot socket");
        goto fail;
    }
    else {
        _d("written ATAT to boot socket, waiting for ACK");
    }

    char buf[2];
    if (receive(io_data.boot_fd, buf, 1) < 0) {
        _e("failed to receive bootloader ACK");
        goto fail;
    }
    if (receive(io_data.boot_fd, buf + 1, 1) < 0) {
        _e("failed to receive chip IP ACK");
        goto fail;
    }
    _i("receive ID: [%02x %02x]", buf[0], buf[1]);

    if ((ret = send_PSI(client, &io_data)) < 0) {
        _e("failed to upload PSI");
        goto fail;
    }
    else {
        _d("PSI download complete");
    }

    if ((ret = send_EBL(client, &io_data)) < 0) {
        _e("failed to upload EBL");
        goto fail;
    }
    else {
        _d("EBL download complete");
    }

    if ((ret = ack_BootInfo(client, &io_data)) < 0) {
        _e("failed to receive Boot Info");
        goto fail;
    }
    else {
        _d("Boot Info ACK done");
    }

    if ((ret = send_SecureImage(client, &io_data)) < 0) {
        _e("failed to upload Secure Image");
        goto fail;
    }
    else {
        _d("Secure Image download complete");
    }

    usleep(POST_BOOT_TIMEOUT_US);

    if ((ret = reboot_modem_i9100(client, &io_data, false))) {
        _e("failed to soft reset modem");
        goto fail;
    }
    else {
        _d("modem soft reset done");
    }

    _i("online");
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
