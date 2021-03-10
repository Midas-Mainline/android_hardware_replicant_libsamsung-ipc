/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2010-2011 Joerie de Gram <j.de.gram@gmail.com>
 * Copyright (C) 2013-2014 Paul Kocialkowski <contact@paulk.fr>
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

#include <stdio.h>

#include <samsung-ipc.h>

const char *ipc_request_type_string(unsigned char type)
{
	static char type_string[5] = { 0 };

	switch (type) {
	case IPC_TYPE_EXEC:
		return "IPC_TYPE_EXEC";
	case IPC_TYPE_GET:
		return "IPC_TYPE_GET";
	case IPC_TYPE_SET:
		return "IPC_TYPE_SET";
	case IPC_TYPE_CFRM:
		return "IPC_TYPE_CFRM";
	case IPC_TYPE_EVENT:
		return "IPC_TYPE_EVENT";
	default:
		snprintf((char *) &type_string, sizeof(type_string), "0x%02x",
			 type);
		return type_string;
	}
}

const char *ipc_response_type_string(unsigned char type)
{
	static char type_string[5] = { 0 };

	switch (type) {
	case IPC_TYPE_INDI:
		return "IPC_TYPE_INDI";
	case IPC_TYPE_RESP:
		return "IPC_TYPE_RESP";
	case IPC_TYPE_NOTI:
		return "IPC_TYPE_NOTI";
	default:
		snprintf((char *) &type_string, sizeof(type_string), "0x%02x",
			 type);
		return type_string;
	}
}

