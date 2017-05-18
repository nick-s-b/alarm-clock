/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */
#include <config.h>
#include "alarm_thread.h"
#include "configuration.h"
#include "main_window.h"
#include "main_list.h"
#include "alarm_runner.h"
#include "translate.h"
#include "modify_alarm.h"
#include "status_icon.h"
#include <time.h>

gchar *format_time(gint usecs)
{
	gchar *tleft, *ghours, *gminutes, *gseconds;
	gint int_buf;
	gint days = usecs / 86400;
	int_buf = days * 86400;
	gint hours = (usecs - int_buf) / 3600;
	int_buf = int_buf + hours * 3600;
	gint minutes = (usecs - int_buf) / 60;
	int_buf = int_buf + minutes * 60;
	gint seconds = (usecs - int_buf);

	if (hours < 10)
		ghours = g_strdup_printf("0%i", hours);
	else
		ghours = g_strdup_printf("%i", hours);

	if (minutes < 10)
		gminutes = g_strdup_printf("0%i", minutes);
	else
		gminutes = g_strdup_printf("%i", minutes);

	if (seconds < 10)
		gseconds = g_strdup_printf("0%i", seconds);
	else
		gseconds = g_strdup_printf("%i", seconds);

	if (days < 1) {
		tleft = g_strdup_printf("%s:%s:%s", ghours, gminutes, gseconds);
	} else if (days < 365) {
		tleft =
		    g_strdup_printf("%id %s:%s:%s", days, ghours, gminutes,
				    gseconds);
	} else {
		gint years = days / 365;
		days = days % 365;
		tleft =
		    g_strdup_printf("%iy %id %s:%s:%s", years, days, ghours,
				    gminutes, gseconds);
	}

	g_free(ghours);
	g_free(gminutes);
	g_free(gseconds);

	return tleft;
}

gboolean is_date_excluded(gchar * alarm_name, gint day, gint month)
{
	gchar *buffer, **split, **date_split;
	gint d, m, count = 0;

	buffer =
	    g_key_file_get_string(loaded_alarms, alarm_name,
				  "ScheduleDateExclude", NULL);
	if (buffer == NULL)
		return FALSE;

	split = g_strsplit(buffer, ";", -1);
	g_free(buffer);
	while (TRUE) {
		if (split[count] == NULL)
			break;
		date_split = g_strsplit(split[count], "/", 2);

		d = g_ascii_strtoll(date_split[0], NULL, 10);
		m = g_ascii_strtoll(date_split[1], NULL, 10);

		if (d == day && m == month) {
			g_strfreev(split);
			g_strfreev(date_split);
			return TRUE;
		}

		g_strfreev(date_split);
		count++;
	}

	g_strfreev(split);

	return FALSE;
}

gulong check_date(gint month, gint day, gint hour, gint minute, gint year)
{
	time_t rawtime;
	time(&rawtime);
	struct tm *mytime;
	mytime = localtime(&rawtime);

	mytime->tm_mon = month - 1;
	mytime->tm_mday = day;
	mytime->tm_hour = hour;
	mytime->tm_min = minute;
	mytime->tm_year = year;
	mytime->tm_sec = 0;

	return mktime(mytime);
}

struct tm *convert_back(struct tm *convert)
{
	gulong conv = mktime(convert);

	struct tm *mytime;
	mytime = localtime((const time_t *)&conv);

	return convert;
}

gboolean check_date_excluded(gchar ** ex, gint month, gint day)
{
	gint count = 0;

	while (TRUE) {
		if (ex[count] == NULL)
			break;
		gchar *buffer = g_strdup_printf("%i/%i", day, month + 1);
		if (g_strcmp0(ex[count], buffer) == 0) {
			g_free(buffer);
			return TRUE;
		}
		g_free(buffer);
		count++;
	}
	return FALSE;
}

