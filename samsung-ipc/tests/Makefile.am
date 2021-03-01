NULL =

AM_CFLAGS = \
        -I$(top_srcdir)/include \
        -I$(top_srcdir)/samsung-ipc \
        $(OPENSSL_CFLAGS) \
        $(NULL)

if WANT_DEBUG
AM_CFLAGS += -ggdb -O0 -DDEBUG
endif

bin_PROGRAMS = libsamsung-ipc-test

libsamsung_ipc_test_SOURCES = \
	main.c \
	partitions/android.c \
	partitions/android.h \
	$(NULL)

libsamsung_ipc_test_LDADD = $(top_builddir)/samsung-ipc/libsamsung-ipc.la
libsamsung_ipc_test_LDFLAGS =

# TODO: Find a way to make test more modular and represent each run of
# libsamsung-ipc-test in TEST while having it implemented in a single
# python file
PY_LOG_COMPILER = $(PYTHON)
TEST_EXTENSIONS = .py
TESTS = libsamsung-ipc-test.py
