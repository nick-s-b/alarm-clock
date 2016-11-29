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
#include <time.h>
#include <string.h>

#include "modify_alarm.h"
#include "main_list.h"
#include "main_window.h"
#include "configuration.h"
#include "translate.h"
#include "alarm_thread.h"
#include "templates.h"

void
fill_templates(GtkWidget *treeview)
{
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), NULL);
	modify_templates_store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
	gchar **templates_content;
	GtkTreeIter iter;
	gint templates_num = 0;
	gchar *buffer;
	GtkWidget *templates_notebook = GTK_WIDGET (gtk_builder_get_object (gxml, "notebook4"));
	GtkWidget *settings_notebook = GTK_WIDGET (gtk_builder_get_object (gxml, "notebook2"));

	GKeyFile *templates_key = g_key_file_new();

	g_key_file_load_from_file(templates_key, config_templates, G_KEY_FILE_NONE, NULL);

	templates_content = g_key_file_get_groups(templates_key, NULL);

	while (TRUE)
	{
		if (templates_content[templates_num] == NULL) break;
		buffer = g_key_file_get_string(templates_key, templates_content[templates_num], "Title", NULL);
		gtk_list_store_append(GTK_LIST_STORE(modify_templates_store), &iter);
		gtk_list_store_set(GTK_LIST_STORE(modify_templates_store), &iter, 0, buffer, 1, templates_content[templates_num], -1);
		g_free(buffer);

		templates_num++;
	}


	if (templates_num == 0)
	{
		gtk_notebook_set_current_page(GTK_NOTEBOOK(templates_notebook), 1);
		gtk_notebook_set_current_page(GTK_NOTEBOOK(settings_notebook), 1);
	}

	else
	{
		gtk_notebook_set_current_page(GTK_NOTEBOOK(templates_notebook), 0);
		gtk_notebook_set_current_page(GTK_NOTEBOOK(settings_notebook), 0);
	}




	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeview), FALSE);
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(modify_templates_store));
	g_strfreev(templates_content);
}



void
notification_toggle(void)
{
	if (unload_boxes) return;
	GtkWidget *chk1 = GTK_WIDGET (gtk_builder_get_object (gxml, "checkbutton1"));
	GtkWidget *chk2 = GTK_WIDGET (gtk_builder_get_object (gxml, "checkbutton2"));
	GtkWidget *chk3 = GTK_WIDGET (gtk_builder_get_object (gxml, "checkbutton3"));
	GtkWidget *chk4 = GTK_WIDGET (gtk_builder_get_object (gxml, "checkbutton4"));
	GtkWidget *chk5 = GTK_WIDGET (gtk_builder_get_object (gxml, "checkbutton5"));

	GtkWidget *btn3 = GTK_WIDGET (gtk_builder_get_object (gxml, "button3"));
	GtkWidget *btn4 = GTK_WIDGET (gtk_builder_get_object (gxml, "button4"));
	GtkWidget *btn5 = GTK_WIDGET (gtk_builder_get_object (gxml, "button5"));
	GtkWidget *btn6 = GTK_WIDGET (gtk_builder_get_object (gxml, "button6"));
	GtkWidget *btn7 = GTK_WIDGET (gtk_builder_get_object (gxml, "button7"));

	gboolean chk1_act = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(chk1));
	gtk_widget_set_sensitive(GTK_WIDGET(btn3), chk1_act);
	if (!load) g_key_file_set_boolean(current_key, untitled_name, "SoundEnabled", chk1_act);

	gboolean chk2_act = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(chk2));
	gtk_widget_set_sensitive(GTK_WIDGET(btn4), chk2_act);
	if (!load) 	g_key_file_set_boolean(current_key, untitled_name, "PassivePopupEnabled", chk2_act);

	gboolean chk3_act = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(chk3));
	gtk_widget_set_sensitive(GTK_WIDGET(btn5), chk3_act);
	if (!load) 	g_key_file_set_boolean(current_key, untitled_name, "DialogWindowEnabled", chk3_act);

	gboolean chk4_act = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(chk4));
	gtk_widget_set_sensitive(GTK_WIDGET(btn6), chk4_act);
	if (!load) 	g_key_file_set_boolean(current_key, untitled_name, "RunCommand", chk4_act);

	gboolean chk5_act = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(chk5));
	gtk_widget_set_sensitive(GTK_WIDGET(btn7), chk5_act);
	if (!load) 	g_key_file_set_boolean(current_key, untitled_name, "ActionEnabled", chk5_act);

}

void
date_radio_toggle(void)
{
	GtkWidget *single_pref = GTK_WIDGET (gtk_builder_get_object (gxml, "single_day_button"));
	GtkWidget *schedule_pref = GTK_WIDGET (gtk_builder_get_object (gxml, "schedule_button"));
	GtkWidget *radio1 = GTK_WIDGET (gtk_builder_get_object (gxml, "radiobutton1"));
	GtkWidget *radio2 = GTK_WIDGET (gtk_builder_get_object (gxml, "radiobutton2"));
	GtkWidget *radio3 = GTK_WIDGET (gtk_builder_get_object (gxml, "radiobutton3"));
	GtkWidget *radio4 = GTK_WIDGET (gtk_builder_get_object (gxml, "radiobutton4"));

	GTimeVal current_time;
	g_get_current_time(&current_time);


	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio1)))
	{
		gtk_widget_set_sensitive(GTK_WIDGET(single_pref), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(schedule_pref), FALSE);
	}

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio2)))
	{
		gtk_widget_set_sensitive(GTK_WIDGET(single_pref), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(schedule_pref), TRUE);
	}

	// Today

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio3)))
	{
		gtk_widget_set_sensitive(GTK_WIDGET(single_pref), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(schedule_pref), FALSE);
		if (current_key != NULL)
			g_key_file_set_double(current_key, untitled_name, "DateTime", current_time.tv_sec);
	}

	// Tomorrow

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio4)))
	{
		gtk_widget_set_sensitive(GTK_WIDGET(single_pref), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(schedule_pref), FALSE);
		if (current_key != NULL)
			g_key_file_set_double(current_key, untitled_name, "DateTime", current_time.tv_sec + 86400);
	}
}