gulong get_iso_date_from_scheduled(gchar * alarm_name)
{
	time_t rawtime;
	time(&rawtime);
	struct tm *timeinfo;

	gulong smallest_date = 0;
	gulong qdate = 0, alarm_time =
	    g_key_file_get_integer(loaded_alarms, alarm_name, "DateTime", NULL);
	gint count = 0, hour, minute, year;
	gchar *sweekdays =
	    g_key_file_get_string(loaded_alarms, alarm_name, "ScheduleWeekdays",
				  NULL);
	gchar *smonths =
	    g_key_file_get_string(loaded_alarms, alarm_name, "ScheduleMonths",
				  NULL);
	gchar *sinclude =
	    g_key_file_get_string(loaded_alarms, alarm_name,
				  "ScheduleDateInclude",
				  NULL);
	gchar *sexclude =
	    g_key_file_get_string(loaded_alarms, alarm_name,
				  "ScheduleDateExclude",
				  NULL);

	gchar **sinclude_split = g_strsplit(sinclude, ";", 0);
	gchar **sexclude_split = g_strsplit(sexclude, ";", 0);

	g_free(sinclude);
	g_free(sexclude);

	timeinfo = localtime((const time_t *)&alarm_time);

	hour = timeinfo->tm_hour;
	minute = timeinfo->tm_min;
	year = timeinfo->tm_year;
	timeinfo = localtime(&rawtime);

	while (TRUE) {
		if (sinclude_split[count] == NULL)
			break;
		gchar **split_date = g_strsplit(sinclude_split[count], "/", 0);

		gint day = g_ascii_strtoull(split_date[0], NULL, 10);
		gint month = g_ascii_strtoull(split_date[1], NULL, 10);

		g_strfreev(split_date);

		gulong date_got = check_date(month, day, hour, minute, year);

		if (smallest_date != 0 && smallest_date > date_got) {
			smallest_date = date_got;
		}
		if (smallest_date == 0) {
			smallest_date = date_got;
		}
		count++;
	}

	gchar **sweekdays_split = g_strsplit(sweekdays, ":", 0);
	gchar **smonths_split = g_strsplit(smonths, ":", 0);

	time(&rawtime);
	struct tm *mytime;
	mytime = localtime(&rawtime);

	mytime->tm_hour = hour;
	mytime->tm_min = minute;
	mytime->tm_sec = 0;

	while (TRUE) {
		gint wday = mytime->tm_wday - 1;

		if (wday == -1)
			wday = 6;

		if (g_strcmp0(smonths_split[mytime->tm_mon], "T") == 0
		    && g_strcmp0(sweekdays_split[wday], "T") == 0) {
			qdate = mktime(mytime);
			if (qdate > rawtime
			    && !check_date_excluded(sexclude_split,
						    mytime->tm_mon,
						    mytime->tm_mday))
				break;
		}

		mytime->tm_year = year;

		mytime->tm_mon = mytime->tm_mon;
		mytime->tm_hour = hour;
		mytime->tm_min = minute;
		mytime->tm_mday = mytime->tm_mday + 1;
		mytime->tm_sec = 0;

		mytime = convert_back(mytime);
		g_usleep(G_USEC_PER_SEC / 5);
	}

	g_strfreev(sinclude_split);
	g_strfreev(sexclude_split);
	g_strfreev(smonths_split);
	g_strfreev(sweekdays_split);
	g_free(sweekdays);
	g_free(smonths);

	if (qdate < smallest_date && smallest_date != 0)
		return qdate;
	if (smallest_date == 0)
		return qdate;
	else
		return smallest_date;
}

void move_to_missed(gchar * name)
{
	gint key = 0;
	gchar *buffer, **keys;
	GKeyFile *temp_key = g_key_file_new();

	g_key_file_load_from_file(temp_key, config_missed, G_KEY_FILE_NONE,
				  NULL);

	keys = g_key_file_get_keys(loaded_alarms, name, NULL, NULL);

	while (TRUE) {
		if (keys[key] == NULL)
			break;
		buffer =
		    g_key_file_get_string(loaded_alarms, name, keys[key], NULL);

		g_key_file_set_string(temp_key, name, keys[key], buffer);

		key++;
		g_free(buffer);
	}

	buffer = g_key_file_to_data(temp_key, NULL, NULL);

	g_file_set_contents(config_missed, buffer, -1, NULL);

	g_key_file_free(temp_key);
	g_free(buffer);
	g_strfreev(keys);

}

