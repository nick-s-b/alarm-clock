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
 
#include "date.h"
#include "modify_alarm.h"
#include <main_window.h>
void
single_day_prefs(void)
{
	GtkWidget *prefs = GTK_WIDGET (gtk_builder_get_object (gxml, "single_day_prefs"));
	
	gtk_widget_show(GTK_WIDGET(prefs));
	
}

void
date_selected(void)
{
	GtkWidget *prefs = GTK_WIDGET (gtk_builder_get_object (gxml, "single_day_prefs"));
	GtkWidget *cal = GTK_WIDGET (gtk_builder_get_object (gxml, "calendar1"));
	gulong current_date = g_key_file_get_double(current_key, untitled_name, "DateTime", NULL);
	gulong new_date;
	guint year, month, day;
	struct tm *timeinfo;
	timeinfo = localtime((const time_t*)&current_date);
	
	gtk_calendar_get_date(GTK_CALENDAR(cal), &year, &month, &day);

	timeinfo->tm_year = year - 1900;
	timeinfo->tm_mon = month;
	timeinfo->tm_mday = day;

	new_date = mktime(timeinfo);

	g_key_file_set_double(current_key, untitled_name, "DateTime", new_date);
	gtk_widget_hide(prefs);
}