void
set_time_format(void)
{
	GKeyFile *keyfile = g_key_file_new();
	g_key_file_load_from_file(keyfile, config_global, G_KEY_FILE_NONE, NULL);
	gint value = g_key_file_get_integer(keyfile, "Global", "TimeType", NULL);
	g_key_file_free(keyfile);

	if (value == 24)
	{
		GtkWidget *hour = GTK_WIDGET (gtk_builder_get_object (gxml, "hour_spin"));

		gtk_widget_hide(GTK_WIDGET(ampm_combo));

		gtk_spin_button_set_range(GTK_SPIN_BUTTON(hour), 0, 23);
	}
	else if (value == 12)
	{
		GtkWidget *hour = GTK_WIDGET (gtk_builder_get_object (gxml, "hour_spin"));

		gtk_widget_show(GTK_WIDGET(ampm_combo));

		gtk_combo_box_set_active(GTK_COMBO_BOX(ampm_combo), 0);

		gtk_spin_button_set_range(GTK_SPIN_BUTTON(hour), 1, 12);
	}
	else
	{
		GKeyFile *keyfile = g_key_file_new();
		g_key_file_load_from_file(keyfile, config_global, G_KEY_FILE_NONE, NULL);

		g_key_file_set_integer(keyfile, "Global", "TimeType", 24);

		g_key_file_free(keyfile);

		GtkWidget *hour = GTK_WIDGET (gtk_builder_get_object (gxml, "hour_spin"));

		gtk_widget_hide(GTK_WIDGET(ampm_combo));

		gtk_spin_button_set_range(GTK_SPIN_BUTTON(hour), 0, 23);
	}
}

void
fill_default(void)
{
	GTimeVal current_time;
	gint number;
	current_key = g_key_file_new();
	GtkWidget *title = GTK_WIDGET (gtk_builder_get_object (gxml, "alarm_title_entry"));

	g_get_current_time(&current_time);

	struct tm *timeinfo;

	timeinfo = localtime((const time_t*)&current_time);

	timeinfo->tm_sec = 0;

	current_time.tv_sec = mktime(timeinfo);

	g_free(untitled_name);


	number = g_random_int();
	if (number < 0)
	{
		number = number * -1;
	}
	untitled_name = g_strdup_printf("alarm_%i", number);
	if (current_key != NULL)
		g_key_file_set_double(current_key, untitled_name, "DateTime", current_time.tv_sec);
	g_key_file_set_string(current_key, untitled_name, "Title", _("Untitled"));
	gtk_entry_set_text(GTK_ENTRY(title), _("Untitled"));
	g_key_file_set_string(current_key, untitled_name, "ScheduleWeekdays", "");
	g_key_file_set_string(current_key, untitled_name, "ScheduleMonths", "");
	g_key_file_set_string(current_key, untitled_name, "ScheduleDateInclude", "");
	g_key_file_set_string(current_key, untitled_name, "ScheduleDateExclude", "");
	g_key_file_set_string(current_key, untitled_name, "AlarmType", "Single");
	g_key_file_set_boolean(current_key, untitled_name, "SoundEnabled", TRUE);
	g_key_file_set_boolean(current_key, untitled_name, "CustomSoundEnabled", FALSE);
	g_key_file_set_string(current_key, untitled_name, "CustomSoundFile", "[DEFAULT]");
	g_key_file_set_boolean(current_key, untitled_name, "ConstantSoundEnabled", TRUE);
	g_key_file_set_integer(current_key, untitled_name, "ConstantSoundValue", 5);
	g_key_file_set_boolean(current_key, untitled_name, "FadeSoundEnabled", FALSE);
	g_key_file_set_integer(current_key, untitled_name, "FadeSoundValue", 5);
	g_key_file_set_boolean(current_key, untitled_name, "RepeatSoundEnabled", FALSE);
	g_key_file_set_integer(current_key, untitled_name, "RepeatSoundValue", 5);
	g_key_file_set_boolean(current_key, untitled_name, "PassivePopupEnabled", FALSE);
	g_key_file_set_integer(current_key, untitled_name, "PassivePopupType", 0);
	g_key_file_set_integer(current_key, untitled_name, "PassivePopupTimeout", 30);
	g_key_file_set_string(current_key, untitled_name, "PassivePopupTitle", _("Popup"));
	g_key_file_set_string(current_key, untitled_name, "PassivePopupText", _("Short popup"));
	g_key_file_set_boolean(current_key, untitled_name, "DialogWindowEnabled", FALSE);
	g_key_file_set_string(current_key, untitled_name, "DialogTextColor", "#000000000000");
	g_key_file_set_string(current_key, untitled_name, "DialogBackgroundColor", "#ffffffffffff");
	g_key_file_set_integer(current_key, untitled_name, "DialogTextSize", 1);
	g_key_file_set_string(current_key, untitled_name, "DialogTitle", _("Dialog window"));
	g_key_file_set_string(current_key, untitled_name, "DialogMessage", _("This is a small message"));
	g_key_file_set_boolean(current_key, untitled_name, "SnoozeEnabled", FALSE);
	g_key_file_set_integer(current_key, untitled_name, "SnoozeTime", 5);
	g_key_file_set_boolean(current_key, untitled_name, "RunCommand", FALSE);
	g_key_file_set_string(current_key, untitled_name, "RunCommandContent", "");
	g_key_file_set_boolean(current_key, untitled_name, "RunInTerminal", TRUE);
	g_key_file_set_boolean(current_key, untitled_name, "ActionEnabled", FALSE);
	g_key_file_set_boolean(current_key, untitled_name, "ActionFirstSelected", TRUE);

}

gboolean
spin_change(GtkWidget *spinbutton,
			GtkScrollType *scrolltype,
			gpointer user_data)
{
	glong datetime;
	struct tm * timeinfo;
	GKeyFile *keyfile = g_key_file_new();
	g_key_file_load_from_file(keyfile, config_global, G_KEY_FILE_NONE, NULL);

	gint timeformat = g_key_file_get_integer(keyfile, "Global", "TimeType", NULL);
	gint ampm = gtk_combo_box_get_active(GTK_COMBO_BOX(ampm_combo));

	GtkWidget *hour = GTK_WIDGET (gtk_builder_get_object (gxml, "hour_spin"));
	GtkWidget *minute = GTK_WIDGET (gtk_builder_get_object (gxml, "minute_spin"));
	if (current_key != NULL)
		datetime = g_key_file_get_double(current_key, untitled_name, "DateTime", NULL);

	timeinfo = localtime ( &datetime );
	gint val = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(hour));
	if (timeformat == 12) {
		if (ampm == 0) {
			if (val == 12) val = 0;
			timeinfo->tm_hour = val;
		}

		if (ampm == 1) {
			if (val == 12) val = 0;
			timeinfo->tm_hour = val + 12;
		}
	}
	else
	{
		timeinfo->tm_hour = val;
	}
	timeinfo->tm_min = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(minute));
	if (current_key != NULL)
		g_key_file_set_double(current_key, untitled_name, "DateTime", mktime(timeinfo));

	return FALSE;
}