void manage_old_alarm(gchar * name)
{
	gchar *type =
	    g_key_file_get_string(loaded_alarms, name, "AlarmType", NULL);
	gboolean snooze =
	    g_key_file_get_boolean(loaded_alarms, name, "snoozed", NULL);
	gboolean single = TRUE;

	if (g_strcmp0(type, "Schedule") == 0)
		single = FALSE;
	if (snooze)
		snooze = FALSE;

	g_free(type);

	if (need_check_first) {
		move_to_missed(name);
		if (single) {
			remove_alarm(name);
			reload_alarms();
			update_list_entries();
		}
	} else {
		if (single && !snooze && !running_snooze) {
			g_usleep(G_USEC_PER_SEC);
			remove_alarm(name);
			reload_alarms();
			update_list_entries();
		}
	}
}

void update_alarm_thread(gboolean thread)
{
	if (blocked)
		return;
	blocked = TRUE;
	glong diff = 0;
	gchar *time_left, *alarm_type, **alarms;
	guint count;
	GtkTreeIter iter;
	gchar *name;
	gboolean snoozed;

	count = 0;
	gboolean first = TRUE;

	gulong alarm_time;
	GTimeVal cur_time;
	alarms = g_key_file_get_groups(loaded_alarms, NULL);
	while (TRUE) {
		if (alarms[count] == NULL) {
			break;
		}

		alarm_type =
		    g_key_file_get_string(loaded_alarms, alarms[count],
					  "AlarmType", NULL);
		snoozed =
		    g_key_file_get_boolean(loaded_alarms, alarms[count],
					   "Snoozed", NULL);

		if (g_strcmp0(alarm_type, "Single") == 0
		    || g_strcmp0(alarm_type, "Counter") == 0) {
			alarm_time =
			    g_key_file_get_double(loaded_alarms, alarms[count],
						  "DateTime", NULL);
			g_free(alarm_type);
		} else if (g_strcmp0(alarm_type, "Schedule") == 0) {
			alarm_time = get_iso_date_from_scheduled(alarms[count]);
			g_free(alarm_type);
		} else {
			break;
		}

		g_get_current_time(&cur_time);

		diff = alarm_time - cur_time.tv_sec;

		gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store), &iter);

		while (TRUE) {
			gtk_tree_model_get(GTK_TREE_MODEL(store), &iter, 4,
					   &name, -1);
			if (g_strcmp0(name, alarms[count]) == 0) {
				g_free(name);
				break;
			}
			if (gtk_tree_model_iter_next
			    (GTK_TREE_MODEL(store), &iter) == FALSE) {
				g_free(name);
				break;
			}
			g_free(name);
		}

		if (diff < 1) {
			manage_old_alarm(alarms[count]);
		}

		if (diff > 1) {
			if (snoozed) {
				gchar *buf = format_time(diff);
				time_left = g_strdup_printf("<i>%s</i>", buf);
				if (thread)
					gdk_threads_enter();
				gtk_list_store_set(store, &iter, 5, time_left,
						   -1);
				if (thread)
					gdk_threads_leave();
				g_free(buf);
				g_free(time_left);
			} else {
				time_left = format_time(diff);
				if (thread)
					gdk_threads_enter();
				gtk_list_store_set(store, &iter, 5, time_left,
						   -1);
				if (thread)
					gdk_threads_leave();
				g_free(time_left);
			}

		}

		if (diff == 1) {
			gdk_threads_enter();
			run_alarm(alarms[count]);
			manage_old_alarm(alarms[count]);
			gdk_threads_leave();
			update_list_entries();
		}

		if (first) {
			first = FALSE;
			continue;
		} else {
			count++;
		}
	}
	g_strfreev(alarms);
	blocked = FALSE;
}

