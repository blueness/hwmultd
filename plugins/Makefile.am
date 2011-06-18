AM_CPPFLAGS = -DDEFAULT_CONF_DIR='"$(sysconfdir)/hwmultd"'

CLIENTS = libnull_cl.la libexample_cl.la libscript_cl.la
SERVERS = libnull_hw.la libexample_hw.la libscript_hw.la libtime_hw.la libtemp_hw.la libentropy_hw.la

pkglib_LTLIBRARIES = $(CLIENTS) $(SERVERS)

libnull_cl_la_SOURCES = libnull_cl.c clcommon.h
libexample_cl_la_SOURCES = libexample_cl.c clcommon.h
libscript_cl_la_SOURCES = libscript_cl.c clcommon.h

libnull_hw_la_SOURCES = libnull_hw.c hwcommon.h
libexample_hw_la_SOURCES = libexample_hw.c hwcommon.h
libscript_hw_la_SOURCES = libscript_hw.c hwcommon.h

libtime_hw_la_SOURCES = libtime_hw.c hwcommon.h
libtemp_hw_la_SOURCES = libtemp_hw.c hwcommon.h
libtemp_hw_la_SOURCES = libentropy_hw.c hwcommon.h
