BUILD_IPC-MODEMCTRL := true
DEBUG := true

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := libsamsung-ipc
LOCAL_MODULE_TAGS := optional

LOCAL_C_INCLUDES := external/openssl/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/samsung-ipc/device/xmm6260/
LOCAL_LDFLAGS += -lcrypto

ifeq ($(TARGET_DEVICE),crespo)
	board_name := herring
endif

ifeq ($(TARGET_DEVICE),galaxys2)
	board_name := smdk4210
endif

ifeq ($(TARGET_DEVICE),galaxysmtd)
	board_name := aries
endif

ifeq ($(TARGET_DEVICE),galaxytab)
	board_name := gt-p1000
endif

ifeq ($(TARGET_DEVICE),spica)
	board_name := i5700
endif

ifeq ($(TARGET_DEVICE),maguro)
	board_name := tuna
endif

ifeq ($(TARGET_DEVICE),p5100)
	board_name := espresso10
endif

ifeq ($(TARGET_DEVICE),p3100)
	board_name := espresso
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
	samsung-ipc/device/crespo/crespo_ipc.c \
	samsung-ipc/device/aries/aries_ipc.c \
	samsung-ipc/device/xmm6260/xmm6260_loader.c \
	samsung-ipc/device/xmm6260/xmm6260_modemctl.c \
	samsung-ipc/device/xmm6260/xmm6260_ipc.c \
	samsung-ipc/device/galaxys2/galaxys2_loader.c \
	samsung-ipc/device/galaxys2/galaxys2_ipc.c \
	samsung-ipc/device/maguro/maguro_loader.c \
	samsung-ipc/device/maguro/maguro_ipc.c \
	samsung-ipc/device/piranha/piranha_loader.c \
	samsung-ipc/device/piranha/piranha_ipc.c

LOCAL_SRC_FILES := $(samsung-ipc_files)
LOCAL_CFLAGS += -DIPC_BOARD_NAME_EXPLICIT=\"$(board_name)\"

LOCAL_SHARED_LIBRARIES := libutils
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include \
	$(LOCAL_PATH)/samsung-ipc

include $(BUILD_STATIC_LIBRARY)

ifeq ($(BUILD_IPC-MODEMCTRL),true)

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

endif
