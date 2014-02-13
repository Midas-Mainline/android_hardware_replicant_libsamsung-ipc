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
	ipc_util.c \
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
	devices/crespo/crespo_ipc.c \
	devices/crespo/crespo_ipc.h \
	devices/crespo/crespo_modem_ctl.h \
	devices/aries/aries_ipc.c \
	devices/aries/aries_ipc.h \
	devices/aries/onedram.h \
	devices/aries/phonet.h \
	devices/galaxys2/galaxys2_ipc.c \
	devices/galaxys2/galaxys2_ipc.h \
	devices/maguro/maguro_ipc.c \
	devices/maguro/maguro_ipc.h \
	devices/piranha/piranha_ipc.c \
	devices/piranha/piranha_ipc.h \
	devices/i9300/i9300_ipc.c \
	devices/i9300/i9300_ipc.h \
	devices/n7100/n7100_ipc.c \
	devices/n7100/n7100_ipc.h \
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
