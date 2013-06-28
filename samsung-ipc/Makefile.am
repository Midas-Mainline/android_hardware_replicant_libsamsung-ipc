NULL =

AM_CFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/samsung-ipc \
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
	device/crespo/crespo_ipc.c \
	device/aries/aries_ipc.c \
	device/aries/sipc4.h \
	device/aries/aries_ipc.h \
	device/aries/onedram.h \
	device/aries/phonet.h \
	device/crespo/crespo_modem_ctl.h \
	device/crespo/crespo_ipc.h \
	device/xmm6260/xmm6260_loader.c \
	device/xmm6260/xmm6260_loader.h \
	device/xmm6260/xmm6260_modemctl.c \
	device/xmm6260/xmm6260_modemctl.h \
	device/xmm6260/xmm6260_ipc.c \
	device/xmm6260/xmm6260_ipc.h \
	device/xmm6260/modem_prj.h \
	device/galaxys2/galaxys2_loader.c \
	device/galaxys2/galaxys2_loader.h \
	device/galaxys2/galaxys2_ipc.c \
	device/maguro/maguro_loader.c \
	device/maguro/maguro_loader.h \
	device/maguro/maguro_ipc.c \
	device/piranha/piranha_loader.c \
	device/piranha/piranha_loader.h \
	device/piranha/piranha_ipc.c \
	$(NULL)

libsamsung_ipc_la_LIBADD = \
	$(OPENSSL_LIBS) \
	$(NULL)
