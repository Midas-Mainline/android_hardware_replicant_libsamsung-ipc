NULL =

AM_CFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/samsung-ipc \
	-I$(top_srcdir)/samsung-ipc/devices \
	-I$(top_srcdir)/samsung-ipc/modems/xmm616 \
	-I$(top_srcdir)/samsung-ipc/modems/xmm626 \
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
	ipc_utils.c \
	modems/xmm616/xmm616.c \
	modems/xmm616/xmm616.h \
	modems/xmm626/xmm626.c \
	modems/xmm626/xmm626.h \
	modems/xmm626/xmm626_hsic.c \
	modems/xmm626/xmm626_hsic.h \
	modems/xmm626/xmm626_mipi.c \
	modems/xmm626/xmm626_mipi.h \
	modems/xmm626/xmm626_sec_modem.c \
	modems/xmm626/xmm626_sec_modem.h \
	modems/xmm626/xmm626_modem.h \
	modems/xmm626/xmm626_modem_link_device_hsic.h \
	modems/xmm626/xmm626_modem_prj.h \
	devices/ipc_devices.c \
	devices/ipc_devices.h \
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
	devices/n5100/n5100.c \
	devices/n5100/n5100.h \
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

libsamsung_ipc_la_LIBADD = \
	$(OPENSSL_LIBS) \
	$(NULL)
