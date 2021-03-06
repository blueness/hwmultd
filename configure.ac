#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([hwmultd],[0.2],[http://opensource.dyc.edu/bugzilla3])

AM_INIT_AUTOMAKE([1.11 foreign])
AM_SILENT_RULES([yes])

LT_PREREQ([2.4])
LT_INIT([dlopen])

AC_CONFIG_SRCDIR([src/hwmultd.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([dl], [dlopen], [], [AC_MSG_ERROR(["Missing dlopen"])])

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h dlfcn.h fcntl.h ifaddrs.h netdb.h netinet/in.h \
	pwd.h signal.h stdarg.h stdint.h stdio.h stdlib.h string.h sys/socket.h \
	sys/stat.h sys/types.h syslog.h termios.h time.h unistd.h utmp.h],
[], [AC_MSG_ERROR(["Missing necessary header"])])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_PID_T
AC_TYPE_UID_T
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T

# Checks for library functions.
AC_FUNC_CHOWN
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_CHECK_FUNCS([gethostbyname inet_ntoa memset mkdir socket],
[], [AC_MSG_ERROR(["Missing necessary function"])])

AC_CONFIG_FILES([Makefile
	plugins/Makefile
	include/Makefile
	src/Makefile
	etc/Makefile
	etc/scripts/Makefile
	doc/Makefile])

AC_OUTPUT
