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

#include <gtk/gtk.h>
#include "main_window.h"
#include "translate.h"
#include "configuration.h"
#include "main_list.h"
#include "alarm_thread.h"

void
save_alarms_list(void)
{
	gchar *filename, *buffer;
	GtkWidget *parent = GTK_WIDGET (gtk_builder_get_object (gxml, "main_window"));
	GtkWidget *dialog = gtk_file_chooser_dialog_new(_("Save alarms list"),
	                                                GTK_WINDOW(parent),
	                                                GTK_FILE_CHOOSER_ACTION_SAVE,
	                                                "gtk-cancel", 2,
	                                                "gtk-save", 3, NULL);

	GtkFileFilter *all_filter = gtk_file_filter_new();
	GtkFileFilter *alarms_filter = gtk_file_filter_new();

	gtk_file_filter_set_name(all_filter, _("All files"));
	gtk_file_filter_add_pattern(all_filter, "*");
	gtk_file_filter_set_name(alarms_filter, _("Alarm clock alarms"));
	gtk_file_filter_add_pattern(alarms_filter, "*.alarms");

	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), all_filter);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), alarms_filter);
	
	gint response = gtk_dialog_run(GTK_DIALOG(dialog));

	if (response == 3)
	{
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		g_file_get_contents(config_alarms, &buffer, NULL, NULL);

		g_file_set_contents(filename, buffer, -1, NULL);

		g_free(filename);
		g_free(buffer);
		gtk_widget_destroy(GTK_WIDGET(dialog));
	}
	else
	{
		gtk_widget_destroy(GTK_WIDGET(dialog));
	}
}

void
open_alarms_list(void)
{
	gchar *filename, *buffer;
	GtkWidget *parent = GTK_WIDGET (gtk_builder_get_object (gxml, "main_window"));
	GtkWidget *dialog = gtk_file_chooser_dialog_new(_("Open alarms list"),
	                                                GTK_WINDOW(parent),
	                                                GTK_FILE_CHOOSER_ACTION_SAVE,
	                                                "gtk-cancel", 2,
	                                                "gtk-open", 3, NULL);

	GtkFileFilter *all_filter = gtk_file_filter_new();
	GtkFileFilter *alarms_filter = gtk_file_filter_new();

	gtk_file_filter_set_name(all_filter, _("All files"));
	gtk_file_filter_add_pattern(all_filter, "*");
	gtk_file_filter_set_name(alarms_filter, _("Alarm clock alarms"));
	gtk_file_filter_add_pattern(alarms_filter, "*.alarms");

	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), all_filter);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), alarms_filter);

	
	gint response = gtk_dialog_run(GTK_DIALOG(dialog));

	if (response == 3)
	{
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		g_file_get_contents(filename, &buffer, NULL, NULL);

		g_file_set_contents(config_alarms, buffer, -1, NULL);

		g_free(filename);
		g_free(buffer);
		gtk_widget_destroy(GTK_WIDGET(dialog));
		update_list_entries ();
		reload_alarms ();
		update_alarm_thread (TRUE);
	}
	else
	{
		gtk_widget_destroy(GTK_WIDGET(dialog));
	}
}
