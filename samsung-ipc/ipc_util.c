/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2010-2011 Joerie de Gram <j.de.gram@gmail.com>
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

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <asm/types.h>
#include <mtd/mtd-abi.h>

#include <samsung-ipc.h>

#include "ipc.h"

/* Log utils */
const char *ipc_response_type_to_str(int type)
{
    switch (type) {
        case IPC_TYPE_INDI:
            return "INDI";
        case IPC_TYPE_RESP:
            return "RESP";
        case IPC_TYPE_NOTI:
            return "NOTI";
        default:
            return "UNKNOWN";
    }
}

const char *ipc_request_type_to_str(int type)
{
    switch (type) {
        case IPC_TYPE_EXEC:
            return "EXEC";
        case IPC_TYPE_GET:
            return "GET";
        case IPC_TYPE_SET:
            return "SET";
        case IPC_TYPE_CFRM:
            return "CFRM";
        case IPC_TYPE_EVENT:
            return "EVENT";
        default:
            return "UNKNOWN";
    }
}

const char *ipc_command_to_str(int command)
{
    switch (command) {
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
        case IPC_DISP_ICON_INFO:
            return "IPC_DISP_ICON_INFO";
        case IPC_DISP_HOMEZONE_INFO:
            return "IPC_DISP_HOMEZONE_INFO";
        case IPC_DISP_RSSI_INFO:
            return "IPC_DISP_RSSI_INFO";
        case IPC_GEN_PHONE_RES:
            return "IPC_GEN_PHONE_RES";
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
        case IPC_GPRS_3G_QUAL_SERVICE_PROFILE:
            return "IPC_GPRS_3G_QUAL_SERVICE_PROFILE";
        case IPC_GPRS_IP_CONFIGURATION:
            return "IPC_GPRS_IP_CONFIGURATION";
        case IPC_GPRS_DEFINE_SEC_PDP_CONTEXT:
            return "IPC_GPRS_DEFINE_SEC_PDP_CONTEXT";
        case IPC_GPRS_TFT:
            return "IPC_GPRS_TFT";
        case IPC_GPRS_HSDPA_STATUS:
            return "IPC_GPRS_HSDPA_STATUS";
        case IPC_GPRS_CURRENT_SESSION_DATA_COUNT:
            return "IPC_GPRS_CURRENT_SESSION_DATA_COUNT";
        case IPC_GPRS_DATA_DORMANT:
            return "IPC_GPRS_DATA_DORMANT";
        case IPC_GPRS_DUN_PIN_CTRL:
            return "IPC_GPRS_DUN_PIN_CTRL";
        case IPC_GPRS_CALL_STATUS:
            return "IPC_GPRS_CALL_STATUS";
        case IPC_GPRS_PORT_LIST:
            return "IPC_GPRS_PORT_LIST";
        case IPC_IMEI_START:
            return "IPC_IMEI_START";
        case IPC_IMEI_CHECK_DEVICE_INFO:
            return "IPC_IMEI_CHECK_DEVICE_INFO";
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
        case IPC_NET_PREF_PLMN:
            return "IPC_NET_PREF_PLMN";
        case IPC_NET_PLMN_SEL:
            return "IPC_NET_PLMN_SEL";
        case IPC_NET_CURRENT_PLMN:
            return "IPC_NET_CURRENT_PLMN";
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
        case IPC_NET_CURRENT_RRC_STATUS:
            return "IPC_NET_CURRENT_RRC_STATUS";
        case IPC_PB_ACCESS:
            return "IPC_PB_ACCESS";
        case IPC_PB_STORAGE:
            return "IPC_PB_STORAGE";
        case IPC_PB_STORAGE_LIST:
            return "IPC_PB_STORAGE_LIST";
        case IPC_PB_ENTRY_INFO:
            return "IPC_PB_ENTRY_INFO";
        case IPC_PB_CAPABILITY_INFO:
            return "IPC_PB_CAPABILITY_INFO";
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
        case IPC_RFS_NV_READ_ITEM:
            return "IPC_RFS_NV_READ_ITEM";
        case IPC_RFS_NV_WRITE_ITEM:
            return "IPC_RFS_NV_WRITE_ITEM";
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
        case IPC_SAT_CALL_PROCESSING:
            return "IPC_SAT_CALL_PROCESSING";
        case IPC_SEC_SIM_STATUS:
            return "IPC_SEC_SIM_STATUS";
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
        case IPC_SEC_LOCK_INFO:
            return "IPC_SEC_LOCK_INFO";
        case IPC_SEC_ISIM_AUTH:
            return "IPC_SEC_ISIM_AUTH";
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
        case IPC_SMS_CBS_CONFIG:
            return "IPC_SMS_CBS_CONFIG";
        case IPC_SMS_STORED_MSG_STATUS:
            return "IPC_SMS_STORED_MSG_STATUS";
        case IPC_SMS_PARAM_COUNT:
            return "IPC_SMS_PARAM_COUNT";
        case IPC_SMS_PARAM:
            return "IPC_SMS_PARAM";
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
        default:
            return "UNKNOWN";
    }
}

