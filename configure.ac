#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([hwmultd],[0.1],[http://opensource.dyc.edu/bugzilla3])

AM_INIT_AUTOMAKE([1.11 foreign])

LT_PREREQ([2.2])
LT_INIT([dlopen])

AC_CONFIG_SRCDIR([src/hwmultd.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h])
AC_CHECK_HEADERS([utmp.h])
AC_CHECK_HEADERS([netdb.h])
AC_CHECK_HEADERS([netinet/in.h])
AC_CHECK_HEADERS([pwd.h])
AC_CHECK_HEADERS([signal.h])
AC_CHECK_HEADERS([stdarg.h])
AC_CHECK_HEADERS([stdint.h])
AC_CHECK_HEADERS([stdio.h])
AC_CHECK_HEADERS([stdlib.h])
AC_CHECK_HEADERS([string.h])
AC_CHECK_HEADERS([sys/socket.h])
AC_CHECK_HEADERS([sys/stat.h])
AC_CHECK_HEADERS([sys/types.h])
AC_CHECK_HEADERS([time.h])
AC_CHECK_HEADERS([unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_PID_T
AC_TYPE_UID_T
AC_TYPE_UINT16_T

# Checks for library functions.
AC_FUNC_CHOWN
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_CHECK_FUNCS([gethostbyname])
AC_CHECK_FUNCS([memset])
AC_CHECK_FUNCS([mkdir])
AC_CHECK_FUNCS([socket])


AC_CONFIG_FILES([Makefile
                 plugins/Makefile
                 src/Makefile
                 etc/Makefile
                 doc/Makefile])
AC_OUTPUT
