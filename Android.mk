LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

DEBUG := true

LOCAL_MODULE := libsamsung-ipc
LOCAL_MODULE_TAGS := optional

LOCAL_C_INCLUDES := external/openssl/include
LOCAL_LDFLAGS += -lcrypto

ifneq (,$(filter crespo,$(TARGET_DEVICE)))
	ipc_device_name := crespo
endif

ifneq (,$(filter galaxysmtd galaxytab,$(TARGET_DEVICE)))
	ipc_device_name := aries
endif

ifneq (,$(filter maguro,$(TARGET_DEVICE)))
	ipc_device_name := maguro
endif

ifneq (,$(filter p5100 p3100,$(TARGET_DEVICE)))
	ipc_device_name := piranha
endif

ifneq (,$(filter i9100 galaxys2 n7000,$(TARGET_DEVICE)))
	ipc_device_name := galaxys2
endif

ifneq (,$(filter i9300,$(TARGET_DEVICE)))
	ipc_device_name := i9300
endif

ifeq ($(DEBUG),true)
	LOCAL_CFLAGS += -DDEBUG
	LOCAL_CFLAGS += -DLOG_STDOUT
endif

samsung-ipc_files := \
	samsung-ipc/ipc.c \
	samsung-ipc/ipc_util.c \
	samsung-ipc/ipc_devices.c \
	samsung-ipc/call.c \
	samsung-ipc/rfs.c \
	samsung-ipc/gen.c \
	samsung-ipc/gprs.c \
	samsung-ipc/misc.c \
	samsung-ipc/net.c \
	samsung-ipc/sec.c \
	samsung-ipc/sms.c \
	samsung-ipc/util.c \
	samsung-ipc/device/xmm6160/xmm6160.c \
	samsung-ipc/device/xmm6260/xmm6260.c \
	samsung-ipc/device/xmm6260/xmm6260_hsic.c \
	samsung-ipc/device/xmm6260/xmm6260_mipi.c \
	samsung-ipc/device/xmm6260/xmm6260_sec_modem.c \
	samsung-ipc/device/crespo/crespo_ipc.c \
	samsung-ipc/device/aries/aries_ipc.c \
	samsung-ipc/device/galaxys2/galaxys2_ipc.c \
	samsung-ipc/device/i9300/i9300_ipc.c \
	samsung-ipc/device/maguro/maguro_ipc.c \
	samsung-ipc/device/piranha/piranha_ipc.c

LOCAL_SRC_FILES := $(samsung-ipc_files)
LOCAL_CFLAGS += -DIPC_DEVICE_NAME=\"$(ipc_device_name)\"

LOCAL_SHARED_LIBRARIES := libutils
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/include \
	$(LOCAL_PATH)/samsung-ipc \
	$(LOCAL_PATH)/samsung-ipc/device/xmm6160/ \
	$(LOCAL_PATH)/samsung-ipc/device/xmm6260/

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := ipc-modemctrl
LOCAL_MODULE_TAGS := optional

ifeq ($(DEBUG),true)
	LOCAL_CFLAGS += -DDEBUG
	LOCAL_CFLAGS += -DLOG_STDOUT
endif

modemctrl_files := tools/modemctrl.c

LOCAL_SRC_FILES := $(modemctrl_files)

LOCAL_STATIC_LIBRARIES := libsamsung-ipc
LOCAL_SHARED_LIBRARIES := libutils
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include

include $(BUILD_EXECUTABLE)
