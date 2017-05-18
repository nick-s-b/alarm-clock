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

#include "main_window.h"
#include "configuration.h"
#include "translate.h"
#include "counters.h"
#include "main_list.h"
#include "alarm_thread.h"

#include <time.h>
#include <sys/time.h>

void fill_default_for_counter(gchar * alarm_name, gulong date)
{
	g_key_file_set_double(key, alarm_name, "DateTime", date);
	g_key_file_set_string(key, alarm_name, "Title", _("Untitled"));
	g_key_file_set_string(key, alarm_name, "AlarmType", "Single");
	g_key_file_set_boolean(key, alarm_name, "SoundEnabled", TRUE);
	g_key_file_set_boolean(key, alarm_name, "CustomSoundEnabled", FALSE);
	g_key_file_set_string(key, alarm_name, "CustomSoundFile", "[DEFAULT]");
	g_key_file_set_boolean(key, alarm_name, "ConstantSoundEnabled", TRUE);
	g_key_file_set_integer(key, alarm_name, "ConstantSoundValue", 5);
	g_key_file_set_boolean(key, alarm_name, "FadeSoundEnabled", FALSE);
	g_key_file_set_integer(key, alarm_name, "FadeSoundValue", 5);
	g_key_file_set_boolean(key, alarm_name, "RepeatSoundEnabled", FALSE);
	g_key_file_set_integer(key, alarm_name, "RepeatSoundValue", 5);
	g_key_file_set_boolean(key, alarm_name, "PassivePopupEnabled", TRUE);
	g_key_file_set_integer(key, alarm_name, "PassivePopupType", 0);
	g_key_file_set_integer(key, alarm_name, "PassivePopupTimeout", 30);
	g_key_file_set_string(key, alarm_name, "PassivePopupTitle", _("Popup"));
	g_key_file_set_string(key, alarm_name, "PassivePopupText",
			      _("Short popup"));
	g_key_file_set_boolean(key, alarm_name, "DialogWindowEnabled", FALSE);
	g_key_file_set_string(key, alarm_name, "DialogTextColor",
			      "#000000000000");
	g_key_file_set_string(key, alarm_name, "DialogBackgroundColor",
			      "#ffffffffffff");
	g_key_file_set_integer(key, alarm_name, "DialogTextSize", 1);
	g_key_file_set_string(key, alarm_name, "DialogTitle",
			      _("Dialog window"));
	g_key_file_set_string(key, alarm_name, "DialogMessage",
			      _("This is a small message"));
	g_key_file_set_boolean(key, alarm_name, "SnoozeEnabled", FALSE);
	g_key_file_set_integer(key, alarm_name, "SnoozeTime", 5);
	g_key_file_set_boolean(key, alarm_name, "RunCommand", FALSE);
	g_key_file_set_string(key, alarm_name, "RunCommandContent", "");
	g_key_file_set_boolean(key, alarm_name, "RunInTerminal", TRUE);
	g_key_file_set_boolean(key, alarm_name, "ActionEnabled", FALSE);
	g_key_file_set_boolean(key, alarm_name, "ActionFirstSelected", TRUE);
}

void add_counter_dialog(void)
{
	GtkWidget *dialog =
	    GTK_WIDGET(gtk_builder_get_object(gxml, "counter_editor_dialog"));
	GtkWidget *minute_spin =
	    GTK_WIDGET(gtk_builder_get_object(gxml, "minutes_spin"));
	GtkWidget *title_entry =
	    GTK_WIDGET(gtk_builder_get_object(gxml, "title_entr"));
	GtkWidget *counter_text =
	    GTK_WIDGET(gtk_builder_get_object(gxml, "counter_text"));
	GtkWidget *standard_sound_radio =
	    GTK_WIDGET(gtk_builder_get_object(gxml, "standard_sound_radio"));
	GtkWidget *soundfile_chooser =
	    GTK_WIDGET(gtk_builder_get_object(gxml, "soundfile_chooser"));
	GtkTextBuffer *buf =
	    gtk_text_view_get_buffer(GTK_TEXT_VIEW(counter_text));

	GtkFileFilter *sound_filter = gtk_file_filter_new();
	GtkFileFilter *all_filter = gtk_file_filter_new();

	gtk_file_filter_add_mime_type(GTK_FILE_FILTER(sound_filter), "audio/*");
	gtk_file_filter_set_name(GTK_FILE_FILTER(sound_filter),
				 _("Sound files"));

	gtk_file_filter_add_pattern(GTK_FILE_FILTER(all_filter), "*");
	gtk_file_filter_set_name(GTK_FILE_FILTER(all_filter), _("All files"));

	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(buf), "", -1);

	gtk_spin_button_set_value(GTK_SPIN_BUTTON(minute_spin), 10);

	gtk_entry_set_text(GTK_ENTRY(title_entry), "");

	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(standard_sound_radio),
				     TRUE);

	gtk_file_chooser_set_uri(GTK_FILE_CHOOSER(soundfile_chooser), "");
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(soundfile_chooser),
				    sound_filter);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(soundfile_chooser),
				    all_filter);

	gtk_widget_show(GTK_WIDGET(dialog));
}

