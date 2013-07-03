NULL =

AM_CFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/samsung-ipc \
	-I$(top_srcdir)/samsung-ipc/device/xmm6160 \
	-I$(top_srcdir)/samsung-ipc/device/xmm6260 \
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
	rfs.c \
	misc.c \
	sec.c \
	sms.c \
	gen.c \
	gprs.c \
	call.c \
	net.c \
	util.c \
	ipc.h \
	ipc_devices.h \
	util.h \
	device/xmm6160/xmm6160.c \
	device/xmm6160/xmm6160.h \
	device/xmm6260/xmm6260.c \
	device/xmm6260/xmm6260_hsic.c \
	device/xmm6260/xmm6260_mipi.c \
	device/xmm6260/xmm6260_sec_modem.c \
	device/xmm6260/xmm6260.h \
	device/xmm6260/xmm6260_hsic.h \
	device/xmm6260/xmm6260_mipi.h \
	device/xmm6260/xmm6260_sec_modem.h \
	device/xmm6260/modem.h \
	device/xmm6260/modem_link_device_hsic.h \
	device/xmm6260/modem_prj.h \
	device/crespo/crespo_ipc.c \
	device/crespo/crespo_ipc.h \
	device/crespo/crespo_modem_ctl.h \
	device/aries/aries_ipc.c \
	device/aries/aries_ipc.h \
	device/aries/sipc4.h \
	device/aries/onedram.h \
	device/aries/phonet.h \
	device/galaxys2/galaxys2_ipc.c \
	device/galaxys2/galaxys2_ipc.h \
	device/galaxys3/galaxys3_ipc.c \
	device/galaxys3/galaxys3_ipc.h \
	device/maguro/maguro_ipc.c \
	device/maguro/maguro_ipc.h \
	device/piranha/piranha_ipc.c \
	device/piranha/piranha_ipc.h \
	$(NULL)

libsamsung_ipc_la_LIBADD = \
	$(OPENSSL_LIBS) \
	$(NULL)
