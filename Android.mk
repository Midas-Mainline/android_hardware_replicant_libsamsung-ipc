BUILD_IPC-MODEMCTRL := true
DEBUG := true

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := libsamsung-ipc
LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS += -Iexternal/openssl/include
LOCAL_LDFLAGS += -lcrypto

ifeq ($(TARGET_DEVICE),crespo)
	LOCAL_CFLAGS += -DDEVICE_IPC_V4
	samsung-ipc_device := crespo
endif

ifeq ($(TARGET_DEVICE),galaxysmtd)
	LOCAL_CFLAGS += -DDEVICE_IPC_V4
	samsung-ipc_device := aries
endif

ifeq ($(TARGET_DEVICE),galaxytab)
	LOCAL_CFLAGS += -DDEVICE_IPC_V4
	samsung-ipc_device := aries
endif

ifeq ($(TARGET_DEVICE),h1)
	LOCAL_CFLAGS += -DDEVICE_H1
endif

ifeq ($(DEBUG),true)
	LOCAL_CFLAGS += -DDEBUG
	LOCAL_CFLAGS += -DLOG_STDOUT
endif

samsung-ipc_files := \
	samsung-ipc/ipc.c \
	samsung-ipc/ipc_util.c \
	samsung-ipc/ipc_devices.c \
	samsung-ipc/util.c \
	samsung-ipc/rfs.c \
	samsung-ipc/gen.c \
	samsung-ipc/gprs.c \
	samsung-ipc/misc.c \
	samsung-ipc/net.c \
	samsung-ipc/sec.c \
	samsung-ipc/device/$(samsung-ipc_device)/$(samsung-ipc_device)_ipc.c

LOCAL_SRC_FILES := $(samsung-ipc_files)

LOCAL_SHARED_LIBRARIES := libutils
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include \
	$(LOCAL_PATH)/samsung-ipc

include $(BUILD_STATIC_LIBRARY)

ifeq ($(BUILD_IPC-MODEMCTRL),true)

include $(CLEAR_VARS)

LOCAL_MODULE := ipc-modemctrl
LOCAL_MODULE_TAGS := optional

ifeq ($(TARGET_DEVICE),crespo)
	LOCAL_CFLAGS += -DDEVICE_IPC_V4
	samsung-ipc_device := crespo
endif

ifeq ($(TARGET_DEVICE),galaxysmtd)
	LOCAL_CFLAGS += -DDEVICE_IPC_V4
	samsung-ipc_device := aries
endif

ifeq ($(TARGET_DEVICE),galaxytab)
	LOCAL_CFLAGS += -DDEVICE_IPC_V4
	samsung-ipc_device := aries
endif

ifeq ($(TARGET_DEVICE),h1)
	LOCAL_CFLAGS += -DDEVICE_H1
endif

ifeq ($(DEBUG),true)
	LOCAL_CFLAGS += -DDEBUG
	LOCAL_CFLAGS += -DLOG_STDOUT
endif

modemctrl_files := tools/modemctrl.c

LOCAL_SRC_FILES := $(modemctrl_files)

LOCAL_STATIC_LIBRARIES := libsamsung-ipc
LOCAL_SHARED_LIBRARIES := libutils
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include

include $(BUILD_EXECUTABLE)

endif
