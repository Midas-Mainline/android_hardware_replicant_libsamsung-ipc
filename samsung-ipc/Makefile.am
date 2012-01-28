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
	util.c \
	\
	rfs.c \
	misc.c \
	sec.c \
	gen.c \
	gprs.c \
	call.c \
	net.c \
	sms.c \
	$(NULL)

libsamsung_ipc_la_SOURCES += \
	device/crespo/crespo_ipc.c \
	$(NULL)
AM_CFLAGS += -DDEVICE_IPC_V4

libsamsung_ipc_la_LIBADD = \
	$(OPENSSL_LIBS) \
	$(NULL)
