ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -I$(top_srcdir)/include -DDEFAULT_CONF_DIR='"$(sysconfdir)/hwmultd"'

CLIENTS = null_cl.la example_cl.la script_cl.la entropy_cl.la
SERVERS = null_hw.la example_hw.la script_hw.la entropy_hw.la time_hw.la temp_hw.la

pkglib_LTLIBRARIES = $(CLIENTS) $(SERVERS)

#
# Client plugins
#

null_cl_la_SOURCES = null_cl.c
null_cl_la_LDFLAGS = -module -avoid-version

example_cl_la_SOURCES = example_cl.c
example_cl_la_LDFLAGS = -module -avoid-version

script_cl_la_SOURCES = script_cl.c
script_cl_la_LDFLAGS = -module -avoid-version

entropy_cl_la_SOURCES = entropy_cl.c
entropy_cl_la_LDFLAGS = -module -avoid-version

#
# Hardware(server) plugins
#

null_hw_la_SOURCES = null_hw.c
null_hw_la_LDFLAGS = -module -avoid-version

example_hw_la_SOURCES = example_hw.c
example_hw_la_LDFLAGS = -module -avoid-version

script_hw_la_SOURCES = script_hw.c
script_hw_la_LDFLAGS = -module -avoid-version

entropy_hw_la_SOURCES = entropy_hw.c
entropy_hw_la_LDFLAGS = -module -avoid-version

temp_hw_la_SOURCES = temp_hw.c
temp_hw_la_LDFLAGS = -module -avoid-version

time_hw_la_SOURCES = time_hw.c
time_hw_la_LDFLAGS = -module -avoid-version
