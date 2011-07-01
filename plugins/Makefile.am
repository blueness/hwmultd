AM_CPPFLAGS = -I$(top_srcdir)/include -DDEFAULT_CONF_DIR='"$(sysconfdir)/hwmultd"'

CLIENTS = libnull_cl.la libexample_cl.la libscript_cl.la libentropy_cl.la
SERVERS = libnull_hw.la libexample_hw.la libscript_hw.la libentropy_hw.la libtime_hw.la libtemp_hw.la

pkglib_LTLIBRARIES = $(CLIENTS) $(SERVERS)

libnull_cl_la_SOURCES = libnull_cl.c
libexample_cl_la_SOURCES = libexample_cl.c
libscript_cl_la_SOURCES = libscript_cl.c
libentropy_cl_la_SOURCES = libentropy_cl.c


libnull_hw_la_SOURCES = libnull_hw.c
libexample_hw_la_SOURCES = libexample_hw.c
libscript_hw_la_SOURCES = libscript_hw.c
libentropy_hw_la_SOURCES = libentropy_hw.c

libtemp_hw_la_SOURCES = libtemp_hw.c
libtime_hw_la_SOURCES = libtime_hw.c
