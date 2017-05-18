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

#include "alarm_runner.h"
#include "alarm_thread.h"
#include "common.h"
#include "status_icon.h"
#include "translate.h"
#include "main_window.h"
#include "main_list.h"
#include "modify_alarm.h"

#include <config.h>

#include <libnotify/notify.h>
#include <gst/gst.h>

void volume_fade_thread(gchar * name)
{
	g_object_set(G_OBJECT(play), "volume", 0.0, NULL);
	gfloat current_volume = 0.0;
	gboolean repeat =
	    g_key_file_get_boolean(loaded_alarms, name, "RepeatSoundEnabled",
				   NULL);
	gint repeat_value =
	    g_key_file_get_integer(loaded_alarms, name, "RepeatSoundValue",
				   NULL);
	gfloat addition = 0.0;
	if (repeat)
		addition =
		    1.1 / (count_play - current_time.tv_sec -
			   repeat_value * 60);
	else
		addition = 1.1 / (count_play - current_time.tv_sec);

	while (fade_enabled) {
		g_usleep(G_USEC_PER_SEC / 100);
		g_get_current_time(&current_time);
		current_volume += addition / 100;
		g_object_set(G_OBJECT(play), "volume", current_volume, NULL);
		if (current_volume > 1)
			break;
	}

	g_free(name);
}

static gboolean
constant_bus_callback(GstBus * bus, GstMessage * message, gpointer data)
{
	g_get_current_time(&current_time);

	if (GST_MESSAGE_TYPE(message) == GST_MESSAGE_EOS) {
		if (count_play < current_time.tv_sec) {
			GtkWidget *stop_sound =
			    GTK_WIDGET(gtk_builder_get_object
				       (gxml, "stop_the_sound_toolbutton"));
			gtk_widget_hide(GTK_WIDGET(stop_sound));
			gst_element_set_state(play, GST_STATE_NULL);
			gst_object_unref(bus);
			gst_object_unref(play);
			gst_message_unref(message);
			sound_ref--;
			return TRUE;
		}
		gst_element_set_state(play, GST_STATE_NULL);
		gst_element_set_state(play, GST_STATE_PLAYING);
	}

	return TRUE;
}

