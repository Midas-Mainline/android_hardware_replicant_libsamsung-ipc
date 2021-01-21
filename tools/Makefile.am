NULL =

AM_CFLAGS = \
	-I$(top_srcdir)/include \
	$(NULL)

bin_PROGRAMS = \
	ipc-modem \
	ipc-imei \
	ipc-test \
	nv_data-imei \
	nv_data-md5 \
	$(NULL)

# TODO: Find a way to make test more modular and represent each run of the
# nv_data-imei in TEST while having it implemented in a single python file
PY_LOG_COMPILER = $(PYTHON)
TEST_EXTENSIONS = .py
TESTS = nv_data-imei.py \
	nv_data-md5.py

ipc_modem_SOURCES = ipc-modem.c
ipc_modem_LDADD = $(top_builddir)/samsung-ipc/libsamsung-ipc.la
ipc_modem_LDFLAGS =

ipc_imei_SOURCES = ipc-imei.c common/modem.c
ipc_imei_LDADD = $(top_builddir)/samsung-ipc/libsamsung-ipc.la
ipc_imei_LDFLAGS =

ipc_test_SOURCES = ipc-test.c
ipc_test_LDADD = $(top_builddir)/samsung-ipc/libsamsung-ipc.la
ipc_test_LDFLAGS =

nv_data_md5_SOURCES = nv_data-md5.c
nv_data_md5_LDADD = $(top_builddir)/samsung-ipc/libsamsung-ipc.la
nv_data_md5_LDFLAGS =

nv_data_imei_SOURCES = nv_data-imei.c
nv_data_imei_LDADD = $(top_builddir)/samsung-ipc/libsamsung-ipc.la
nv_data_imei_LDFLAGS =
