NULL =

AM_CFLAGS = \
	-I$(top_srcdir)/include \
	$(NULL)

AM_CFLAGS += -DDEVICE_IPC_V4

bin_PROGRAMS = \
	modemctrl \
	samsung_ipc_forward \
	$(NULL)

modemctrl_SOURCES = modemctrl.c
modemctrl_LDADD = $(top_builddir)/samsung-ipc/libsamsung-ipc.la
modemctrl_LDFLAGS =

samsung_ipc_forward_SOURCES = forward.c
samsung_ipc_forward_LDADD = $(top_builddir)/samsung-ipc/libsamsung-ipc.la
modemctrl_LDFLAGS =