void hide_counter_editor(void)
{
	GtkWidget *dialog =
	    GTK_WIDGET(gtk_builder_get_object(gxml, "counter_editor_dialog"));
	gtk_widget_hide(GTK_WIDGET(dialog));
}

void sound_toggled(void)
{
	GtkWidget *soundfile_radio =
	    GTK_WIDGET(gtk_builder_get_object(gxml, "soundfile_radio"));
	GtkWidget *soundfile_chooser =
	    GTK_WIDGET(gtk_builder_get_object(gxml, "soundfile_chooser"));

	gtk_widget_set_sensitive(GTK_WIDGET(soundfile_chooser),
				 gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON
							      (soundfile_radio)));
}

void counter_accept(void)
{
	GtkWidget *dialog =
	    GTK_WIDGET(gtk_builder_get_object(gxml, "counter_editor_dialog"));
	GtkWidget *soundfile_chooser =
	    GTK_WIDGET(gtk_builder_get_object(gxml, "soundfile_chooser"));
	GtkWidget *minute_spin =
	    GTK_WIDGET(gtk_builder_get_object(gxml, "minutes_spin"));
	GtkWidget *title_entry =
	    GTK_WIDGET(gtk_builder_get_object(gxml, "title_entr"));
	GtkWidget *counter_text =
	    GTK_WIDGET(gtk_builder_get_object(gxml, "counter_text"));
	GtkWidget *soundfile_radio =
	    GTK_WIDGET(gtk_builder_get_object(gxml, "soundfile_radio"));
	GtkWidget *no_sound_radio =
	    GTK_WIDGET(gtk_builder_get_object(gxml, "no_sound_radio"));
	GtkWidget *standard_sound_radio =
	    GTK_WIDGET(gtk_builder_get_object(gxml, "standard_sound_radio"));
	gchar *file =
	    gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(soundfile_chooser));
	key = g_key_file_new();
	GtkTextBuffer *contents_buffer =
	    gtk_text_view_get_buffer(GTK_TEXT_VIEW(counter_text));
	GtkTextIter start, end;
	GKeyFile *old_key = g_key_file_new();
	gint mins, number;
	gchar *alarm_name, *title_buffer, **keys, *counter_text_buffer,
	    *keydata;
	gint x;

	GTimeVal current_time;
	gulong new_time;
	g_get_current_time(&current_time);

	struct tm *timeinfo;

	timeinfo = localtime((const time_t *)&current_time);

	gboolean is_chosen =
	    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(soundfile_radio));

	title_buffer = (gchar *) gtk_entry_get_text(GTK_ENTRY(title_entry));

	if (g_strcmp0(file, NULL) == 0 && is_chosen) {
		GtkWidget *message =
		    gtk_message_dialog_new_with_markup(GTK_WINDOW(dialog),
						       GTK_DIALOG_DESTROY_WITH_PARENT,
						       GTK_MESSAGE_WARNING,
						       GTK_BUTTONS_CLOSE, NULL);

		gtk_message_dialog_set_markup(GTK_MESSAGE_DIALOG(message),
					      _("<b>Invalid filename</b>"));
		gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG
							 (message),
							 _
							 ("You need to specify sound file name."));

		gtk_dialog_run(GTK_DIALOG(message));
		gtk_widget_destroy(message);
		g_key_file_free(key);
		g_free(file);
		g_key_file_free(old_key);
		return;
	}

	if (g_strcmp0(title_buffer, "") == 0) {
		GtkWidget *message =
		    gtk_message_dialog_new_with_markup(GTK_WINDOW(dialog),
						       GTK_DIALOG_DESTROY_WITH_PARENT,
						       GTK_MESSAGE_WARNING,
						       GTK_BUTTONS_CLOSE, NULL);

		gtk_message_dialog_set_markup(GTK_MESSAGE_DIALOG(message),
					      _("<b>Invalid title</b>"));
		gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG
							 (message),
							 _
							 ("You need to specify title of the counter."));

		gtk_dialog_run(GTK_DIALOG(message));
		gtk_widget_destroy(message);
		g_key_file_free(key);
		g_free(file);
		g_key_file_free(old_key);
		return;
	}

	mins = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(minute_spin));

	gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(contents_buffer),
				       &start);
	gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(contents_buffer), &end);

	timeinfo->tm_min = timeinfo->tm_min + mins;

	new_time = mktime(timeinfo);

	counter_text_buffer =
	    gtk_text_buffer_get_text(GTK_TEXT_BUFFER(contents_buffer), &start,
				     &end, FALSE);

	number = g_random_int();
	if (number < 0) {
		number = number * -1;
	}
	alarm_name = g_strdup_printf("alarm_%i", number);

	fill_default_for_counter(alarm_name, new_time);

	g_key_file_load_from_file(old_key, config_alarms, G_KEY_FILE_NONE, 0);

	g_key_file_set_string(key, alarm_name, "Title", title_buffer);
	g_key_file_set_double(key, alarm_name, "DateTime", new_time);
	g_key_file_set_string(key, alarm_name, "AlarmType", "Counter");

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(no_sound_radio))) {
		g_key_file_set_boolean(key, alarm_name, "SoundEnabled", FALSE);
	}
	if (gtk_toggle_button_get_active
	    (GTK_TOGGLE_BUTTON(standard_sound_radio))) {
		g_key_file_set_boolean(key, alarm_name, "SoundEnabled", TRUE);
		g_key_file_set_boolean(key, alarm_name, "CustomSoundEnabled",
				       FALSE);
		g_key_file_set_string(key, alarm_name, "CustomSoundFile",
				      "[DEFAULT]");
		g_key_file_set_boolean(key, alarm_name, "ConstantSoundEnabled",
				       TRUE);
		g_key_file_set_integer(key, alarm_name, "ConstantSoundValue",
				       1);
	}
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(soundfile_radio))) {
		g_key_file_set_boolean(key, alarm_name, "SoundEnabled", TRUE);
		g_key_file_set_boolean(key, alarm_name, "CustomSoundEnabled",
				       TRUE);
		g_key_file_set_string(key, alarm_name, "CustomSoundFile", file);
		g_key_file_set_boolean(key, alarm_name, "ConstantSoundEnabled",
				       TRUE);
		g_key_file_set_integer(key, alarm_name, "ConstantSoundValue",
				       1);
	}

	g_key_file_set_string(key, alarm_name, "PassivePopupTitle",
			      title_buffer);
	g_key_file_set_string(key, alarm_name, "PassivePopupText",
			      counter_text_buffer);

	keys = g_key_file_get_keys(key, alarm_name, NULL, NULL);

	x = 0;

	while (TRUE) {
		if (keys[x] == NULL)
			break;
		title_buffer =
		    g_key_file_get_string(key, alarm_name, keys[x], NULL);
		g_key_file_set_string(old_key, alarm_name, keys[x],
				      title_buffer);
		g_free(title_buffer);
		x++;
	}

	keydata = g_key_file_to_data(old_key, NULL, NULL);

	g_file_set_contents(config_alarms, keydata, -1, NULL);

	reload_alarms();
	update_list_entries();
	update_alarm_thread(FALSE);
	change_selection();

	g_free(alarm_name);
	g_strfreev(keys);
	g_free(counter_text_buffer);
	g_free(keydata);
	g_free(file);
	g_key_file_free(key);
	g_key_file_free(old_key);
	gtk_widget_hide(GTK_WIDGET(dialog));
}
