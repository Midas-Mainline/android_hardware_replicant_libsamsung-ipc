# This file is part of libsamsung-ipc.
#
# Copyright (C) 2011-2013 Paul Kocialkowski <contact@paulk.fr>
#
# libsamsung-ipc is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# libsamsung-ipc is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with libsamsung-ipc.  If not, see <http://www.gnu.org/licenses/>.

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

ifneq (,$(filter crespo,$(TARGET_DEVICE)))
	ipc_device_name := crespo
endif

ifneq (,$(filter galaxysmtd galaxytab,$(TARGET_DEVICE)))
	ipc_device_name := aries
endif

ifneq (,$(filter i9100 galaxys2 n7000,$(TARGET_DEVICE)))
	ipc_device_name := galaxys2
endif

ifneq (,$(filter maguro,$(TARGET_DEVICE)))
	ipc_device_name := maguro
endif

ifneq (,$(filter p5100 p3100,$(TARGET_DEVICE)))
	ipc_device_name := piranha
endif

ifneq (,$(filter i9300,$(TARGET_DEVICE)))
	ipc_device_name := i9300
endif

ifneq (,$(filter n7100,$(TARGET_DEVICE)))
	ipc_device_name := n7100
endif

LOCAL_SRC_FILES := \
	samsung-ipc/ipc.c \
	samsung-ipc/ipc_devices.c \
	samsung-ipc/ipc_util.c \
	samsung-ipc/devices/xmm6160/xmm6160.c \
	samsung-ipc/devices/xmm6260/xmm6260.c \
	samsung-ipc/devices/xmm6260/xmm6260_hsic.c \
	samsung-ipc/devices/xmm6260/xmm6260_mipi.c \
	samsung-ipc/devices/xmm6260/xmm6260_sec_modem.c \
	samsung-ipc/devices/crespo/crespo_ipc.c \
	samsung-ipc/devices/aries/aries_ipc.c \
	samsung-ipc/devices/galaxys2/galaxys2_ipc.c \
	samsung-ipc/devices/maguro/maguro_ipc.c \
	samsung-ipc/devices/piranha/piranha_ipc.c \
	samsung-ipc/devices/i9300/i9300_ipc.c \
	samsung-ipc/devices/n7100/n7100_ipc.c \
	samsung-ipc/util.c \
	samsung-ipc/call.c \
	samsung-ipc/sms.c \
	samsung-ipc/sec.c \
	samsung-ipc/net.c \
	samsung-ipc/misc.c \
	samsung-ipc/gprs.c \
	samsung-ipc/rfs.c \
	samsung-ipc/gen.c

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/include \
	$(LOCAL_PATH)/samsung-ipc \
	$(LOCAL_PATH)/samsung-ipc/devices/xmm6160/ \
	$(LOCAL_PATH)/samsung-ipc/devices/xmm6260/ \
	external/openssl/include

LOCAL_CFLAGS += \
	-DIPC_DEVICE_NAME=\"$(ipc_device_name)\" \
	-DDEBUG

LOCAL_SHARED_LIBRARIES := libutils libcrypto

LOCAL_MODULE := libsamsung-ipc
LOCAL_MODULE_TAGS := optional

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := tools/modemctrl.c

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include

LOCAL_STATIC_LIBRARIES := libsamsung-ipc
LOCAL_SHARED_LIBRARIES := libutils

LOCAL_MODULE := ipc-modemctrl
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)
