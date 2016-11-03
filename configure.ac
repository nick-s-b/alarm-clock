dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_INIT(alarm-clock, 1.4.2)

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AC_CONFIG_HEADERS([config.h])
AM_MAINTAINER_MODE

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_CONFIG_MACRO_DIR([m4])


dnl ***************************************************************************
dnl Internatinalization
dnl ***************************************************************************
GETTEXT_PACKAGE=alarm-clock
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [GETTEXT package name])
AM_GLIB_GNU_GETTEXT
IT_PROG_INTLTOOL([0.35.0])



AM_PROG_LIBTOOL


AC_ARG_ENABLE([appindicator],
    AS_HELP_STRING([--enable-appindicator], [Enable appindicator support for Ubuntu]))

AS_IF([test "x$enable_appindicator" == "xyes"], [
	PKG_CHECK_MODULES(ALARMCLOCK, [gtk+-2.0 >= 2.8 glib-2.0 gthread-2.0 gstreamer-0.10 libnotify unique-1.0 appindicator-0.1])
	AC_DEFINE([APPINDICATOR], [], [Use indicator applet instead of system tray])
])

AS_IF([test "x$enable_appindicator" != "xyes"], [
	PKG_CHECK_MODULES(ALARMCLOCK, [gtk+-2.0 >= 2.8 glib-2.0 gthread-2.0 gstreamer-0.10 libnotify unique-1.0])
])



AC_OUTPUT([ po/Makefile.in
Makefile
src/Makefile
])