void start_sound(gpointer user_data)
{
	if (sound_ref > 0) {
		gdk_threads_enter();
		GtkWidget *dialog =
		    gtk_message_dialog_new_with_markup(GTK_WINDOW(main_window),
						       GTK_DIALOG_DESTROY_WITH_PARENT,
						       GTK_MESSAGE_WARNING,
						       GTK_BUTTONS_CLOSE, NULL);

		gtk_message_dialog_set_markup(GTK_MESSAGE_DIALOG(dialog),
					      _("<b>Too many sounds!</b>"));
		gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG
							 (dialog),
							 _
							 ("Cannot play more than 1 sound at once."));

		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
		gdk_threads_leave();
		return;
	}

	fade_enabled = FALSE;

	gchar *name = (gchar *) user_data, *soundfile;

	if (g_key_file_get_boolean
	    (loaded_alarms, name, "CustomSoundEnabled", NULL)) {
		soundfile =
		    g_strdup_printf("file://%s",
				    g_key_file_get_string(loaded_alarms, name,
							  "CustomSoundFile",
							  NULL));
	} else {
		soundfile =
		    g_strdup_printf("file://%s", ALARM_CLOCK_DEFAULT_SOUND);
	}

	gchar **split = g_strsplit(soundfile, "//", -1);
	if (!g_file_test(split[1], G_FILE_TEST_EXISTS)) {
		gdk_threads_enter();
		gchar *markup =
		    g_strdup_printf(_
				    ("Cannot play sound because the sound file <b>%s</b> was not found."),
split[1]);
		GtkWidget *dialog =
		    gtk_message_dialog_new_with_markup(GTK_WINDOW(main_window),
						       GTK_DIALOG_DESTROY_WITH_PARENT,
						       GTK_MESSAGE_WARNING,
						       GTK_BUTTONS_CLOSE, NULL);

		gtk_message_dialog_set_markup(GTK_MESSAGE_DIALOG(dialog),
					      _
					      ("<b>Cannot find sound file!</b>"));
		gtk_message_dialog_format_secondary_markup(GTK_MESSAGE_DIALOG
							   (dialog), markup,
							   NULL);

		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);

		g_free(markup);
		g_strfreev(split);
		g_free(soundfile);
		g_free(name);
		gdk_threads_leave();
		return;
	}

	g_strfreev(split);

	g_get_current_time(&current_time);

	sound_ref++;

	play = gst_element_factory_make("playbin", "playbin");
	g_object_set(G_OBJECT(play), "uri", soundfile, NULL);

	bus = gst_pipeline_get_bus(GST_PIPELINE(play));

	if (g_key_file_get_boolean
	    (loaded_alarms, name, "ConstantSoundEnabled", NULL)) {
		count_play =
		    g_key_file_get_integer(loaded_alarms, name,
					   "ConstantSoundValue", NULL);
		count_play = count_play * 60 + current_time.tv_sec;
		gboolean repeat =
		    g_key_file_get_boolean(loaded_alarms, name,
					   "RepeatSoundEnabled",
					   NULL);
		gint repeat_value =
		    g_key_file_get_integer(loaded_alarms, name,
					   "RepeatSoundValue",
					   NULL);

		if (repeat) {
			count_play = count_play + (60 * repeat_value);
		}

		gst_bus_add_watch(bus, constant_bus_callback, NULL);
		gst_element_set_state(play, GST_STATE_PLAYING);
	}

	if (g_key_file_get_boolean
	    (loaded_alarms, name, "FadeSoundEnabled", NULL)) {
		fade_enabled = TRUE;
		count_play =
		    g_key_file_get_integer(loaded_alarms, name,
					   "FadeSoundValue", NULL);
		count_play = count_play * 60 + current_time.tv_sec;
		gboolean repeat =
		    g_key_file_get_boolean(loaded_alarms, name,
					   "RepeatSoundEnabled",
					   NULL);
		gint repeat_value =
		    g_key_file_get_integer(loaded_alarms, name,
					   "RepeatSoundValue",
					   NULL);

		if (repeat) {
			count_play = count_play + (60 * repeat_value);
		}

		g_thread_try_new("volume fade",
				 (GThreadFunc) volume_fade_thread,
				 g_strdup(name), NULL);

		gst_bus_add_watch(bus, constant_bus_callback, NULL);
		gst_element_set_state(play, GST_STATE_PLAYING);
	}

	g_free(name);
	g_free(soundfile);

	gdk_threads_enter();
	GtkWidget *stop_sound =
	    GTK_WIDGET(gtk_builder_get_object
		       (gxml, "stop_the_sound_toolbutton"));
	gtk_widget_show(GTK_WIDGET(stop_sound));
	gdk_threads_leave();
}

void show_popup(gchar * name)
{
	gint type, timeout;
	gchar *title, *text;

	type =
	    g_key_file_get_integer(loaded_alarms, name, "PassivePopupType",
				   NULL);
	timeout =
	    g_key_file_get_integer(loaded_alarms, name, "PassivePopupTimeout",
				   NULL);
	title =
	    g_key_file_get_string(loaded_alarms, name, "PassivePopupTitle",
				  NULL);
	text =
	    g_key_file_get_string(loaded_alarms, name, "PassivePopupText",
				  NULL);

	if (notify != NULL)
		notify_notification_close(notify, NULL);
#ifndef APPINDICATOR
#if NOTIFY_CHECK_VERSION (0, 7, 0)
	notify = notify_notification_new(title, text, NULL);
	notify = notify_notification_new(title, text, NULL);
#else
	notify =
	    notify_notification_new_with_status_icon(title, text, NULL,
						     status_icon);
#endif
	notify = notify_notification_new(title, text, NULL);
#endif
	if (notify == NULL)
		g_print("BAD");
	notify_notification_set_urgency(notify, type);
	notify_notification_set_timeout(notify, timeout * 1000);
	notify_notification_show(notify, NULL);
}

