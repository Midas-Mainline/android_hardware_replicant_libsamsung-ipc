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
	ipc.h \
	ipc_strings.c \
	ipc_utils.c \
	utils.c \
	call.c \
	sms.c \
	sec.c \
	net.c \
	misc.c \
	svc.c \
	gprs.c \
	rfs.c \
	gen.c \
	$(NULL)

include devices/Makefile.am
include modems/Makefile.am
include partitions/Makefile.am

libsamsung_ipc_la_LIBADD = \
	$(OPENSSL_LIBS) \
	$(NULL)