const char *ipc_command_string(unsigned short command)
{
	static char command_string[7] = { 0 };

	switch (command) {
	case IPC_PWR_PHONE_PWR_UP:
		return "IPC_PWR_PHONE_PWR_UP";
	case IPC_PWR_PHONE_PWR_OFF:
		return "IPC_PWR_PHONE_PWR_OFF";
	case IPC_PWR_PHONE_RESET:
		return "IPC_PWR_PHONE_RESET";
	case IPC_PWR_BATT_STATUS:
		return "IPC_PWR_BATT_STATUS";
	case IPC_PWR_BATT_TYPE:
		return "IPC_PWR_BATT_TYPE";
	case IPC_PWR_BATT_COMP:
		return "IPC_PWR_BATT_COMP";
	case IPC_PWR_PHONE_STATE:
		return "IPC_PWR_PHONE_STATE";
	case IPC_CALL_OUTGOING:
		return "IPC_CALL_OUTGOING";
	case IPC_CALL_INCOMING:
		return "IPC_CALL_INCOMING";
	case IPC_CALL_RELEASE:
		return "IPC_CALL_RELEASE";
	case IPC_CALL_ANSWER:
		return "IPC_CALL_ANSWER";
	case IPC_CALL_STATUS:
		return "IPC_CALL_STATUS";
	case IPC_CALL_LIST:
		return "IPC_CALL_LIST";
	case IPC_CALL_BURST_DTMF:
		return "IPC_CALL_BURST_DTMF";
	case IPC_CALL_CONT_DTMF:
		return "IPC_CALL_CONT_DTMF";
	case IPC_CALL_WAITING:
		return "IPC_CALL_WAITING";
	case IPC_CALL_LINE_ID:
		return "IPC_CALL_LINE_ID";
	case IPC_SMS_SEND_MSG:
		return "IPC_SMS_SEND_MSG";
	case IPC_SMS_INCOMING_MSG:
		return "IPC_SMS_INCOMING_MSG";
	case IPC_SMS_READ_MSG:
		return "IPC_SMS_READ_MSG";
	case IPC_SMS_SAVE_MSG:
		return "IPC_SMS_SAVE_MSG";
	case IPC_SMS_DEL_MSG:
		return "IPC_SMS_DEL_MSG";
	case IPC_SMS_DELIVER_REPORT:
		return "IPC_SMS_DELIVER_REPORT";
	case IPC_SMS_DEVICE_READY:
		return "IPC_SMS_DEVICE_READY";
	case IPC_SMS_SEL_MEM:
		return "IPC_SMS_SEL_MEM";
	case IPC_SMS_STORED_MSG_COUNT:
		return "IPC_SMS_STORED_MSG_COUNT";
	case IPC_SMS_SVC_CENTER_ADDR:
		return "IPC_SMS_SVC_CENTER_ADDR";
	case IPC_SMS_SVC_OPTION:
		return "IPC_SMS_SVC_OPTION";
	case IPC_SMS_MEM_STATUS:
		return "IPC_SMS_MEM_STATUS";
	case IPC_SMS_CBS_MSG:
		return "IPC_SMS_CBS_MSG";
	case IPC_SMS_CBS_CFG:
		return "IPC_SMS_CBS_CFG";
	case IPC_SMS_STORED_MSG_STATUS:
		return "IPC_SMS_STORED_MSG_STATUS";
	case IPC_SMS_PARAM_COUNT:
		return "IPC_SMS_PARAM_COUNT";
	case IPC_SMS_PARAM:
		return "IPC_SMS_PARAM";
	case IPC_SEC_PIN_STATUS:
		return "IPC_SEC_PIN_STATUS";
	case IPC_SEC_PHONE_LOCK:
		return "IPC_SEC_PHONE_LOCK";
	case IPC_SEC_CHANGE_LOCKING_PW:
		return "IPC_SEC_CHANGE_LOCKING_PW";
	case IPC_SEC_SIM_LANG:
		return "IPC_SEC_SIM_LANG";
	case IPC_SEC_RSIM_ACCESS:
		return "IPC_SEC_RSIM_ACCESS";
	case IPC_SEC_GSIM_ACCESS:
		return "IPC_SEC_GSIM_ACCESS";
	case IPC_SEC_SIM_ICC_TYPE:
		return "IPC_SEC_SIM_ICC_TYPE";
	case IPC_SEC_LOCK_INFORMATION:
		return "IPC_SEC_LOCK_INFORMATION";
	case IPC_SEC_IMS_AUTH:
		return "IPC_SEC_IMS_AUTH";
	case IPC_PB_ACCESS:
		return "IPC_PB_ACCESS";
	case IPC_PB_STORAGE:
		return "IPC_PB_STORAGE";
	case IPC_PB_STORAGE_LIST:
		return "IPC_PB_STORAGE_LIST";
	case IPC_PB_ENTRY_INFO:
		return "IPC_PB_ENTRY_INFO";
	case IPC_PB_3GPB_CAPA:
		return "IPC_PB_3GPB_CAPA";
	case IPC_DISP_ICON_INFO:
		return "IPC_DISP_ICON_INFO";
	case IPC_DISP_HOMEZONE_INFO:
		return "IPC_DISP_HOMEZONE_INFO";
	case IPC_DISP_RSSI_INFO:
		return "IPC_DISP_RSSI_INFO";
	case IPC_NET_PREF_PLMN:
		return "IPC_NET_PREF_PLMN";
	case IPC_NET_PLMN_SEL:
		return "IPC_NET_PLMN_SEL";
	case IPC_NET_SERVING_NETWORK:
		return "IPC_NET_SERVING_NETWORK";
	case IPC_NET_PLMN_LIST:
		return "IPC_NET_PLMN_LIST";
	case IPC_NET_REGIST:
		return "IPC_NET_REGIST";
	case IPC_NET_SUBSCRIBER_NUM:
		return "IPC_NET_SUBSCRIBER_NUM";
	case IPC_NET_BAND_SEL:
		return "IPC_NET_BAND_SEL";
	case IPC_NET_SERVICE_DOMAIN_CONFIG:
		return "IPC_NET_SERVICE_DOMAIN_CONFIG";
	case IPC_NET_POWERON_ATTACH:
		return "IPC_NET_POWERON_ATTACH";
	case IPC_NET_MODE_SEL:
		return "IPC_NET_MODE_SEL";
	case IPC_NET_ACQ_ORDER:
		return "IPC_NET_ACQ_ORDER";
	case IPC_NET_IDENTITY:
		return "IPC_NET_IDENTITY";
	case IPC_NET_PREFERRED_NETWORK_INFO:
		return "IPC_NET_PREFERRED_NETWORK_INFO";
	case IPC_SND_SPKR_VOLUME_CTRL:
		return "IPC_SND_SPKR_VOLUME_CTRL";
	case IPC_SND_MIC_MUTE_CTRL:
		return "IPC_SND_MIC_MUTE_CTRL";
	case IPC_SND_AUDIO_PATH_CTRL:
		return "IPC_SND_AUDIO_PATH_CTRL";
	case IPC_SND_AUDIO_SOURCE_CTRL:
		return "IPC_SND_AUDIO_SOURCE_CTRL";
	case IPC_SND_LOOPBACK_CTRL:
		return "IPC_SND_LOOPBACK_CTRL";
	case IPC_SND_VOICE_RECORDING_CTRL:
		return "IPC_SND_VOICE_RECORDING_CTRL";
	case IPC_SND_VIDEO_CALL_CTRL:
		return "IPC_SND_VIDEO_CALL_CTRL";
	case IPC_SND_RINGBACK_TONE_CTRL:
		return "IPC_SND_RINGBACK_TONE_CTRL";
	case IPC_SND_CLOCK_CTRL:
		return "IPC_SND_CLOCK_CTRL";
	case IPC_SND_WB_AMR_STATUS:
		return "IPC_SND_WB_AMR_STATUS";
	case IPC_MISC_ME_VERSION:
		return "IPC_MISC_ME_VERSION";
	case IPC_MISC_ME_IMSI:
		return "IPC_MISC_ME_IMSI";
	case IPC_MISC_ME_SN:
		return "IPC_MISC_ME_SN";
	case IPC_MISC_TIME_INFO:
		return "IPC_MISC_TIME_INFO";
	case IPC_MISC_DEBUG_LEVEL:
		return "IPC_MISC_DEBUG_LEVEL";
	case IPC_SVC_ENTER:
		return "IPC_SVC_ENTER";
	case IPC_SVC_END:
		return "IPC_SVC_END";
	case IPC_SVC_PRO_KEYCODE:
		return "IPC_SVC_PRO_KEYCODE";
	case IPC_SVC_SCREEN_CFG:
		return "IPC_SVC_SCREEN_CFG";
	case IPC_SVC_DISPLAY_SCREEN:
		return "IPC_SVC_DISPLAY_SCREEN";
	case IPC_SVC_CHANGE_SVC_MODE:
		return "IPC_SVC_CHANGE_SVC_MODE";
	case IPC_SVC_DEVICE_TEST:
		return "IPC_SVC_DEVICE_TEST";
	case IPC_SVC_DEBUG_DUMP:
		return "IPC_SVC_DEBUG_DUMP";
	case IPC_SVC_DEBUG_STRING:
		return "IPC_SVC_DEBUG_STRING";
	case IPC_SS_WAITING:
		return "IPC_SS_WAITING";
	case IPC_SS_CLI:
		return "IPC_SS_CLI";
	case IPC_SS_BARRING:
		return "IPC_SS_BARRING";
	case IPC_SS_BARRING_PW:
		return "IPC_SS_BARRING_PW";
	case IPC_SS_FORWARDING:
		return "IPC_SS_FORWARDING";
	case IPC_SS_INFO:
		return "IPC_SS_INFO";
	case IPC_SS_MANAGE_CALL:
		return "IPC_SS_MANAGE_CALL";
	case IPC_SS_USSD:
		return "IPC_SS_USSD";
	case IPC_SS_AOC:
		return "IPC_SS_AOC";
	case IPC_SS_RELEASE_COMPLETE:
		return "IPC_SS_RELEASE_COMPLETE";
	case IPC_GPRS_DEFINE_PDP_CONTEXT:
		return "IPC_GPRS_DEFINE_PDP_CONTEXT";
	case IPC_GPRS_QOS:
		return "IPC_GPRS_QOS";
	case IPC_GPRS_PS:
		return "IPC_GPRS_PS";
	case IPC_GPRS_PDP_CONTEXT:
		return "IPC_GPRS_PDP_CONTEXT";
	case IPC_GPRS_ENTER_DATA:
		return "IPC_GPRS_ENTER_DATA";
	case IPC_GPRS_SHOW_PDP_ADDR:
		return "IPC_GPRS_SHOW_PDP_ADDR";
	case IPC_GPRS_MS_CLASS:
		return "IPC_GPRS_MS_CLASS";
	case IPC_GPRS_3G_QUAL_SRVC_PROFILE:
		return "IPC_GPRS_3G_QUAL_SRVC_PROFILE";
	case IPC_GPRS_IP_CONFIGURATION:
		return "IPC_GPRS_IP_CONFIGURATION";
	case IPC_GPRS_DEFINE_SEC_PDP_CONTEXT:
		return "IPC_GPRS_DEFINE_SEC_PDP_CONTEXT";
	case IPC_GPRS_TFT:
		return "IPC_GPRS_TFT";
	case IPC_GPRS_HSDPA_STATUS:
		return "IPC_GPRS_HSDPA_STATUS";
	case IPC_GPRS_CURRENT_SESSION_DATA_COUNTER:
		return "IPC_GPRS_CURRENT_SESSION_DATA_COUNTER";
	case IPC_GPRS_DATA_DORMANT:
		return "IPC_GPRS_DATA_DORMANT";
	case IPC_GPRS_PIN_CTRL:
		return "IPC_GPRS_PIN_CTRL";
	case IPC_GPRS_CALL_STATUS:
		return "IPC_GPRS_CALL_STATUS";
	case IPC_GPRS_PORT_LIST:
		return "IPC_GPRS_PORT_LIST";
	case IPC_SAT_PROFILE_DOWNLOAD:
		return "IPC_SAT_PROFILE_DOWNLOAD";
	case IPC_SAT_ENVELOPE_CMD:
		return "IPC_SAT_ENVELOPE_CMD";
	case IPC_SAT_PROACTIVE_CMD:
		return "IPC_SAT_PROACTIVE_CMD";
	case IPC_SAT_TERMINATE_USAT_SESSION:
		return "IPC_SAT_TERMINATE_USAT_SESSION";
	case IPC_SAT_EVENT_DOWNLOAD:
		return "IPC_SAT_EVENT_DOWNLOAD";
	case IPC_SAT_PROVIDE_LOCAL_INFO:
		return "IPC_SAT_PROVIDE_LOCAL_INFO";
	case IPC_SAT_POLLING:
		return "IPC_SAT_POLLING";
	case IPC_SAT_REFRESH:
		return "IPC_SAT_REFRESH";
	case IPC_SAT_SETUP_EVENT_LIST:
		return "IPC_SAT_SETUP_EVENT_LIST";
	case IPC_SAT_CALL_CONTROL_RESULT:
		return "IPC_SAT_CALL_CONTROL_RESULT";
	case IPC_SAT_IMAGE_CLUT:
		return "IPC_SAT_IMAGE_CLUT";
	case IPC_SAT_SETUP_CALL_PROCESSING:
		return "IPC_SAT_SETUP_CALL_PROCESSING";
	case IPC_IMEI_START:
		return "IPC_IMEI_START";
	case IPC_IMEI_CHECK_DEVICE_INFO:
		return "IPC_IMEI_CHECK_DEVICE_INFO";
	case IPC_RFS_NV_READ_ITEM:
		return "IPC_RFS_NV_READ_ITEM";
	case IPC_RFS_NV_WRITE_ITEM:
		return "IPC_RFS_NV_WRITE_ITEM";
	case IPC_GEN_PHONE_RES:
		return "IPC_GEN_PHONE_RES";
	default:
		snprintf((char *) &command_string, sizeof(command_string),
			 "0x%04x", command);
		return command_string;
	}
}

