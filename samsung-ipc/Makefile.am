NULL =

AM_CFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/samsung-ipc \
	-I$(top_srcdir)/samsung-ipc/devices/xmm6160 \
	-I$(top_srcdir)/samsung-ipc/devices/xmm6260 \
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
	ipc_devices.c \
	ipc_devices.h \
	ipc_utils.c \
	devices/xmm6160/xmm6160.c \
	devices/xmm6160/xmm6160.h \
	devices/xmm6260/xmm6260.c \
	devices/xmm6260/xmm6260.h \
	devices/xmm6260/xmm6260_hsic.c \
	devices/xmm6260/xmm6260_hsic.h \
	devices/xmm6260/xmm6260_mipi.c \
	devices/xmm6260/xmm6260_mipi.h \
	devices/xmm6260/xmm6260_sec_modem.c \
	devices/xmm6260/xmm6260_sec_modem.h \
	devices/xmm6260/modem.h \
	devices/xmm6260/modem_link_device_hsic.h \
	devices/xmm6260/modem_prj.h \
	devices/crespo/crespo.c \
	devices/crespo/crespo.h \
	devices/crespo/crespo_modem_ctl.h \
	devices/aries/aries.c \
	devices/aries/aries.h \
	devices/aries/onedram.h \
	devices/aries/phonet.h \
	devices/galaxys2/galaxys2.c \
	devices/galaxys2/galaxys2.h \
	devices/maguro/maguro.c \
	devices/maguro/maguro.h \
	devices/piranha/piranha.c \
	devices/piranha/piranha.h \
	devices/i9300/i9300.c \
	devices/i9300/i9300.h \
	devices/n7100/n7100.c \
	devices/n7100/n7100.h \
	utils.c \
	utils.h \
	call.c \
	sms.c \
	sec.c \
	net.c \
	misc.c \
	gprs.c \
	rfs.c \
	gen.c \
	$(NULL)

libsamsung_ipc_la_LIBADD = \
	$(OPENSSL_LIBS) \
	$(NULL)