void check_current_birthdays(void)
{
	gchar *iso_date, *month, *day, *processed_date, *buffer;
	GString *names = g_string_new(NULL);
	gboolean got_birthday = FALSE, first_name = TRUE;
	GKeyFile *birthdays_key;
	gchar **birthdays;
	gint count = 0;
	time_t rawtime;
	struct tm *timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);

	if (timeinfo->tm_mon + 1 < 10) {
		month = g_strdup_printf("0%i", timeinfo->tm_mon + 1);
	} else {
		month = g_strdup_printf("%i", timeinfo->tm_mon + 1);
	}

	if (timeinfo->tm_mday < 10) {
		day = g_strdup_printf("0%i", timeinfo->tm_mday);
	} else {
		day = g_strdup_printf("%i", timeinfo->tm_mday);
	}

	iso_date =
	    g_strdup_printf("%i-%s-%s", timeinfo->tm_year + 1900, month, day);

	birthdays_key = g_key_file_new();
	g_key_file_load_from_file(birthdays_key, config_birthdays,
				  G_KEY_FILE_NONE, NULL);
	birthdays = g_key_file_get_groups(birthdays_key, NULL);

	while (TRUE) {
		if (birthdays[count] == NULL)
			break;
		processed_date =
		    g_strdup(g_key_file_get_string
			     (birthdays_key, birthdays[count], "Date", NULL));
		if (g_strcmp0(iso_date, processed_date) == 0) {
			got_birthday = TRUE;
			if (first_name) {
				g_string_append(names,
						g_key_file_get_string
						(birthdays_key,
						 birthdays[count], "Name",
						 NULL));
				first_name = FALSE;
			} else {
#ifndef APPINDICATOR
				g_string_append(names, "\n");
#endif
#ifdef APPINDICATOR
				g_string_append(names, ", ");
#endif
				g_string_append(names,
						g_key_file_get_string
						(birthdays_key,
						 birthdays[count], "Name",
						 NULL));
			}
		}
		g_free(processed_date);
		count++;
	}

#ifndef APPINDICATOR
	change_birthday_status(got_birthday);

	if (got_birthday) {
		buffer =
		    g_strdup_printf(_("Today's birthdays:\n\n%s"), names->str);
		set_status_tooltip(buffer);
		g_free(buffer);
	} else {
		set_status_tooltip(_("Alarm Clock"));
	}
#endif
#ifdef APPINDICATOR

	if (got_birthday) {
		buffer =
		    g_strdup_printf(_("Today's birthdays: %s"), names->str);
		change_birthday_status(TRUE, buffer);
		g_free(buffer);
	} else {
		change_birthday_status(FALSE, NULL);
	}
#endif
	g_free(iso_date);
	g_free(month);
	g_free(day);
	g_strfreev(birthdays);
	g_key_file_free(birthdays_key);
	g_string_free(names, TRUE);
}

void alarm_thread(void)
{
	blocked = FALSE;
	time_t rawtime;
	struct tm *timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	need_check_first = TRUE;
	gdk_threads_enter();
	check_current_birthdays();
	gdk_threads_leave();
	update_list_entries();

	while (TRUE) {
		if (timeinfo->tm_hour == 0 && timeinfo->tm_min == 0
		    && timeinfo->tm_sec == 0) {
			gdk_threads_enter();
			check_current_birthdays();
			gdk_threads_leave();
		}

		update_alarm_thread(TRUE);
		if (need_check_first) {
			gdk_threads_enter();
			check_missed_alarms();
			gdk_threads_leave();
		}
		g_usleep(G_USEC_PER_SEC);
		need_check_first = FALSE;

	}
}

void reload_alarms(void)
{
	if (loaded_alarms != NULL)
		g_key_file_free(loaded_alarms);

	loaded_alarms = g_key_file_new();
	g_key_file_load_from_file(loaded_alarms, config_alarms, G_KEY_FILE_NONE,
				  NULL);
}
