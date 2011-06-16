AM_CPPFLAGS = -DDEFAULT_CONF_DIR='"$(sysconfdir)/hwmultd"'

pkglib_LTLIBRARIES = libnull_cl.la libexample_cl.la libscript_cl.la libnull_hw.la libexample_hw.la libtemp_hw.la libtime_hw.la

libnull_cl_la_SOURCES = libnull_cl.c clcommon.h
libexample_cl_la_SOURCES = libexample_cl.c clcommon.h
libscript_cl_la_SOURCES = libscript_cl.c clcommon.h

libnull_hw_la_SOURCES = libnull_hw.c hwcommon.h
libexample_hw_la_SOURCES = libexample_hw.c clcommon.h
libtemp_hw_la_SOURCES = libtemp_hw.c hwcommon.h
libtime_hw_la_SOURCES = libtime_hw.c hwcommon.h