void
sound_props_check(void)
{
	gint value;
	gchar *text;

	GtkWidget *custom1 = GTK_WIDGET (gtk_builder_get_object (gxml, "custom_sound_check"));
	GtkWidget *custom2 = GTK_WIDGET (gtk_builder_get_object (gxml, "custom_sound_button"));

	GtkWidget *constant1 = GTK_WIDGET (gtk_builder_get_object (gxml, "constant_volume_radio"));
	GtkWidget *constant2 = GTK_WIDGET (gtk_builder_get_object (gxml, "constant_volume_scale"));
	GtkWidget *constant3 = GTK_WIDGET (gtk_builder_get_object (gxml, "constant_volume_label"));

	GtkWidget *faded1 = GTK_WIDGET (gtk_builder_get_object (gxml, "faded_volume_radio"));
	GtkWidget *faded2 = GTK_WIDGET (gtk_builder_get_object (gxml, "faded_volume_scale"));
	GtkWidget *faded3 = GTK_WIDGET (gtk_builder_get_object (gxml, "faded_volume_label"));

	GtkWidget *repeat1 = GTK_WIDGET (gtk_builder_get_object (gxml, "repeat_check"));
	GtkWidget *repeat2 = GTK_WIDGET (gtk_builder_get_object (gxml, "repeat_scale"));
	GtkWidget *repeat3 = GTK_WIDGET (gtk_builder_get_object (gxml, "repeat_label"));

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(custom1)))
	{
		gtk_widget_set_sensitive(GTK_WIDGET(custom2), TRUE);
	}
	else
	{
		gtk_widget_set_sensitive(GTK_WIDGET(custom2), FALSE);
	}


	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(constant1)))
	{
		gtk_widget_set_sensitive(GTK_WIDGET(constant2), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(constant3), TRUE);
	}
	else
	{
		gtk_widget_set_sensitive(GTK_WIDGET(constant2), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(constant3), FALSE);
	}

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(faded1)))
	{
		gtk_widget_set_sensitive(GTK_WIDGET(faded2), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(faded3), TRUE);
	}
	else
	{
		gtk_widget_set_sensitive(GTK_WIDGET(faded2), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(faded3), FALSE);
	}

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(repeat1)))
	{
		gtk_widget_set_sensitive(GTK_WIDGET(repeat2), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(repeat3), TRUE);
	}
	else
	{
		gtk_widget_set_sensitive(GTK_WIDGET(repeat2), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(repeat3), FALSE);
	}

	value = gtk_range_get_value(GTK_RANGE(constant2));
	if (value == 0)
		text = g_strdup_printf(_("Infinite"));
	else
		text = g_strdup_printf("%i min", value);
	gtk_label_set_text(GTK_LABEL(constant3), text);
	g_free(text);



	value = gtk_range_get_value(GTK_RANGE(faded2));
	text = g_strdup_printf("%i min", value);
	gtk_label_set_text(GTK_LABEL(faded3), text);
	g_free(text);



	value = gtk_range_get_value(GTK_RANGE(repeat2));
	if (value == 0)
		text = g_strdup_printf(_("Infinite"));
	else
		text = g_strdup_printf("%i min", value);
	gtk_label_set_text(GTK_LABEL(repeat3), text);
	g_free(text);
}

void
sound_props_ok(void)
{
	GtkWidget *sound = GTK_WIDGET (gtk_builder_get_object (gxml, "sound_prefs_dialog"));
	GtkWidget *custom1 = GTK_WIDGET (gtk_builder_get_object (gxml, "custom_sound_check"));
	GtkWidget *custom2 = GTK_WIDGET (gtk_builder_get_object (gxml, "custom_sound_button"));

	GtkWidget *constant1 = GTK_WIDGET (gtk_builder_get_object (gxml, "constant_volume_radio"));
	GtkWidget *constant2 = GTK_WIDGET (gtk_builder_get_object (gxml, "constant_volume_scale"));

	GtkWidget *faded1 = GTK_WIDGET (gtk_builder_get_object (gxml, "faded_volume_radio"));
	GtkWidget *faded2 = GTK_WIDGET (gtk_builder_get_object (gxml, "faded_volume_scale"));

	GtkWidget *repeat1 = GTK_WIDGET (gtk_builder_get_object (gxml, "repeat_check"));
	GtkWidget *repeat2 = GTK_WIDGET (gtk_builder_get_object (gxml, "repeat_scale"));

	gchar *buffer;
	gint int_buffer;;
	gboolean bool_buffer;
	buffer = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(custom2));
	bool_buffer = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(custom1));
	if (buffer == NULL && bool_buffer == TRUE)
	{
		GtkWidget *message = gtk_message_dialog_new_with_markup (GTK_WINDOW(sound),
																 GTK_DIALOG_DESTROY_WITH_PARENT,
																 GTK_MESSAGE_WARNING,
																 GTK_BUTTONS_CLOSE, NULL);

		gtk_message_dialog_set_markup(GTK_MESSAGE_DIALOG(message), _("<b>Invalid filename</b>"));
		gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(message), _("You need to specify sound file name."));

		gtk_dialog_run (GTK_DIALOG (message));
		gtk_widget_destroy (message);
		g_free(buffer);
		return;
	}

	g_key_file_set_boolean(current_key, untitled_name, "SoundEnabled", TRUE);

	bool_buffer = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(custom1));

	g_key_file_set_boolean(current_key, untitled_name, "CustomSoundEnabled", bool_buffer);

	buffer = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(custom2));

	if (buffer == NULL) buffer = g_strdup_printf("[DEFAULT]");

	g_key_file_set_string(current_key, untitled_name, "CustomSoundFile", buffer);
	g_free(buffer);


	/* Repeat */

	bool_buffer = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(constant1));

	g_key_file_set_boolean(current_key, untitled_name, "ConstantSoundEnabled",
						   bool_buffer);

	int_buffer= gtk_range_get_value(GTK_RANGE(constant2));
	g_key_file_set_integer(current_key, untitled_name, "ConstantSoundValue",
						   int_buffer);


	/* Fade */

	bool_buffer = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(faded1));

	g_key_file_set_boolean(current_key, untitled_name, "FadeSoundEnabled",
						   bool_buffer);

	int_buffer= gtk_range_get_value(GTK_RANGE(faded2));
	g_key_file_set_integer(current_key, untitled_name, "FadeSoundValue",
						   int_buffer);

	/* Repeat after */

	bool_buffer = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(repeat1));

	g_key_file_set_boolean(current_key, untitled_name, "RepeatSoundEnabled",
						   bool_buffer);

	int_buffer= gtk_range_get_value(GTK_RANGE(repeat2));
	g_key_file_set_integer(current_key, untitled_name, "RepeatSoundValue",
						   int_buffer);


	gtk_widget_hide(GTK_WIDGET(sound));

}

