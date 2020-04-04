/*
 * Copyright (C) 2010 Google, Inc.
 * Copyright (C) 2010 Samsung Electronics.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include <linux/types.h>

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t

#ifndef __MODEM_PRJ_H__
#define __MODEM_PRJ_H__

#define MAX_CPINFO_SIZE		512

#define MAX_LINK_DEVTYPE		3

#define MAX_FMT_DEVS			10
#define MAX_RAW_DEVS			32
#define MAX_RFS_DEVS			10
#define MAX_NUM_IO_DEV			(MAX_FMT_DEVS + MAX_RAW_DEVS + \
					 MAX_RFS_DEVS)

#define IOCTL_MODEM_ON			_IO('o', 0x19)
#define IOCTL_MODEM_OFF		_IO('o', 0x20)
#define IOCTL_MODEM_RESET		_IO('o', 0x21)
#define IOCTL_MODEM_BOOT_ON		_IO('o', 0x22)
#define IOCTL_MODEM_BOOT_OFF		_IO('o', 0x23)
#define IOCTL_MODEM_START		_IO('o', 0x24)

#define IOCTL_MODEM_PROTOCOL_SUSPEND	_IO('o', 0x25)
#define IOCTL_MODEM_PROTOCOL_RESUME	_IO('o', 0x26)

#define IOCTL_MODEM_STATUS		_IO('o', 0x27)
#define IOCTL_MODEM_DL_START		_IO('o', 0x28)
#define IOCTL_MODEM_FW_UPDATE		_IO('o', 0x29)

#define IOCTL_MODEM_NET_SUSPEND	_IO('o', 0x30)
#define IOCTL_MODEM_NET_RESUME		_IO('o', 0x31)

#define IOCTL_MODEM_DUMP_START		_IO('o', 0x32)
#define IOCTL_MODEM_DUMP_UPDATE	_IO('o', 0x33)
#define IOCTL_MODEM_FORCE_CRASH_EXIT	_IO('o', 0x34)
#define IOCTL_MODEM_CP_UPLOAD		_IO('o', 0x35)
#define IOCTL_MODEM_DUMP_RESET		_IO('o', 0x36)

#define IOCTL_DPRAM_SEND_BOOT		_IO('o', 0x40)
#define IOCTL_DPRAM_INIT_STATUS	_IO('o', 0x43)

/* ioctl command definitions. */
#define IOCTL_DPRAM_PHONE_POWON	_IO('o', 0xd0)
#define IOCTL_DPRAM_PHONEIMG_LOAD	_IO('o', 0xd1)
#define IOCTL_DPRAM_NVDATA_LOAD	_IO('o', 0xd2)
#define IOCTL_DPRAM_PHONE_BOOTSTART	_IO('o', 0xd3)

#define IOCTL_DPRAM_PHONE_UPLOAD_STEP1	_IO('o', 0xde)
#define IOCTL_DPRAM_PHONE_UPLOAD_STEP2	_IO('o', 0xdf)

/* modem status */
#define MODEM_OFF			0
#define MODEM_CRASHED			1
#define MODEM_RAMDUMP			2
#define MODEM_POWER_ON			3
#define MODEM_BOOTING_NORMAL		4
#define MODEM_BOOTING_RAMDUMP		5
#define MODEM_DUMPING			6
#define MODEM_RUNNING			7

#define HDLC_HEADER_MAX_SIZE		6 /* fmt 3, raw 6, rfs 6 */

#define PSD_DATA_CHID_BEGIN		0x2A
#define PSD_DATA_CHID_END		0x38

#define PS_DATA_CH_0			10
#define PS_DATA_CH_LAST		24

#define IP6VERSION			6

#define SOURCE_MAC_ADDR		{0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC}

/* Debugging features */
#define MAX_MIF_LOG_PATH_LEN		128
#define MAX_MIF_LOG_FILE_SIZE		0x800000	/* 8 MB */

#define MAX_MIF_EVT_BUFF_SIZE		256
#define MAX_MIF_TIME_LEN		32
#define MAX_MIF_NAME_LEN		16
#define MAX_MIF_STR_LEN		127
#define MAX_MIF_LOG_LEN		128

enum mif_event_id {
	MIF_IRQ_EVT = 0,
	MIF_LNK_RX_EVT,
	MIF_MUX_RX_EVT,
	MIF_IOD_RX_EVT,
	MIF_IOD_TX_EVT,
	MIF_MUX_TX_EVT,
	MIF_LNK_TX_EVT,
	MAX_MIF_EVT
};

struct dpram_queue_status {
	unsigned int in;
	unsigned int out;
};

struct dpram_queue_status_pair {
	struct dpram_queue_status txq;
	struct dpram_queue_status rxq;
};

struct dpram_irq_buff {
	unsigned int magic;
	unsigned int access;
	struct dpram_queue_status_pair qsp[MAX_IPC_DEV];
	unsigned int int2ap;
	unsigned int int2cp;
};

struct mif_event_buff {
	char time[MAX_MIF_TIME_LEN];

	struct timeval tv;
	enum mif_event_id evt;

	char mc[MAX_MIF_NAME_LEN];

	char iod[MAX_MIF_NAME_LEN];

	char ld[MAX_MIF_NAME_LEN];
	enum modem_link link_type;

	unsigned int rcvd;
	unsigned int len;
	union {
		u8 data[MAX_MIF_LOG_LEN];
		struct dpram_irq_buff dpram_irqb;
	};
};

#define MIF_LOG_DIR			"/sdcard"
#define MIF_LOG_LV_FILE		"/data/.mif_log_level"

/* Does modem ctl structure will use state ? or status defined below ?*/
enum modem_state {
	STATE_OFFLINE,
	STATE_CRASH_RESET, /* silent reset */
	STATE_CRASH_EXIT, /* cp ramdump */
	STATE_BOOTING,
	STATE_ONLINE,
	STATE_NV_REBUILDING, /* <= rebuilding start */
	STATE_LOADER_DONE,
	STATE_SIM_ATTACH,
	STATE_SIM_DETACH,
};

enum com_state {
	COM_NONE,
	COM_ONLINE,
	COM_HANDSHAKE,
	COM_BOOT,
	COM_CRASH,
};

enum link_mode {
	LINK_MODE_INVALID = 0,
	LINK_MODE_IPC,
	LINK_MODE_BOOT,
	LINK_MODE_DLOAD,
	LINK_MODE_ULOAD,
};

struct sim_state {
	bool online;			/* SIM is online? */
	bool changed;			/* online is changed? */
};

#define HDLC_START			0x7F
#define HDLC_END			0x7E
#define SIZE_OF_HDLC_START		1
#define SIZE_OF_HDLC_END		1
#define MAX_LINK_PADDING_SIZE		3

struct header_data {
	char hdr[HDLC_HEADER_MAX_SIZE];
	unsigned int len;
	unsigned int frag_len;
	char start;			/*hdlc start header 0x7F*/
};

struct fmt_hdr {
	u16 len;
	u8 control;
} __attribute__((packed));

struct raw_hdr {
	u32 len;
	u8 channel;
	u8 control;
} __attribute__((packed));

struct rfs_hdr {
	u32 len;
	u8 cmd;
	u8 id;
} __attribute__((packed));

struct sipc_fmt_hdr {
	u16 len;
	u8  msg_seq;
	u8  ack_seq;
	u8  main_cmd;
	u8  sub_cmd;
	u8  cmd_type;
} __attribute__((packed));

#endif /* __MODEM_PRJ_H__ */
