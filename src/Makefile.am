ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -DDEFAULT_CONF_DIR='"$(sysconfdir)/hwmultd"' -DDEFAULT_PLUGIN_PREFIX='"$(pkglibdir)"'

sbin_PROGRAMS = hwmultd
hwmultd_SOURCES = hwmultd.c cmdargs.c cmdargs.h log.c log.h mclient.c mserver.c mcast.h pidfile.c pidfile.h signalhandler.c signalhandler.h service.c serivce.h plugins.c plugins.h
hwmultd_LDFLAGS = -ldl