void
show_sound_properties(void)
{
	GtkWidget *sound = GTK_WIDGET (gtk_builder_get_object (gxml, "sound_prefs_dialog"));
	GtkWidget *custom1 = GTK_WIDGET (gtk_builder_get_object (gxml, "custom_sound_check"));
	GtkWidget *custom2 = GTK_WIDGET (gtk_builder_get_object (gxml, "custom_sound_button"));

	GtkWidget *constant1 = GTK_WIDGET (gtk_builder_get_object (gxml, "constant_volume_radio"));
	GtkWidget *constant2 = GTK_WIDGET (gtk_builder_get_object (gxml, "constant_volume_scale"));

	GtkWidget *faded1 = GTK_WIDGET (gtk_builder_get_object (gxml, "faded_volume_radio"));
	GtkWidget *faded2 = GTK_WIDGET (gtk_builder_get_object (gxml, "faded_volume_scale"));

	GtkWidget *repeat1 = GTK_WIDGET (gtk_builder_get_object (gxml, "repeat_check"));
	GtkWidget *repeat2 = GTK_WIDGET (gtk_builder_get_object (gxml, "repeat_scale"));

	gchar *buffer;
	gint int_buffer;;
	gboolean bool_buffer;

	/* Filling the fields */

	bool_buffer = g_key_file_get_boolean(current_key, untitled_name, "CustomSoundEnabled", NULL);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(custom1), bool_buffer);
	buffer = g_key_file_get_string(current_key, untitled_name, "CustomSoundFile", NULL);
	gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(custom2), buffer);
	g_free(buffer);

	bool_buffer = g_key_file_get_boolean(current_key, untitled_name, "ConstantSoundEnabled", NULL);
	int_buffer = g_key_file_get_integer(current_key, untitled_name, "ConstantSoundValue", NULL);
	if (bool_buffer) gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(constant1), TRUE);
	gtk_range_set_value(GTK_RANGE(constant2), int_buffer);

	bool_buffer = g_key_file_get_boolean(current_key, untitled_name, "FadeSoundEnabled", NULL);
	int_buffer = g_key_file_get_integer(current_key, untitled_name, "FadeSoundValue", NULL);
	if (bool_buffer) gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(faded1), TRUE);
	gtk_range_set_value(GTK_RANGE(faded2), int_buffer);

	bool_buffer = g_key_file_get_boolean(current_key, untitled_name, "RepeatSoundEnabled", NULL);
	int_buffer = g_key_file_get_integer(current_key, untitled_name, "RepeatSoundValue", NULL);
	if (bool_buffer) gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(repeat1), TRUE);
	gtk_range_set_value(GTK_RANGE(repeat2), int_buffer);

	sound_props_check();

	gtk_widget_show(GTK_WIDGET(sound));
}


void
show_popup_properties(void)
{
	GtkWidget *popup = GTK_WIDGET (gtk_builder_get_object (gxml, "popup_prefs_dialog"));
	GtkWidget *timeout = GTK_WIDGET (gtk_builder_get_object (gxml, "timeout_spin"));
	GtkWidget *title = GTK_WIDGET (gtk_builder_get_object (gxml, "title_entry"));
	GtkWidget *message = GTK_WIDGET (gtk_builder_get_object (gxml, "message_textview"));
	GtkTextBuffer *txt_buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(message));
	GtkTextIter start, end;
	gint integer_buffer;
	gchar *gbuffer;

	integer_buffer = g_key_file_get_integer(current_key, untitled_name, "PassivePopupType", NULL);
	gtk_combo_box_set_active(GTK_COMBO_BOX(priority_combo), integer_buffer);

	integer_buffer = g_key_file_get_integer(current_key, untitled_name, "PassivePopupTimeout", NULL);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(timeout), integer_buffer);

	gbuffer = g_key_file_get_string(current_key, untitled_name, "PassivePopupTitle", NULL);
	gtk_entry_set_text(GTK_ENTRY(title), gbuffer);

	gbuffer = g_key_file_get_string(current_key, untitled_name, "PassivePopupText", NULL);
	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(txt_buf), gbuffer, -1);

	gtk_dialog_run(GTK_DIALOG(popup));

	gtk_widget_hide(GTK_WIDGET(popup));

	integer_buffer = gtk_combo_box_get_active(GTK_COMBO_BOX(priority_combo));
	g_key_file_set_integer(current_key, untitled_name, "PassivePopupType", integer_buffer);

	integer_buffer = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(timeout));
	g_key_file_set_integer(current_key, untitled_name, "PassivePopupTimeout", integer_buffer);

	gbuffer = (gchar*)gtk_entry_get_text(GTK_ENTRY(title));
	g_key_file_set_string(current_key, untitled_name, "PassivePopupTitle", gbuffer);

	gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(txt_buf), &start);
	gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(txt_buf), &end);

	gbuffer = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(txt_buf), &start, &end, TRUE);
	g_key_file_set_string(current_key, untitled_name, "PassivePopupText", gbuffer);

	g_free(gbuffer);
}

