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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <config.h>

#include <gtk/gtk.h>
#include <gst/gst.h>
#include <libnotify/notify.h>
#include <unique/unique.h>

#include "templates.h"
#include "main_window.h"
#include "main_list.h"

void hide_me(GtkWidget * widget)
{
	GtkWidget *mwindow =
	    GTK_WIDGET(gtk_builder_get_object(gxml, "main_window"));
	gtk_window_get_position(GTK_WINDOW(mwindow), &current_x, &current_y);
	current_groupname = 0;
	gtk_widget_hide(GTK_WIDGET(widget));
}

/*
 * Standard gettext macros.
 */
#ifdef ENABLE_NLS
#include <libintl.h>
#undef _
#define _(String) dgettext (PACKAGE, String)
#ifdef gettext_noop
#define N_(String) gettext_noop (String)
#else
#define N_(String) (String)
#endif
#else
#define textdomain(String) (String)
#define gettext(String) (String)
#define dgettext(Domain,Message) (Message)
#define dcgettext(Domain,Message,Type) (Message)
#define bindtextdomain(Domain,Directory) (Domain)
#define _(String) (String)
#define N_(String) (String)
#endif

#include "main_window.h"
#include "configuration.h"
#include "status_icon.h"
#include "alarm_thread.h"
#include "common.h"

static UniqueResponse
message_received_cb(UniqueApp * app,
		    UniqueCommand command,
		    UniqueMessageData * message,
		    guint time_, gpointer user_data)
{
	UniqueResponse res;

	if (command == UNIQUE_ACTIVATE) {
		gtk_window_set_screen(GTK_WINDOW(main_window),
				      unique_message_data_get_screen(message));
		gtk_window_present_with_time(GTK_WINDOW(main_window), time_);
		res = UNIQUE_RESPONSE_OK;
		return res;
	}
	return UNIQUE_RESPONSE_FAIL;
}

int main(int argc, char *argv[])
{

#ifdef ENABLE_NLS
	bindtextdomain(GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
	textdomain(GETTEXT_PACKAGE);
#endif

	gtk_set_locale();
	gtk_init(&argc, &argv);
	GKeyFile *key = g_key_file_new();

	UniqueApp *app;
	app = unique_app_new("org.gnome.AlarmClock", NULL);

	if (unique_app_is_running(app)) {
		UniqueCommand command = UNIQUE_ACTIVATE;
		UniqueResponse response;

		response = unique_app_send_message(app, command, NULL);

		return 0;
	}

	gdk_threads_init();

	gst_init(&argc, &argv);

	g_set_application_name(_("Alarm Clock"));

	GError *error = NULL;

	gxml = gtk_builder_new();
	if (!gtk_builder_add_from_file(gxml, GTK_BUILDER_FILE, &error)) {
		g_print("Cannot load GTK Builder file: %s\n", error->message);
		g_error_free(error);
		exit(1);
	}

	if (error)
		g_error_free(error);

	initialize_config();

	g_key_file_load_from_file(key, config_global, G_KEY_FILE_NONE, NULL);

	GtkWidget *main_window = create_main_window();

	create_status_icon();

	reload_alarms();

	g_thread_try_new("alarm thread", (GThreadFunc) alarm_thread, NULL,
			 NULL);

	notify_init(_("Alarm Clock"));

	gtk_window_get_position(GTK_WINDOW(main_window), &current_x,
				&current_y);

	g_signal_connect(app, "message-received",
			 G_CALLBACK(message_received_cb), NULL);

	if (!g_key_file_get_boolean(key, "Global", "StartMinimized", NULL))
		gtk_widget_show_all(main_window);

	GtkWidget *stop_sound =
	    GTK_WIDGET(gtk_builder_get_object
		       (gxml, "stop_the_sound_toolbutton"));
	gtk_widget_hide(GTK_WIDGET(stop_sound));

	change_selection();

	gtk_main();

	g_key_file_free(key);
	g_object_unref(app);

	return 0;
}
