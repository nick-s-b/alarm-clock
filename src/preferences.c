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
#include <glib/gstdio.h>

#include "configuration.h"
#include "main_window.h"
#include "main_list.h"
#include "modify_alarm.h"
#include "alarm_thread.h"

void
show_preferences(void)
{
	GtkWidget *prefs_dialog = GTK_WIDGET (gtk_builder_get_object (gxml, "preferences_dialog"));
	GtkWidget *start_automatically = GTK_WIDGET (gtk_builder_get_object (gxml, "start_automatically"));
	GtkWidget *start_minimized = GTK_WIDGET (gtk_builder_get_object (gxml, "start_minimized"));
	GtkWidget *radio_12 = GTK_WIDGET (gtk_builder_get_object (gxml, "radio_12"));
	GtkWidget *radio_24 = GTK_WIDGET (gtk_builder_get_object (gxml, "radio_24"));
	gchar *keydata, *desktop_file, *autostart_file;
	gboolean buffer;
	gchar *auto_path = g_strdup_printf("%s/.config/autostart", g_getenv("HOME"));


	GKeyFile *config_key = g_key_file_new();

	g_key_file_load_from_file(config_key, config_global, G_KEY_FILE_NONE, 0);

	if (!g_file_test(auto_path, G_FILE_TEST_IS_DIR))
	{
		g_mkdir_with_parents(auto_path, S_IRWXU);
	}

	g_free(auto_path);

	autostart_file = g_strdup_printf("%s/.config/autostart/alarm-clock.desktop", g_getenv("HOME"));


	if (g_key_file_get_boolean(config_key, "Global", "StartMinimized", NULL))
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(start_minimized), TRUE);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(start_minimized), FALSE);


	if (g_file_test (autostart_file, G_FILE_TEST_EXISTS))
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(start_automatically), TRUE);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(start_automatically), FALSE);


	if (g_key_file_get_integer(config_key, "Global", "TimeType", NULL) == 12)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_12), TRUE);
	else if (g_key_file_get_integer(config_key, "Global", "TimeType", NULL) == 24)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_24), TRUE);
	else
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_12), FALSE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_24), FALSE);
	}



	gtk_dialog_run(GTK_DIALOG(prefs_dialog));

	buffer = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(start_minimized));

	g_key_file_set_boolean(config_key, "Global", "StartMinimized", buffer);

	buffer = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(start_automatically));

	if (buffer)
	{
		desktop_file = g_strdup("[Desktop Entry]\nType=Application\nEncoding=UTF-8\nVersion=1.0\nName=Alarm Clock\nExec=alarm-clock\nX-GNOME-Autostart-enabled=true\n");
		g_file_set_contents(autostart_file, desktop_file, -1, NULL);
		g_free(desktop_file);
	}
	else
	{
		g_remove(autostart_file);
	}




	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_12)))
		g_key_file_set_integer(config_key, "Global", "TimeType", 12);
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_24)))
		g_key_file_set_integer(config_key, "Global", "TimeType", 24);




	keydata = g_key_file_to_data(config_key, NULL, NULL);

	g_file_set_contents(config_global, keydata, -1, NULL);



	gtk_widget_hide(GTK_WIDGET(prefs_dialog));

	set_time_format();
	update_list_entries();
	update_alarm_thread(FALSE);
	change_selection();

	g_free(autostart_file);
	g_free(keydata);
	g_key_file_free(config_key);

}
