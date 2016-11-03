## Process this file with automake to produce Makefile.in

## Created by Anjuta

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(prefix)/share/alarm-clock"\" \
	$(ALARMCLOCK_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = alarm-clock

alarm_clock_SOURCES = \
	main.c      \
	configuration.c      \
	configuration.h      \
	main_list.c      \
	main_list.h      \
	main_window.c      \
	main_window.h      \
	translate.h      \
	modify_alarm.c      \
	modify_alarm.h      \
	common.c      \
	version.h      \
	common.h      \
	status_icon.c      \
	status_icon.h      \
	alarm_thread.c      \
	alarm_thread.h      \
	alarm_runner.c      \
	alarm_runner.h      \
	date.c      \
	date.h      \
	templates.c      \
	templates.h      \
	counters.c      \
	counters.h      \
	preferences.c      \
	schedule.c      \
	schedule.h      \
	export.c      

alarm_clock_LDFLAGS = \
	-export-dynamic

alarm_clock_LDADD = $(ALARMCLOCK_LIBS)

EXTRA_DIST = $(Data_DATA)\
	$(desktop_file_DATA) \
	$(icons_DATA)
	

Data_DATA = \
	sounds/ring.wav\
	gfx/alarm-caution.svg \
	gfx/alarm-clock.svg \
	interface/alarm-clock.ui

Datadir = \
	$(pkgdatadir)