void ipc_client_hex_dump(struct ipc_client *client, void *data, int size)
{
    /* dumps size bytes of *data to stdout. Looks like:
     * [0000] 75 6E 6B 6E 6F 77 6E 20
     *                  30 FF 00 00 00 00 39 00 unknown 0.....9.
     * (in a single line of course)
     */

    unsigned char *p = data;
    unsigned char c;
    int n;
    char bytestr[4] = {0};
    char addrstr[10] = {0};
    char hexstr[ 16*3 + 5] = {0};
    char charstr[16*1 + 5] = {0};
    for (n=1;n<=size;n++) {
        if (n%16 == 1) {
            /* store address for this line */
            unsigned int end = 0, start = 0;
            end = *((unsigned int*) p);
            start = *((unsigned int*) data);
            snprintf(addrstr, sizeof(addrstr), "%.4x", end - start);
        }

        c = *p;
        if (isalnum(c) == 0) {
            c = '.';
        }

        /* store hex str (for left side) */
        snprintf(bytestr, sizeof(bytestr), "%02X ", *p);
        strncat(hexstr, bytestr, sizeof(hexstr)-strlen(hexstr)-1);

        /* store char str (for right side) */
        snprintf(bytestr, sizeof(bytestr), "%c", c);
        strncat(charstr, bytestr, sizeof(charstr)-strlen(charstr)-1);

        if (n%16 == 0) {
            /* line completed */
            ipc_client_log(client, "[%4.4s]   %-50.50s  %s", addrstr, hexstr, charstr);
            hexstr[0] = 0;
            charstr[0] = 0;
        } else if (n%8 == 0) {
            /* half line: add whitespaces */
            strncat(hexstr, "  ", sizeof(hexstr)-strlen(hexstr)-1);
            strncat(charstr, " ", sizeof(charstr)-strlen(charstr)-1);
        }
        p++; /* next byte */
    }

    if (strlen(hexstr) > 0) {
        /* print rest of buffer if not empty */
        ipc_client_log(client, "[%4.4s]   %-50.50s  %s\n", addrstr, hexstr, charstr);
    }
}

void ipc_client_log_recv(struct ipc_client *client,
    struct ipc_message_info *response, const char *prefix)
{
    switch (client->type) {
        case IPC_CLIENT_TYPE_FMT:
            ipc_client_log(client, "%s: RECV FMT!", prefix);
            ipc_client_log(client, "%s: Response: aseq=0x%02x command=%s (0x%04x) type=%s",
                prefix, response->aseq, ipc_command_to_str(IPC_COMMAND(response)), IPC_COMMAND(response), ipc_response_type_to_str(response->type));
#ifdef DEBUG
            if (response->length > 0) {
                ipc_client_log(client, "==== FMT DATA DUMP ====");
                ipc_client_hex_dump(client, (void *) response->data,
                    response->length > 0x100 ? 0x100 : response->length);
                ipc_client_log(client, "=======================");
            }
#endif
            break;
        case IPC_CLIENT_TYPE_RFS:
            ipc_client_log(client, "%s: RECV RFS!", prefix);
            ipc_client_log(client, "%s: Response: aseq=0x%02x command=%s (0x%04x)",
                prefix, response->aseq, ipc_command_to_str(IPC_COMMAND(response)), IPC_COMMAND(response));
#ifdef DEBUG
            if (response->length > 0) {
                ipc_client_log(client, "==== RFS DATA DUMP ====");
                ipc_client_hex_dump(client, (void *) response->data,
                    response->length > 0x100 ? 0x100 : response->length);
                ipc_client_log(client, "=======================");
            }
#endif
            break;
    }
}

