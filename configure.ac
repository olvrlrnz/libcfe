AC_PREREQ([2.69])

AC_INIT([libcfe],[0.1.0],[libcfe@olorenz.org])

AC_CONFIG_SRCDIR([src/log.c])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_LIBOBJ_DIR([compat])
AC_CONFIG_TESTDIR([test])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_PRESERVE_HELP_ORDER

AM_INIT_AUTOMAKE([1.14 check-news foreign silent-rules subdir-objects -Wall -Werror])
AC_CONFIG_HEADER([config.h])

AM_SILENT_RULES([no])
AM_MAINTAINER_MODE([enable])

AC_LANG([C])
AC_USE_SYSTEM_EXTENSIONS
AM_PROG_AR
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CC_C99
AC_PROG_INSTALL
AC_PROG_MAKE_SET

AC_HEADER_STDC
AC_C_CONST
AC_C_INLINE
AC_C_RESTRICT

AC_MSG_CHECKING([if we have pthreads support])
m4_include([m4/ax_pthread.m4])
AX_PTHREAD([
        AC_DEFINE(HAVE_PTHREAD,1,[Define if you have POSIX threads libraries and header files.])
        CLIBS="$PTHREAD_LIBS $LIBS"
        CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
        LDFLAGS="$LDFLAGS $PTHREAD_CFLAGS"
        CC="$PTHREAD_CC"],
        AC_MSG_ERROR([No pthread support on this machine]))

LT_INIT
LT_LANG([C])

PKG_PROG_PKG_CONFIG([0.28])
PKG_CHECK_MODULES([CMOCKA], [cmocka >= 1.0.1])

case $host_os in
	darwin*)
		AC_MSG_NOTICE([OS Darwin])
		#AC_SUBST(DARWIN)
		;;
	linux*)
		AC_MSG_NOTICE([OS Linux])
		#AC_SUBST(LINUX)
		;;
	*BSD*)
		AC_MSG_NOTICE([OS BSD])
		#AC_SUBST(BSD)
		;;
	*)
		AC_MSG_NOTICE([OS Unknown])
		#AC_SUBST(UNKNOWN)
		;;
esac

AC_CONFIG_FILES([Makefile
                 include/Makefile
                 lib/Makefile
                 lib/libcfe.pc
                 src/Makefile
                 test/Makefile
               ])
AC_OUTPUT