void
load_current_key()
{
	gint today_tomorrow = 0;
	GKeyFile *buffer_key = g_key_file_new();
	gulong datetime;
	gint amin, ahour;
	if (current_key != NULL)
		datetime = g_key_file_get_double(current_key, untitled_name, "DateTime", NULL);

	struct tm * timeinfo;
	timeinfo = localtime ((const time_t*)&datetime);
	GKeyFile *keyfile = g_key_file_new();
	g_key_file_load_from_file(keyfile, config_global, G_KEY_FILE_NONE, NULL);

	gint value = g_key_file_get_integer(keyfile, "Global", "TimeType", NULL);
	GtkWidget *title = GTK_WIDGET (gtk_builder_get_object (gxml, "alarm_title_entry"));
	GtkWidget *hour = GTK_WIDGET (gtk_builder_get_object (gxml, "hour_spin"));
	GtkWidget *min = GTK_WIDGET (gtk_builder_get_object (gxml, "minute_spin"));
	GtkWidget *chk1 = GTK_WIDGET (gtk_builder_get_object (gxml, "checkbutton1"));
	GtkWidget *chk2 = GTK_WIDGET (gtk_builder_get_object (gxml, "checkbutton2"));
	GtkWidget *chk3 = GTK_WIDGET (gtk_builder_get_object (gxml, "checkbutton3"));
	GtkWidget *chk4 = GTK_WIDGET (gtk_builder_get_object (gxml, "checkbutton4"));
	GtkWidget *chk5 = GTK_WIDGET (gtk_builder_get_object (gxml, "checkbutton5"));
	GtkWidget *radio3 = GTK_WIDGET (gtk_builder_get_object (gxml, "radiobutton3"));
	GtkWidget *radio4 = GTK_WIDGET (gtk_builder_get_object (gxml, "radiobutton4"));
	GtkWidget *calendar = GTK_WIDGET (gtk_builder_get_object (gxml, "calendar1"));

	ahour = timeinfo->tm_hour;
	amin = timeinfo->tm_min;

	if (value == 12)
	{
		if (ahour < 12 && ahour > 0) {
				gtk_spin_button_set_value(GTK_SPIN_BUTTON(hour), ahour);
				gtk_combo_box_set_active(GTK_COMBO_BOX(ampm_combo), 0);
			}

		if (ahour > 12)	{
			gtk_spin_button_set_value(GTK_SPIN_BUTTON(hour), ahour - 12);
			gtk_combo_box_set_active(GTK_COMBO_BOX(ampm_combo), 1);
			}

		if (ahour == 12) {
				gtk_spin_button_set_value(GTK_SPIN_BUTTON(hour), 12);
				gtk_combo_box_set_active(GTK_COMBO_BOX(ampm_combo), 1);
			}
		if (ahour == 24) {
				gtk_spin_button_set_value(GTK_SPIN_BUTTON(hour), 12);
				gtk_combo_box_set_active(GTK_COMBO_BOX(ampm_combo), 0);
			}

		if (ahour == 0) {
				gtk_spin_button_set_value(GTK_SPIN_BUTTON(hour), 12);
				gtk_combo_box_set_active(GTK_COMBO_BOX(ampm_combo), 0);
		}

	}
	else
	{
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(hour), ahour);
	}



	gtk_spin_button_set_value(GTK_SPIN_BUTTON(min), amin);

	gtk_entry_set_text(GTK_ENTRY(title), g_key_file_get_string(current_key, untitled_name, "Title", NULL));
	gchar *data = g_key_file_to_data(current_key, NULL, 0);
	g_key_file_load_from_data(buffer_key, data, -1, G_KEY_FILE_NONE, NULL);
	g_free(data);

	gtk_calendar_select_month(GTK_CALENDAR(calendar), timeinfo->tm_mon, timeinfo->tm_year + 1900);

	gtk_calendar_select_day(GTK_CALENDAR(calendar), timeinfo->tm_mday);


	if (g_strcmp0(g_key_file_get_string(buffer_key, untitled_name, "SoundEnabled", NULL), "true") == 0)
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(chk1), TRUE);
	else
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(chk1), FALSE);

	if (g_strcmp0(g_key_file_get_string(buffer_key, untitled_name, "PassivePopupEnabled", NULL), "true") == 0)
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(chk2), TRUE);
	else
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(chk2), FALSE);

	if (g_strcmp0(g_key_file_get_string(buffer_key, untitled_name, "DialogWindowEnabled", NULL), "true") == 0)
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(chk3), TRUE);
	else
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(chk3), FALSE);

	if (g_strcmp0(g_key_file_get_string(buffer_key, untitled_name, "RunCommand", NULL), "true") == 0)
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(chk4), TRUE);
	else
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(chk4), FALSE);

	if (g_strcmp0(g_key_file_get_string(buffer_key, untitled_name, "ActionEnabled", NULL), "true") == 0)
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(chk5), TRUE);
	else
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(chk5), FALSE);


	today_tomorrow = g_key_file_get_integer(buffer_key, untitled_name, "TodayTomorrow", NULL);

	if (today_tomorrow == 1)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio3), TRUE);

	if (today_tomorrow == 2)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio4), TRUE);


	g_key_file_free(buffer_key);
}

void
add_alarm_dialog(void)
{
	GtkWidget *dialog = GTK_WIDGET (gtk_builder_get_object (gxml, "add_alarm_dialog"));
	GtkWidget *tree = GTK_WIDGET (gtk_builder_get_object (gxml, "templates_tree"));
	GtkWidget *note = GTK_WIDGET (gtk_builder_get_object (gxml, "notebook2"));
	GtkWidget *single_radio = GTK_WIDGET (gtk_builder_get_object (gxml, "radiobutton1"));



	gtk_notebook_set_current_page(GTK_NOTEBOOK(note), 0);

	fill_templates(tree);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(single_radio), TRUE);
	fill_default();
	load_current_key();

	notification_toggle();

	gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(main_window));

	gtk_widget_show(GTK_WIDGET(dialog));
}

void
translate_keys(GKeyFile *from, GKeyFile *to)
{
	gchar **keys = g_key_file_get_keys(from, untitled_name, NULL, NULL);


	int count = 0;

	gchar *buffer;

	while (TRUE)
	{
		if (keys[count] == NULL) break;
		buffer = g_key_file_get_value(from, untitled_name, keys[count], NULL);
		g_key_file_set_value(to, untitled_name, keys[count], buffer);
		g_free(buffer);
		count++;
	}
}