void ipc_client_log_send(struct ipc_client *client,
    struct ipc_message_info *request, const char *prefix)
{
    switch (client->type) {
        case IPC_CLIENT_TYPE_FMT:
            ipc_client_log(client, "%s: SEND FMT!", prefix);
            ipc_client_log(client, "%s: Request: mseq=0x%02x command=%s (0x%04x) type=%s",
                prefix, request->mseq, ipc_command_to_str(IPC_COMMAND(request)), IPC_COMMAND(request), ipc_request_type_to_str(request->type));
#ifdef DEBUG
            if (request->length > 0) {
                ipc_client_log(client, "==== FMT DATA DUMP ====");
                ipc_client_hex_dump(client, (void *) request->data,
                    request->length > 0x100 ? 0x100 : request->length);
                ipc_client_log(client, "=======================");
            }
#endif
            break;
        case IPC_CLIENT_TYPE_RFS:
            ipc_client_log(client, "%s: SEND RFS!", prefix);
            ipc_client_log(client, "%s: Request: mseq=0x%02x command=%s (0x%04x)",
                prefix, request->mseq, ipc_command_to_str(IPC_COMMAND(request)), IPC_COMMAND(request));
#ifdef DEBUG
            if (request->length > 0) {
                ipc_client_log(client, "==== RFS DATA DUMP ====");
                ipc_client_hex_dump(client, (void *) request->data,
                    request->length > 0x100 ? 0x100 : request->length);
                ipc_client_log(client, "=======================");
            }
#endif
            break;
    }
}

void *ipc_client_mtd_read(struct ipc_client *client, char *mtd_name, int size,
    int block_size)
{
    void *mtd_p=NULL;
    uint8_t *data_p=NULL;

    loff_t offs;
    int fd;
    int i;

    if (mtd_name == NULL || size <= 0 || block_size <= 0)
        goto error;

    ipc_client_log(client, "ipc_client_mtd_read: reading 0x%x bytes from %s with 0x%x bytes block size\n", size, mtd_name, block_size);

    fd=open(mtd_name, O_RDONLY);
    if (fd < 0)
        goto error;

    mtd_p=malloc(size);
    if (mtd_p == NULL)
        goto error;

    memset(mtd_p, 0, size);

    data_p=(uint8_t *) mtd_p;

    for (i=0; i < size / block_size; i++)
    {
        offs = i * block_size;
        if (ioctl(fd, MEMGETBADBLOCK, &offs) == 1)
        {
            ipc_client_log(client, "ipc_client_mtd_read: warning: bad block at offset %lld\n", (long long int) offs);
            data_p+=block_size;
            continue;
        }

        read(fd, data_p, block_size);
        data_p+=block_size;
    }

    close(fd);

    return mtd_p;

error:
    ipc_client_log(client, "ipc_client_mtd_read: something went wrong\n");
    return NULL;
}

void *ipc_client_file_read(struct ipc_client *client, char *file_name, int size,
    int block_size)
{
    void *file_p=NULL;
    uint8_t *data_p=NULL;

    int fd;
    int i;

    if (file_name == NULL || size <= 0 || block_size <= 0)
        goto error;

    ipc_client_log(client, "ipc_client_file_read: reading 0x%x bytes from %s with 0x%x bytes block size\n", size, file_name, block_size);

    fd=open(file_name, O_RDONLY);
    if (fd < 0)
        goto error;

    file_p=malloc(size);
    if (file_p == NULL)
        goto error;

    memset(file_p, 0, size);

    data_p=(uint8_t *) file_p;

    for (i=0; i < size / block_size; i++)
    {
        read(fd, data_p, block_size);
        data_p+=block_size;
    }

    close(fd);

    return file_p;

error:
    ipc_client_log(client, "ipc_client_file_read: something went wrong\n");
    return NULL;
}

// vim:ts=4:sw=4:expandtab