const char *ipc_group_string(unsigned char group)
{
	static char group_string[5] = { 0 };

	switch (group) {
	case IPC_GROUP_PWR:
		return "IPC_GROUP_PWR";
	case IPC_GROUP_CALL:
		return "IPC_GROUP_CALL";
	case IPC_GROUP_SMS:
		return "IPC_GROUP_SMS";
	case IPC_GROUP_SEC:
		return "IPC_GROUP_SEC";
	case IPC_GROUP_PB:
		return "IPC_GROUP_PB";
	case IPC_GROUP_DISP:
		return "IPC_GROUP_DISP";
	case IPC_GROUP_NET:
		return "IPC_GROUP_NET";
	case IPC_GROUP_SND:
		return "IPC_GROUP_SND";
	case IPC_GROUP_MISC:
		return "IPC_GROUP_MISC";
	case IPC_GROUP_SVC:
		return "IPC_GROUP_SVC";
	case IPC_GROUP_SS:
		return "IPC_GROUP_SS";
	case IPC_GROUP_GPRS:
		return "IPC_GROUP_GPRS";
	case IPC_GROUP_SAT:
		return "IPC_GROUP_SAT";
	case IPC_GROUP_CFG:
		return "IPC_GROUP_CFG";
	case IPC_GROUP_IMEI:
		return "IPC_GROUP_IMEI";
	case IPC_GROUP_GPS:
		return "IPC_GROUP_GPS";
	case IPC_GROUP_SAP:
		return "IPC_GROUP_SAP";
	case IPC_GROUP_RFS:
		return "IPC_GROUP_RFS";
	case IPC_GROUP_GEN:
		return "IPC_GROUP_GEN";
	default:
		snprintf((char *) &group_string, sizeof(group_string), "0x%02x",
			 (unsigned int)group_string);
		return group_string;
	}
}

const char *ipc_client_type_string(unsigned char client_type)
{
	static char client_type_string[5] = { 0 };

	switch (client_type) {
	case IPC_CLIENT_TYPE_FMT:
		return "IPC_CLIENT_TYPE_FMT";
	case IPC_CLIENT_TYPE_RFS:
		return "IPC_CLIENT_TYPE_RFS";
	case IPC_CLIENT_TYPE_DUMMY:
		return "IPC_CLIENT_TYPE_DUMMY";
	default:
		snprintf((char *) &client_type_string, sizeof(client_type_string), "0x%02x",
			 (unsigned int)client_type_string);
		return client_type_string;
	}
}
