##
 #  ummd ( Micro MultiMedia Daemon )
 ##

AC_PREREQ(2.57)

AC_INIT(ummd, 0.1)

AC_CONFIG_AUX_DIR(autotools)
AC_CONFIG_MACRO_DIR(m4)
AC_CONFIG_HEADER(include/autoconf.h)
AC_CONFIG_SRCDIR(src/main.c)

AC_CANONICAL_BUILD
AC_CANONICAL_HOST

AM_INIT_AUTOMAKE(1.7)

AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CPP
AC_PROG_LIBTOOL
AC_PROG_INSTALL

PKG_PROG_PKG_CONFIG

MY_CFLAGS=""
MY_LIBS=""

MY_CFG_DIR="${sysconfdir}"
MY_LOG_DIR="${localstatedir}/log"
MY_RUN_DIR="${localstatedir}/run"


dnl # options

AC_ARG_ENABLE(
	[debug],
	[AC_HELP_STRING([--enable-debug], [enable debugging support [default=no]])],
	[my_enable_debug=$enableval],
	[my_enable_debug='no']
)
if test "x$my_enable_debug" = "xyes"; then
	AC_DEFINE([MY_DEBUGGING], [1], [ Define to 1 if debugging support is enabled. ])
fi


dnl # headers & libraries checks

AC_CHECK_HEADERS([sys/soundcard.h linux/soundcard.h machine/soundcard.h])
if test "x${ac_cv_header_sys_soundcard_h}" = "xyes" \
	 -o "x${ac_cv_header_linux_soundcard_h}" = "xyes" \
	 -o "x${ac_cv_header_machine_soundcard_h}" = "xyes";
then
	my_enable_oss="yes"
else
	my_enable_oss="no"
fi
if test "x$my_enable_oss" = "xyes"; then
	AC_DEFINE([HAVE_OSS], [1], [ Define to 1 if OSS support is enabled. ])
fi

AC_CHECK_LIB(
	[pthread],
	[pthread_create],
	[MY_LIBS="$MY_LIBS -lpthread"]
)

PKG_CHECK_MODULES(LIBAVCODEC, libavcodec >= 52)
PKG_CHECK_MODULES(LIBAVDEVICE, libavdevice >= 52)
PKG_CHECK_MODULES(LIBAVFORMAT, libavformat >= 52)
PKG_CHECK_MODULES(LIBAVUTIL, libavutil >= 49)

PKG_CHECK_MODULES(LIBCONFIG, libconfig >= 1.0)

PKG_CHECK_MODULES(LIBLO, liblo >= 0.26, [my_enable_osc='yes'], [my_enable_osc='no'])
if test "x$my_enable_osc" = "xyes"; then
	AC_DEFINE([HAVE_OSC], [1], [ Define to 1 if OSC support is enabled. ])
fi

PKG_CHECK_MODULES(LIBMPG123, libmpg123 >= 1.12)

AC_CONFIG_FILES([
	Makefile
	doc/Makefile
	include/Makefile
	src/Makefile
	src/conf/Makefile
	src/core/Makefile
	src/core/controls/Makefile
	src/core/filters/Makefile
	src/core/io/Makefile
	src/test/Makefile
	src/util/Makefile
])

AC_SUBST(HAVE_OSS)
AC_SUBST(OSS_LIBS)
AC_SUBST(OSS_LDFLAGS)
AC_SUBST(OSS_CFLAGS)
AM_CONDITIONAL( [HAVE_OSC], [test "x$my_enable_osc" = "xyes"] )
AM_CONDITIONAL( [HAVE_OSS], [test "x$my_enable_oss" = "xyes"] )

AC_SUBST(MY_DEBUGGING)

AC_SUBST(MY_CFLAGS)
AC_SUBST(MY_LIBS)

AC_SUBST(MY_CFG_DIR)
AC_SUBST(MY_LOG_DIR)
AC_SUBST(MY_RUN_DIR)
AC_OUTPUT
