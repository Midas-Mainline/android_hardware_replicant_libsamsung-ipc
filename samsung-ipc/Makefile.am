NULL =

AM_CFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/samsung-ipc \
	$(OPENSSL_CFLAGS) \
	$(NULL)

if WANT_DEBUG
AM_CFLAGS += -ggdb -O0
endif

lib_LTLIBRARIES = \
	libsamsung-ipc.la \
	$(NULL)

libsamsung_ipc_la_SOURCES = \
	ipc.c \
	ipc_util.c \
	ipc_devices.c \
	util.c \
	\
	rfs.c \
	misc.c \
	sec.c \
	gen.c \
	gprs.c \
	call.c \
	net.c \
	\
	device/crespo/crespo_ipc.c \
	device/aries/aries_ipc.c \
	device/h1/h1_ipc.c \
	\
	ipc_private.h \
	ipc_devices.h \
	device/aries/sipc4.h \
	device/aries/aries_ipc.h \
	device/aries/onedram.h \
	device/aries/phonet.h \
	device/h1 \
	device/h1/h1_ipc.h \
	device/crespo/crespo_modem_ctl.h \
	device/crespo/crespo_ipc.h \
	$(NULL)

AM_CFLAGS += -DDEVICE_IPC_V4

libsamsung_ipc_la_LIBADD = \
	$(OPENSSL_LIBS) \
	$(NULL)