void close_main_dialog(GtkButton * button, gpointer user_data)
{
	GtkWidget *dialog = (GtkWidget *) user_data;
	running_snooze = FALSE;
	gtk_widget_destroy(dialog);
}

void enable_snooze(GtkWidget * button, gpointer user_data)
{
	GTimeVal before_time;
	GtkWidget *dialog = gtk_widget_get_toplevel(GTK_WIDGET(button));
	gchar *alarm_name = (gchar *) user_data;
	gint snooze_time =
	    g_key_file_get_integer(loaded_alarms, alarm_name, "SnoozeTime",
				   NULL);
	gchar *date =
	    g_key_file_get_string(loaded_alarms, alarm_name, "DateTime", NULL);
	gchar *new_date;

	g_time_val_from_iso8601(date, &before_time);
	g_time_val_add(&before_time, G_USEC_PER_SEC * snooze_time * 60);

	new_date = g_time_val_to_iso8601(&before_time);

	g_key_file_set_string(loaded_alarms, alarm_name, "DateTime", new_date);
	g_key_file_set_string(loaded_alarms, alarm_name, "SnoozedOldTime",
			      new_date);
	g_key_file_set_boolean(loaded_alarms, alarm_name, "Snoozed", TRUE);

	g_free(new_date);
	g_free(date);

	gtk_widget_destroy(GTK_WIDGET(dialog));
	update_list_entries();
	update_alarm_thread(FALSE);
}

void show_dialog(gchar * name)
{
	gchar *gbuffer, *to_enter;
	GtkWidget *dialog = gtk_dialog_new();
	GtkWidget *event = gtk_event_box_new();
	GtkWidget *label = gtk_label_new(NULL);
	GtkWidget *vbox = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
	GtkWidget *close_button =
	    gtk_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_CLOSE,
				  GTK_RESPONSE_CLOSE);

	g_signal_connect(G_OBJECT(close_button), "clicked",
			 G_CALLBACK(close_main_dialog), dialog);

	gboolean snooze =
	    g_key_file_get_boolean(loaded_alarms, name, "SnoozeEnabled", NULL);

	if (snooze) {
		running_snooze = TRUE;
		GtkWidget *snooze_button =
		    gtk_dialog_add_button(GTK_DIALOG(dialog), _("Snooze"),
					  GTK_RESPONSE_OK);
		g_signal_connect(G_OBJECT(snooze_button), "clicked",
				 G_CALLBACK(enable_snooze), g_strdup(name));
	}

	gbuffer =
	    g_key_file_get_string(loaded_alarms, name, "DialogTitle", NULL);
	gtk_window_set_title(GTK_WINDOW(dialog), gbuffer);

	g_free(gbuffer);

	GdkColor background_color;

	gbuffer =
	    g_key_file_get_string(loaded_alarms, name, "DialogBackgroundColor",
				  NULL);
	gdk_color_parse(gbuffer, &background_color);
	gtk_widget_modify_bg(GTK_WIDGET(event), GTK_STATE_NORMAL,
			     &background_color);
	g_free(gbuffer);

	gbuffer =
	    g_key_file_get_string(loaded_alarms, name, "DialogMessage", NULL);

	if (g_key_file_get_integer(loaded_alarms, name, "DialogTextSize", NULL)
	    == 0)
		to_enter = g_strdup_printf("<small>%s</small>", gbuffer);

	else if (g_key_file_get_integer
		 (loaded_alarms, name, "DialogTextSize", NULL)
		 == 1)
		to_enter = g_strdup_printf("%s", gbuffer);

	else if (g_key_file_get_integer
		 (loaded_alarms, name, "DialogTextSize", NULL)
		 == 2)
		to_enter = g_strdup_printf("<big>%s</big>", gbuffer);

	else if (g_key_file_get_integer
		 (loaded_alarms, name, "DialogTextSize", NULL)
		 == 3)
		to_enter = g_strdup_printf("<big><big>%s</big></big>", gbuffer);

	else
		to_enter = g_strdup_printf("%s", gbuffer);

	gtk_label_set_markup(GTK_LABEL(label), to_enter);

	gtk_container_add(GTK_CONTAINER(vbox), event);
	gtk_container_add(GTK_CONTAINER(event), label);

	gbuffer =
	    g_key_file_get_string(loaded_alarms, name, "DialogTextColor", NULL);

	GdkColor text_color;

	gdk_color_parse(gbuffer, &text_color);

	gtk_widget_modify_fg(GTK_WIDGET(label), GTK_STATE_NORMAL, &text_color);

	gtk_window_resize(GTK_WINDOW(dialog), 300, 200);

	g_free(gbuffer);

	gtk_widget_show_all(GTK_WIDGET(dialog));
}