void
disable_all_checkboxes(void)
{
	unload_boxes = TRUE;
	GtkWidget *ch1 = GTK_WIDGET (gtk_builder_get_object (gxml, "checkbutton1"));
	GtkWidget *ch2 = GTK_WIDGET (gtk_builder_get_object (gxml, "checkbutton2"));
	GtkWidget *ch3 = GTK_WIDGET (gtk_builder_get_object (gxml, "checkbutton3"));
	GtkWidget *ch4 = GTK_WIDGET (gtk_builder_get_object (gxml, "checkbutton4"));
	GtkWidget *ch5 = GTK_WIDGET (gtk_builder_get_object (gxml, "checkbutton5"));

	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ch1), FALSE);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ch2), FALSE);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ch3), FALSE);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ch4), FALSE);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ch5), FALSE);

	unload_boxes = FALSE;
}

void
hide_modify_alarm(void)
{
	GtkWidget *apply_template = GTK_WIDGET (gtk_builder_get_object (gxml, "apply_template_button"));
	GtkWidget *dialog = GTK_WIDGET (gtk_builder_get_object (gxml, "add_alarm_dialog"));
	editing_template = FALSE;
	gtk_widget_set_sensitive(GTK_WIDGET(apply_template), FALSE);
	g_key_file_free(current_key);
	disable_all_checkboxes();
	gtk_widget_hide(GTK_WIDGET(dialog));
}

void
modify_alarm_clicked_ok(void)
{
	gchar *data;

	spin_change(NULL, NULL, NULL);
	GtkWidget *title = GTK_WIDGET (gtk_builder_get_object (gxml, "alarm_title_entry"));
	GtkWidget *alarms_dialog = GTK_WIDGET (gtk_builder_get_object (gxml, "add_alarm_dialog"));
	GtkWidget *chk1 = GTK_WIDGET (gtk_builder_get_object (gxml, "checkbutton1"));
	GtkWidget *chk2 = GTK_WIDGET (gtk_builder_get_object (gxml, "checkbutton2"));
	GtkWidget *chk3 = GTK_WIDGET (gtk_builder_get_object (gxml, "checkbutton3"));
	GtkWidget *chk4 = GTK_WIDGET (gtk_builder_get_object (gxml, "checkbutton4"));
	GtkWidget *chk5 = GTK_WIDGET (gtk_builder_get_object (gxml, "checkbutton5"));

	GtkWidget *radio3 = GTK_WIDGET (gtk_builder_get_object (gxml, "radiobutton3"));
	GtkWidget *radio4 = GTK_WIDGET (gtk_builder_get_object (gxml, "radiobutton4"));

	if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(chk1)) &&
		!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(chk2)) &&
		!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(chk3)) &&
		!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(chk4)) &&
		!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(chk5)))
	{
		GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(alarms_dialog),
                                  GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
                                  GTK_MESSAGE_ERROR,
                                  GTK_BUTTONS_CLOSE,
                                  NULL);
		gtk_message_dialog_set_markup(GTK_MESSAGE_DIALOG(error_dialog),
									  _("<b>Error occured!</b>"));
		gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(error_dialog),
												 _("You need to select at least one notification."));
		gtk_dialog_run (GTK_DIALOG (error_dialog));
		gtk_widget_destroy (GTK_WIDGET(error_dialog));

		return;
	}



	const gchar *val = gtk_entry_get_text(GTK_ENTRY(title));

	if (g_strcmp0(val, "") == 0)
	{
		GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(alarms_dialog),
                                  GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
                                  GTK_MESSAGE_ERROR,
                                  GTK_BUTTONS_CLOSE,
                                  NULL);
		gtk_message_dialog_set_markup(GTK_MESSAGE_DIALOG(error_dialog),
									  _("<b>Error occured!</b>"));
		gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(error_dialog),
							 _("Alarm title cannot be empty."));
		gtk_dialog_run (GTK_DIALOG (error_dialog));
		gtk_widget_destroy (GTK_WIDGET(error_dialog));

		return;
	}

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio3)) && editing_template)
	    g_key_file_set_integer(current_key, untitled_name, "TodayTomorrow", 1);

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio4)) && editing_template)
	    g_key_file_set_integer(current_key, untitled_name, "TodayTomorrow", 2);

	if (editing_template == FALSE)
	{
		g_key_file_set_integer(current_key, untitled_name, "TodayTomorrow", 0);
		translate_keys(current_key, loaded_alarms);
		data = g_key_file_to_data(loaded_alarms, NULL, NULL);
		g_file_set_contents(config_alarms, data, -1, NULL);
	}
	else
	{
		GKeyFile *templates = g_key_file_new();
		gchar *buf1, *buf2;
		g_key_file_load_from_file(templates, config_templates, G_KEY_FILE_NONE, NULL);
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio3)))
		{
			buf1 = g_key_file_get_string(current_key, untitled_name, "DateTime", NULL);
			buf2 = g_strdup_printf("%s|%s", buf1, "TODAY");
			g_key_file_set_string(current_key, untitled_name, "DateTime", buf2);
			g_free(buf1);
			g_free(buf2);
		}
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio4)))
		{
			buf1 = g_key_file_get_string(current_key, untitled_name, "DateTime", NULL);
			buf2 = g_strdup_printf("%s|%s", buf1, "TOMORROW");
			g_key_file_set_string(current_key, untitled_name, "DateTime", buf2);
			g_free(buf1);
			g_free(buf2);
		}

		translate_keys(current_key, templates);
		data = g_key_file_to_data(templates, NULL, NULL);
		g_file_set_contents(config_templates, data, -1, NULL);
		reload_templates();
		editing_template = FALSE;
		g_key_file_free(templates);
	}

	g_free(data);
	reload_alarms();
	update_list_entries();
	update_alarm_thread(FALSE);
	change_selection();
	hide_modify_alarm();
	disable_all_checkboxes();
}

