/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2011 Paul Kocialkowski <contact@oaulk.fr>
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

#ifndef __SAMSUNG_IPC_SND_H__
#define __SAMSUNG_IPC_SND_H__

/*
 * Commands
 */

#define IPC_SND_SPKR_VOLUME_CTRL				0x0901
#define IPC_SND_MIC_MUTE_CTRL					0x0902
#define IPC_SND_AUDIO_PATH_CTRL				0x0903
#define IPC_SND_AUDIO_SOURCE_CTRL				0x0904
#define IPC_SND_LOOPBACK_CTRL					0x0905
#define IPC_SND_VOICE_RECORDING_CTRL				0x0906
#define IPC_SND_VIDEO_CALL_CTRL				0x0907
#define IPC_SND_RINGBACK_TONE_CTRL				0x0908
#define IPC_SND_CLOCK_CTRL					0x0909
#define IPC_SND_WB_AMR_STATUS					0x090A

/*
 * Values
 */

#define IPC_SND_VOLUME_TYPE_VOICE				0x01
#define IPC_SND_VOLUME_TYPE_SPEAKER				0x11
#define IPC_SND_VOLUME_TYPE_HEADSET				0x31
#define IPC_SND_VOLUME_TYPE_BTVOICE				0x41

#define IPC_SND_AUDIO_PATH_HANDSET				0x01
#define IPC_SND_AUDIO_PATH_HEADSET				0x02
#define IPC_SND_AUDIO_PATH_SPEAKER				0x06
#define IPC_SND_AUDIO_PATH_BLUETOOTH				0x04
#define IPC_SND_AUDIO_PATH_BLUETOOTH_NO_NR			0x08
#define IPC_SND_AUDIO_PATH_HEADPHONE				0x07

/*
 * Structures
 */

struct ipc_snd_spkr_volume_ctrl_data {
	unsigned char type;			/* IPC_SND_VOLUME_TYPE */
	unsigned char volume;
} __attribute__((__packed__));

struct ipc_snd_mic_mute_ctrl_data {
	unsigned char mute;
} __attribute__((__packed__));

struct ipc_snd_audio_path_ctrl_data {
	unsigned char path;			/* IPC_SND_AUDIO_PATH */
} __attribute__((__packed__));

struct ipc_snd_clock_ctrl_data {
	unsigned char sync;
} __attribute__((__packed__));

#endif /* __SAMSUNG_IPC_SND_H__ */