void show_command(gchar * name)
{
	gchar *gbuffer = "", *command = "";
	gboolean bool_buffer;

	bool_buffer =
	    g_key_file_get_boolean(loaded_alarms, name, "RunInTerminal", NULL);
	gbuffer =
	    g_key_file_get_string(loaded_alarms, name, "RunCommandContent",
				  NULL);
	if (bool_buffer) {
		command =
		    g_strdup_printf
		    ("%s; read -p \"%s\"; rm -fr /tmp/ac_script.sh", gbuffer,
		     _("Press any key to continue..."));
		g_file_set_contents("/tmp/ac_script.sh", command, -1, NULL);
		g_spawn_command_line_async
		    ("gnome-terminal --hide-menubar -x sh /tmp/ac_script.sh",
		     NULL);
	} else {
		command =
		    g_strdup_printf("%s; rm -fr /tmp/ac_script.sh", gbuffer);
		g_file_set_contents("/tmp/ac_script.sh", command, -1, NULL);
		g_spawn_command_line_async("sh /tmp/ac_script.sh", NULL);
	}

	g_free(command);
	g_free(gbuffer);

}

void show_standard_action(gchar * name)
{
	gchar *command;
	if (g_key_file_get_boolean
	    (loaded_alarms, name, "ActionFirstSelected", NULL)) {
		g_spawn_command_line_async("gnome-screensaver-command -l",
					   NULL);
	}
/* TODO is this really the ideal way to handle? */
	else {
		command =
		    g_strdup
		    ("gnome-screensaver-command -a; xset dpms force off; rm -fr /tmp/ac_script.sh");
		g_file_set_contents("/tmp/ac_script.sh", command, -1, NULL);
		g_spawn_command_line_async("sh /tmp/ac_script.sh", NULL);
		g_free(command);
	}

}

void run_alarm(gchar * name)
{
	gchar *alarm_name;
	gchar *alarm_code = g_strdup(name);

	alarm_name = g_key_file_get_string(loaded_alarms, name, "Title", NULL);

	if (g_key_file_get_boolean(loaded_alarms, name, "SoundEnabled", NULL))
		g_thread_try_new("start sound", (GThreadFunc) start_sound,
				 alarm_code, NULL);

	if (g_key_file_get_boolean
	    (loaded_alarms, name, "PassivePopupEnabled", NULL)) {
		show_popup(alarm_code);
	}

	if (g_key_file_get_boolean
	    (loaded_alarms, name, "DialogWindowEnabled", NULL)) {
		show_dialog(alarm_code);
	}

	if (g_key_file_get_boolean(loaded_alarms, name, "RunCommand", NULL)) {
		show_command(alarm_code);
	}

	if (g_key_file_get_boolean(loaded_alarms, name, "ActionEnabled", NULL)) {
		show_standard_action(alarm_code);
	}
	g_free(alarm_name);

}

void stop_the_sound(GtkWidget * widget)
{
	gst_element_set_state(play, GST_STATE_NULL);
	gst_object_unref(play);
	gst_object_unref(bus);
	fade_enabled = FALSE;
	sound_ref = 0;
	count_play = 0;
	gtk_widget_hide(GTK_WIDGET(widget));
}