void
modify_alarm(gchar *name)
{
	load = TRUE;
	current_key = g_key_file_new();
	untitled_name = g_strdup_printf("%s", name);
	gchar *buffer;

	if (editing_template == FALSE)
	{
		translate_keys(loaded_alarms, current_key);
	}
	else
	{
		GKeyFile *key = g_key_file_new();
		g_key_file_load_from_file(key, config_templates, G_KEY_FILE_NONE, NULL);
		translate_keys(key, current_key);
		g_key_file_free(key);
	}

	GtkWidget *dialog = GTK_WIDGET (gtk_builder_get_object (gxml, "add_alarm_dialog"));
	GtkWidget *tree = GTK_WIDGET (gtk_builder_get_object (gxml, "templates_tree"));
	GtkWidget *note = GTK_WIDGET (gtk_builder_get_object (gxml, "notebook2"));
	GtkWidget *radio1 = GTK_WIDGET (gtk_builder_get_object (gxml, "radiobutton1"));
	GtkWidget *radio2 = GTK_WIDGET (gtk_builder_get_object (gxml, "radiobutton2"));

	buffer = g_key_file_get_string(current_key, untitled_name, "AlarmType", NULL);

	if (g_strcmp0(buffer, "Single") == 0)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio1), TRUE);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio2), TRUE);

	g_free(buffer);


	gtk_notebook_set_current_page(GTK_NOTEBOOK(note), 1);

	fill_templates(tree);
	load_current_key();
	notification_toggle();

	gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(main_window));

	load = FALSE;

	gtk_widget_show(GTK_WIDGET(dialog));
}

void
title_entry_change(GtkWidget *entry, gpointer user_data)
{
	const gchar *text = gtk_entry_get_text(GTK_ENTRY(entry));
	g_key_file_set_string(current_key, untitled_name, "Title", text);
}

void
snooze_check_toggled(GtkWidget *snooze)
{
	gboolean active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(snooze));
	GtkWidget *snooze_spin = GTK_WIDGET (gtk_builder_get_object (gxml, "snooze_spin"));
	gtk_widget_set_sensitive(GTK_WIDGET(snooze_spin), active);
}

void
show_dialog_properties(void)
{
	GtkWidget *dialog = GTK_WIDGET (gtk_builder_get_object (gxml, "window_prefs_dialog"));
	GtkWidget *text_color_button = GTK_WIDGET (gtk_builder_get_object (gxml, "text_color_button"));
	GtkWidget *background_color_button = GTK_WIDGET (gtk_builder_get_object (gxml, "background_color_button"));
	GtkWidget *title_entry = GTK_WIDGET (gtk_builder_get_object (gxml, "dialog_title"));
	GtkWidget *message_entry = GTK_WIDGET (gtk_builder_get_object (gxml, "dialog_message"));
	GtkWidget *snooze_check = GTK_WIDGET (gtk_builder_get_object (gxml, "snooze_check"));
	GtkWidget *snooze_len = GTK_WIDGET (gtk_builder_get_object (gxml, "snooze_spin"));
	GtkTextBuffer *text_view_buffer;
	gchar *tcolor, *bcolor, *title, *message;
	gint snooze_length, text_size;
	gboolean snooze_active;
	GtkTextIter start_iter, end_iter;
	GdkColor text_color, background_color;

	tcolor = g_key_file_get_string(current_key, untitled_name, "DialogTextColor", NULL);
	gdk_color_parse(tcolor, &text_color);
	gtk_color_button_set_color(GTK_COLOR_BUTTON(text_color_button), &text_color);

	bcolor = g_key_file_get_string(current_key, untitled_name, "DialogBackgroundColor", NULL);
	gdk_color_parse(bcolor, &background_color);
	gtk_color_button_set_color(GTK_COLOR_BUTTON(background_color_button), &background_color);

	text_size = g_key_file_get_integer(current_key, untitled_name, "DialogTextSize", NULL);
	gtk_combo_box_set_active(GTK_COMBO_BOX(text_size_combo), text_size);

	title = g_key_file_get_string(current_key, untitled_name, "DialogTitle", NULL);
	gtk_entry_set_text(GTK_ENTRY(title_entry), title);

	text_view_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(message_entry));
	message = g_key_file_get_string(current_key, untitled_name, "DialogMessage", NULL);
	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(text_view_buffer), message, -1);

	snooze_active = g_key_file_get_boolean(current_key, untitled_name, "SnoozeEnabled", NULL);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(snooze_check), snooze_active);
	gtk_widget_set_sensitive(GTK_WIDGET(snooze_len), snooze_active);

	snooze_length = g_key_file_get_integer(current_key, untitled_name, "SnoozeTime", NULL);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(snooze_len), snooze_length);

	gtk_dialog_run(GTK_DIALOG(dialog));

	gtk_color_button_get_color(GTK_COLOR_BUTTON(text_color_button), &text_color);
	gtk_color_button_get_color(GTK_COLOR_BUTTON(background_color_button), &background_color);

	tcolor = gdk_color_to_string(&text_color);
	bcolor = gdk_color_to_string(&background_color);
	text_size = gtk_combo_box_get_active(GTK_COMBO_BOX(text_size_combo));
	title = (gchar *)gtk_entry_get_text(GTK_ENTRY(title_entry));
	snooze_active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(snooze_check));
	snooze_length = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(snooze_len));
	text_view_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(message_entry));
	gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(text_view_buffer), &start_iter);
	gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(text_view_buffer), &end_iter);
	message = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(text_view_buffer), &start_iter, &end_iter, TRUE);

	g_key_file_set_string(current_key, untitled_name, "DialogTextColor", tcolor);
	g_key_file_set_string(current_key, untitled_name, "DialogBackgroundColor", bcolor);

	g_key_file_set_integer(current_key, untitled_name, "DialogTextSize", text_size);
	g_key_file_set_string(current_key, untitled_name, "DialogTitle", title);
	g_key_file_set_string(current_key, untitled_name, "DialogMessage", message);
	g_key_file_set_boolean(current_key, untitled_name, "SnoozeEnabled", snooze_active);
	g_key_file_set_integer(current_key, untitled_name, "SnoozeTime", snooze_length);

	gtk_widget_hide(GTK_WIDGET(dialog));
}

