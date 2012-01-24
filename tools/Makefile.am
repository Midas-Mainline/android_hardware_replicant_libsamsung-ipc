NULL =

AM_CFLAGS = \
	-I$(top_srcdir)/include \
	$(NULL)

if WANT_PROTOCOL_VERISON_CRESPO
AM_CFLAGS += -DDEVICE_CRESPO
endif

if WANT_PROTOCOL_VERISON_H1
AM_CFLAGS += -DDEVICE_H1
endif

bin_PROGRAMS = \
	modemctrl \
	samsung_ipc_forward \
	$(NULL)

modemctrl_SOURCES = modemctrl.c
modemctrl_LDADD = $(top_srcdir)/samsung-ipc/libsamsung-ipc.la
modemctrl_LDFLAGS =

samsung_ipc_forward_SOURCES = forward.c
samsung_ipc_forward_LDADD = $(top_srcdir)/samsung-ipc/libsamsung-ipc.la
modemctrl_LDFLAGS =
