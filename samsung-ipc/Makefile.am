NULL =

AM_CFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/samsung-ipc \
	$(OPENSSL_CFLAGS) \
	$(NULL)

if WANT_DEBUG
AM_CFLAGS += -ggdb -O0 -DDEBUG
endif

lib_LTLIBRARIES = \
	libsamsung-ipc.la \
	$(NULL)

libsamsung_ipc_la_SOURCES = \
	ipc.c \
	ipc_util.c \
	ipc_devices.c \
	util.c \
	rfs.c \
	misc.c \
	sec.c \
	gen.c \
	gprs.c \
	call.c \
	net.c \
	ipc_private.h \
	ipc_devices.h \
	device/crespo/crespo_ipc.c \
	device/aries/aries_ipc.c \
	device/aries/sipc4.h \
	device/aries/aries_ipc.h \
	device/aries/onedram.h \
	device/aries/phonet.h \
	device/crespo/crespo_modem_ctl.h \
	device/crespo/crespo_ipc.h \
	device/xmm6260/common.h \
	device/xmm6260/fwloader_i9100.c \
	device/xmm6260/fwloader_i9250.c \
	device/xmm6260/io_helpers.c \
	device/xmm6260/io_helpers.h \
	device/xmm6260/log.c \
	device/xmm6260/log.h \
	device/xmm6260/modem_prj.h \
	device/xmm6260/modemctl_common.c \
	device/xmm6260/modemctl_common.h \
	device/xmm6260/xmm6260_ipc.c \
	device/xmm6260/xmm6260_ipc.h \
	$(NULL)

AM_CFLAGS += -DDEVICE_IPC_V4

libsamsung_ipc_la_LIBADD = \
	$(OPENSSL_LIBS) \
	$(NULL)