void
show_command_properties(void)
{
	GtkWidget *dialog = GTK_WIDGET (gtk_builder_get_object (gxml, "command_prefs_dialog"));
	GtkWidget *run_script_entry = GTK_WIDGET (gtk_builder_get_object (gxml, "script_entry"));
	GtkWidget *run_in_terminal = GTK_WIDGET (gtk_builder_get_object (gxml, "command_terminal_check"));

	gchar *command;
	GtkTextIter start, end;
	gboolean run_in_terminal_checked;
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(run_script_entry));

	command = g_key_file_get_string(current_key, untitled_name, "RunCommandContent", NULL);
	run_in_terminal_checked = g_key_file_get_boolean(current_key, untitled_name, "RunInTerminal", NULL);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(run_in_terminal), run_in_terminal_checked);

	gtk_text_buffer_set_text (GTK_TEXT_BUFFER(buffer), command, strlen(command));

	gtk_dialog_run(GTK_DIALOG(dialog));

	gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer), &start);
	gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buffer), &end);

	command = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(buffer), &start, &end, TRUE);
	run_in_terminal_checked = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(run_in_terminal));

	g_key_file_set_string(current_key, untitled_name, "RunCommandContent", command);
	g_key_file_set_boolean(current_key, untitled_name, "RunInTerminal", run_in_terminal_checked);


	gtk_widget_hide(GTK_WIDGET(dialog));
}

void
command_radio_toggle(void)
{
	GtkWidget *run_command_radio = GTK_WIDGET (gtk_builder_get_object (gxml, "radiobutton5"));
	GtkWidget *run_command_entry = GTK_WIDGET (gtk_builder_get_object (gxml, "command_entry"));
	GtkWidget *run_script_entry = GTK_WIDGET (gtk_builder_get_object (gxml, "script_entry"));

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(run_command_radio)))
	{
		gtk_widget_set_sensitive(GTK_WIDGET(run_command_entry), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(run_script_entry), FALSE);
	}
	else
	{
		gtk_widget_set_sensitive(GTK_WIDGET(run_command_entry), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(run_script_entry), TRUE);
	}
}

void
show_std_properties(void)
{
	GtkWidget *dialog = GTK_WIDGET (gtk_builder_get_object (gxml, "action_prefs_dialog"));
	GtkWidget *lock_monitor_radio = GTK_WIDGET (gtk_builder_get_object (gxml, "radiobutton7"));
	GtkWidget *turn_off_radio = GTK_WIDGET (gtk_builder_get_object (gxml, "radiobutton8"));

	gboolean first_selected = g_key_file_get_boolean(current_key, untitled_name, "ActionFirstSelected", NULL);

	if (first_selected)
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lock_monitor_radio), TRUE);
	}
	else
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(turn_off_radio), TRUE);
	}

	gtk_dialog_run(GTK_DIALOG(dialog));

	first_selected = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(lock_monitor_radio));

	g_key_file_set_boolean(current_key, untitled_name, "ActionFirstSelected", first_selected);

	gtk_widget_hide(GTK_WIDGET(dialog));
}

void
remove_alarm(gchar *current)
{
	gchar *data;
	g_key_file_remove_group(loaded_alarms, current, NULL);
	data = g_key_file_to_data(loaded_alarms, NULL, NULL);
	g_file_set_contents(config_alarms, data, -1, NULL);
	g_free(data);
}

void
remove_alarm_dialog(void)
{
	gchar *current = check_current_selected();
	gchar *name = g_key_file_get_string(loaded_alarms, current, "Title", NULL);
	gint result;
	GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW(main_window),
												GTK_DIALOG_DESTROY_WITH_PARENT,
												GTK_MESSAGE_QUESTION,
												GTK_BUTTONS_YES_NO, NULL);



	gtk_message_dialog_set_markup(GTK_MESSAGE_DIALOG(dialog),
								  _("<b>Are you sure?</b>"));
	gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog),
												 _("Are you sure you want to remove alarm %s?"), name);


	result = gtk_dialog_run(GTK_DIALOG(dialog));

	if (result == GTK_RESPONSE_YES)
	{
		remove_alarm(current);
		reload_alarms();
		update_list_entries();
		update_alarm_thread(FALSE);
		change_selection();
	}

	gtk_widget_destroy(GTK_WIDGET(dialog));

	g_free(current);
	g_free(name);


}

gchar *
check_modify_template_selected(void)
{
	GtkWidget *treeview = GTK_WIDGET (gtk_builder_get_object (gxml, "templates_tree"));
	GtkTreeSelection *selection;
	GtkTreeModel *model = GTK_TREE_MODEL(modify_templates_store);
	gchar *name;
	GtkTreeIter iter;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
	if (gtk_tree_selection_get_selected(selection, &model, &iter))
	{
		gtk_tree_model_get (GTK_TREE_MODEL(modify_templates_store), &iter, 1, &name, -1);
	    return name;
	}
	else
		return NULL;

	/* Just in case */
	return NULL;
}


void
modify_template_row_activated(void)
{
	gchar *alarm = check_modify_template_selected();
	GtkWidget *apply_template = GTK_WIDGET (gtk_builder_get_object (gxml, "apply_template_button"));

	if (g_strcmp0(alarm, NULL) != 0)
	{
		gtk_widget_set_sensitive(GTK_WIDGET(apply_template), TRUE);
	}
	else
	{
		gtk_widget_set_sensitive(GTK_WIDGET(apply_template), FALSE);
	}
	g_free(alarm);
}

void
apply_current_template(void)
{
	gchar *alarm = check_modify_template_selected();
	GtkWidget *notebook = GTK_WIDGET (gtk_builder_get_object (gxml, "notebook2"));


	GKeyFile *temp_key = g_key_file_new();
	g_key_file_load_from_file(temp_key, config_templates, G_KEY_FILE_NONE, NULL);

	g_key_file_free(current_key);

	current_key = g_key_file_new();

	gchar **keys = g_key_file_get_keys(temp_key, alarm, NULL, NULL);


	int count = 0;

	gchar *buffer;

	while (TRUE)
	{
		if (keys[count] == NULL) break;
		buffer = g_key_file_get_value(temp_key, alarm, keys[count], NULL);
		g_key_file_set_value(current_key, untitled_name, keys[count], buffer);
		g_free(buffer);
		count++;
	}

	g_strfreev(keys);

	load_current_key();


	gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 1);
	g_free(alarm);
	g_key_file_free(temp_key);
}
