## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = src po

alarmclockdocdir = ${prefix}/share/doc/alarm-clock
DesktopFiledir = ${prefix}/share/applications
iconsdir = ${prefix}/share/icons

alarmclockdoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS

DesktopFile_DATA = \
	alarm-clock.desktop

EXTRA_DIST = config.rpath m4/ChangeLog  $(alarmclockdoc_DATA) \
	$(DesktopFile_DATA)  $(icons_DATA)

# Copy all the spec files. Of cource, only one is actually used.
dist-hook:
	for specfile in *.spec; do \
		if test -f $$specfile; then \
			cp -p $$specfile $(distdir); \
		fi \
	done


ACLOCAL_AMFLAGS = -I m4

icons_DATA = \
	src/gfx/alarm-clock.png \
	src/gfx/indicator-alarm-clock.svg \
	src/gfx/indicator-alarm-caution.svg
