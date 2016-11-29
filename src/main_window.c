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

#include "translate.h"
#include "main_list.h"
#include "main_window.h"
#include "configuration.h"
#include "templates.h"
#include "alarm_runner.h"
#include "alarm_thread.h"
#include "common.h"
#include "modify_alarm.h"
#include "schedule.h"

void
check_missed_alarms(void)
{
	GKeyFile *key = g_key_file_new();
	gchar **groups;
	gsize num_groups;
	g_key_file_load_from_file(key, config_missed, G_KEY_FILE_NONE, NULL);

	groups = g_key_file_get_groups(key, &num_groups);

	g_key_file_free(key);
	g_strfreev(groups);

	if (num_groups > 0)
	{
		gdk_threads_enter();

		GtkWidget *old_alarms_dialog = gtk_message_dialog_new_with_markup (GTK_WINDOW(main_window),
																		   GTK_DIALOG_DESTROY_WITH_PARENT,
																		   GTK_MESSAGE_WARNING,
																		   GTK_BUTTONS_CLOSE, NULL);

		gtk_message_dialog_set_markup(GTK_MESSAGE_DIALOG(old_alarms_dialog), _("<b>You have old alarms</b>"));
		gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(old_alarms_dialog),
		                                         _("It seems you missed some alarms, because your old alarm list is not empty."));

		gtk_dialog_run (GTK_DIALOG (old_alarms_dialog));
		gtk_widget_destroy (old_alarms_dialog);


		gdk_threads_leave();
	}
}

void
show_menu_toggle(void)
{
	GtkWidget *menubar = GTK_WIDGET (gtk_builder_get_object (gxml, "menubar1"));
	show_menu = !show_menu;
	if (show_menu)
	{
		gtk_widget_show(GTK_WIDGET(menubar));
	}
	else
	{
		gtk_widget_hide(GTK_WIDGET(menubar));
	}


}

GtkWidget *
create_main_window(void)
{
	can_do_show_menu = TRUE;
	editing_template = FALSE;
	sound_ref = 0;
	GtkWidget *event;
	GdkColor color;
	GKeyFile *config;
	gint posx, posy, width, height;
	GtkTreeIter iter;
	event = GTK_WIDGET (gtk_builder_get_object (gxml, "main_event_box"));

	gtk_builder_connect_signals (gxml, NULL);

	main_window = GTK_WIDGET (gtk_builder_get_object (gxml, "main_window"));
	main_tree = GTK_WIDGET (gtk_builder_get_object (gxml, "main_tree_view"));
	GtkWidget *templates_set_event = GTK_WIDGET (gtk_builder_get_object (gxml, "templates_set_event"));
	GtkWidget *missed_event = GTK_WIDGET (gtk_builder_get_object (gxml, "missed_event"));
	GtkWidget *menubar = GTK_WIDGET (gtk_builder_get_object (gxml, "menubar1"));
	GtkWidget *menuitem = GTK_WIDGET (gtk_builder_get_object (gxml, "show_menu_menuitem"));
	GtkWidget *win = GTK_WIDGET (gtk_builder_get_object (gxml, "main_window"));
	GtkWidget *schedule_dates_event = GTK_WIDGET (gtk_builder_get_object (gxml, "schedule_dates_event"));
	GtkWidget *image = gtk_image_new_from_file(ALARM_CLOCK_ICON_NORMAL);
	GdkPixbuf *pixbuf = gtk_image_get_pixbuf(GTK_IMAGE(image));

	gtk_window_set_icon(GTK_WINDOW(win), pixbuf);

	GtkWidget *sound_prefs_dialog = GTK_WIDGET (gtk_builder_get_object (gxml, "sound_prefs_dialog"));
	gtk_window_set_icon(GTK_WINDOW(sound_prefs_dialog), pixbuf);

	GtkWidget *popup_prefs_dialog = GTK_WIDGET (gtk_builder_get_object (gxml, "popup_prefs_dialog"));
	gtk_window_set_icon(GTK_WINDOW(popup_prefs_dialog), pixbuf);

	GtkWidget *window_prefs_dialog = GTK_WIDGET (gtk_builder_get_object (gxml, "window_prefs_dialog"));
	gtk_window_set_icon(GTK_WINDOW(window_prefs_dialog), pixbuf);

	GtkWidget *command_prefs_dialog = GTK_WIDGET (gtk_builder_get_object (gxml, "command_prefs_dialog"));
	gtk_window_set_icon(GTK_WINDOW(command_prefs_dialog), pixbuf);

	GtkWidget *action_prefs_dialog = GTK_WIDGET (gtk_builder_get_object (gxml, "action_prefs_dialog"));
	gtk_window_set_icon(GTK_WINDOW(action_prefs_dialog), pixbuf);

	GtkWidget *sday_prefs_dialog = GTK_WIDGET (gtk_builder_get_object (gxml, "single_day_prefs"));
	gtk_window_set_icon(GTK_WINDOW(sday_prefs_dialog), pixbuf);

	GtkWidget *missed_alarms_dialog = GTK_WIDGET (gtk_builder_get_object (gxml, "missed_alarms_dialog"));
	gtk_window_set_icon(GTK_WINDOW(missed_alarms_dialog), pixbuf);

	GtkWidget *templates_dialog = GTK_WIDGET (gtk_builder_get_object (gxml, "templates_birthdays_dialog"));
	gtk_window_set_icon(GTK_WINDOW(templates_dialog), pixbuf);

	GtkWidget *counter_dialog = GTK_WIDGET (gtk_builder_get_object (gxml, "counter_editor_dialog"));
	gtk_window_set_icon(GTK_WINDOW(counter_dialog), pixbuf);

	GtkWidget *birthday_dialog = GTK_WIDGET (gtk_builder_get_object (gxml, "add_birthday_dialog"));
	gtk_window_set_icon(GTK_WINDOW(birthday_dialog), pixbuf);

	GtkWidget *prefs_dialog = GTK_WIDGET (gtk_builder_get_object (gxml, "preferences_dialog"));
	gtk_window_set_icon(GTK_WINDOW(prefs_dialog), pixbuf);

	GtkWidget *schedule_dialog = GTK_WIDGET (gtk_builder_get_object (gxml, "schedule_dialog"));
	gtk_window_set_icon(GTK_WINDOW(schedule_dialog), pixbuf);

	ampm_combo = gtk_combo_box_new_text();
	GtkWidget *ampm_container = GTK_WIDGET (gtk_builder_get_object (gxml, "alignment16"));
	gtk_combo_box_append_text (GTK_COMBO_BOX(ampm_combo), _("AM"));
	gtk_combo_box_append_text (GTK_COMBO_BOX(ampm_combo), _("PM"));
	gtk_container_add(GTK_CONTAINER(ampm_container), ampm_combo);

	gtk_widget_show(GTK_WIDGET(ampm_combo));

	/* Templates list */

	GtkWidget *templates_tree = GTK_WIDGET (gtk_builder_get_object (gxml, "templates_tre"));

	templates_store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);

	gtk_tree_view_set_model(GTK_TREE_VIEW(templates_tree), GTK_TREE_MODEL(templates_store));

	GtkTreeViewColumn *templates_column = gtk_tree_view_column_new();
	GtkCellRenderer *templates_renderer = gtk_cell_renderer_text_new();

	gtk_tree_view_column_pack_start (templates_column, templates_renderer, FALSE);

	gtk_tree_view_column_add_attribute(GTK_TREE_VIEW_COLUMN(templates_column), templates_renderer, "text", 0);

	gtk_tree_view_append_column(GTK_TREE_VIEW(templates_tree), templates_column);

	g_signal_connect (G_OBJECT (templates_tree), "cursor-changed",
					  G_CALLBACK (change_template_selection), NULL);


	/* Birthdays list */

	GtkWidget *birthdays_tree = GTK_WIDGET (gtk_builder_get_object (gxml, "birthdays_tree"));

	birthdays_store = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

	gtk_tree_view_set_model(GTK_TREE_VIEW(birthdays_tree), GTK_TREE_MODEL(birthdays_store));

	GtkTreeViewColumn *birthdays_column = gtk_tree_view_column_new();
	GtkCellRenderer *birthdays_renderer = gtk_cell_renderer_text_new();
	GtkTreeViewColumn *birthdays_date_column = gtk_tree_view_column_new();
	GtkCellRenderer *birthdays_date_renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_set_expand(GTK_TREE_VIEW_COLUMN(birthdays_column), TRUE);



	gtk_tree_view_column_pack_start (birthdays_column, birthdays_renderer, FALSE);
	gtk_tree_view_column_add_attribute(GTK_TREE_VIEW_COLUMN(birthdays_column), birthdays_renderer, "text", 0);

	gtk_tree_view_column_pack_start (birthdays_date_column, birthdays_date_renderer, FALSE);
	gtk_tree_view_column_add_attribute(GTK_TREE_VIEW_COLUMN(birthdays_date_column), birthdays_date_renderer, "text", 1);

	gtk_tree_view_append_column(GTK_TREE_VIEW(birthdays_tree), birthdays_column);
	gtk_tree_view_append_column(GTK_TREE_VIEW(birthdays_tree), birthdays_date_column);

	gtk_tree_view_column_set_title (GTK_TREE_VIEW_COLUMN(birthdays_column), _("Name"));
	gtk_tree_view_column_set_title (GTK_TREE_VIEW_COLUMN(birthdays_date_column), _("Date"));

	g_signal_connect (G_OBJECT (birthdays_tree), "cursor-changed",
					  G_CALLBACK (change_birthday_selection), NULL);


	/* Missed alarms list */

	GtkWidget *missed_tree = GTK_WIDGET (gtk_builder_get_object (gxml, "missed_alarms_tree"));

	missed_store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);

	gtk_tree_view_set_model(GTK_TREE_VIEW(missed_tree), GTK_TREE_MODEL(missed_store));

	GtkTreeViewColumn *missed_column = gtk_tree_view_column_new();
	GtkCellRenderer *missed_renderer = gtk_cell_renderer_text_new();

	gtk_tree_view_column_pack_start (missed_column, missed_renderer, FALSE);
	gtk_tree_view_column_add_attribute(GTK_TREE_VIEW_COLUMN(missed_column), missed_renderer, "text", 0);

	gtk_tree_view_append_column(GTK_TREE_VIEW(missed_tree), missed_column);

	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(missed_tree), FALSE);

	/* Schedule - dates list */
	schedule_dates_tree = GTK_WIDGET (gtk_builder_get_object (gxml, "schedule_dates_tree"));
	GtkWidget *d1 = GTK_WIDGET (gtk_builder_get_object (gxml, "schedule_dialog"));
	GtkWidget *d2 = GTK_WIDGET (gtk_builder_get_object (gxml, "schedule_date_dialog"));
	GtkWidget *s1 = GTK_WIDGET (gtk_builder_get_object (gxml, "schedule_dialog"));
	GtkWidget *s2 = GTK_WIDGET (gtk_builder_get_object (gxml, "add_alarm_dialog"));



	gtk_window_set_transient_for(GTK_WINDOW(d1), GTK_WINDOW(d2));
	gtk_window_set_transient_for(GTK_WINDOW(s1), GTK_WINDOW(s2));

	schedule_dates_store = gtk_tree_store_new(4, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_INT, G_TYPE_INT);

	gtk_tree_view_set_model(GTK_TREE_VIEW(schedule_dates_tree), GTK_TREE_MODEL(schedule_dates_store));

	GtkTreeViewColumn *schedule_date_column = gtk_tree_view_column_new();
	GtkCellRenderer *schedule_date_renderer = gtk_cell_renderer_text_new();

	gtk_tree_view_column_pack_start (schedule_date_column, schedule_date_renderer, FALSE);
	gtk_tree_view_column_add_attribute(GTK_TREE_VIEW_COLUMN(schedule_date_column), schedule_date_renderer, "markup", 0);

	gtk_tree_view_append_column(GTK_TREE_VIEW(schedule_dates_tree), schedule_date_column);

	gtk_tree_view_column_set_expand(GTK_TREE_VIEW_COLUMN(schedule_date_column), TRUE);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(schedule_dates_tree), TRUE);

	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(schedule_dates_tree), FALSE);

	gtk_tree_store_append(GTK_TREE_STORE(schedule_dates_store), &iter, NULL);
	gtk_tree_store_set(GTK_TREE_STORE(schedule_dates_store), &iter, 0, _("<b>Include</b>"), 1, FALSE, -1);
	gtk_tree_store_append(GTK_TREE_STORE(schedule_dates_store), &iter, NULL);
	gtk_tree_store_set(GTK_TREE_STORE(schedule_dates_store), &iter, 0, _("<b>Exclude</b>"), 1, FALSE, -1);

	g_signal_connect (G_OBJECT (schedule_dates_tree), "cursor_changed",
	                  G_CALLBACK (schedule_dates_cursor_change), NULL);


	/* Rest */

	color.red = 65535;
	color.green = 65535;
	color.blue = 65535;

	gtk_widget_modify_bg(GTK_WIDGET(event), GTK_STATE_NORMAL, &color);
	gtk_widget_modify_bg(GTK_WIDGET(templates_set_event), GTK_STATE_NORMAL, &color);
	gtk_widget_modify_bg(GTK_WIDGET(missed_event), GTK_STATE_NORMAL, &color);
	gtk_widget_modify_bg(GTK_WIDGET(schedule_dates_event), GTK_STATE_NORMAL, &color);
	create_list_model_and_view();


	text_size_combo = gtk_combo_box_new_text();
	GtkWidget *hbox = GTK_WIDGET (gtk_builder_get_object (gxml, "hbox11"));

	gtk_combo_box_append_text(GTK_COMBO_BOX(text_size_combo), _("Small"));
	gtk_combo_box_append_text(GTK_COMBO_BOX(text_size_combo), _("Medium"));
	gtk_combo_box_append_text(GTK_COMBO_BOX(text_size_combo), _("Large"));
	gtk_combo_box_append_text(GTK_COMBO_BOX(text_size_combo), _("Very large"));

	gtk_container_add(GTK_CONTAINER(hbox), text_size_combo);

	gtk_widget_show(GTK_WIDGET(text_size_combo));

	/* Moving the window */

	config = g_key_file_new();

	g_key_file_load_from_file(config, config_global, G_KEY_FILE_NONE, NULL);

	posx = g_key_file_get_integer(config, "WindowGeometry", "WindowPositionX", NULL);
	posy = g_key_file_get_integer(config, "WindowGeometry", "WindowPositionY", NULL);
	width = g_key_file_get_integer(config, "WindowGeometry", "WindowSizeX", NULL);
	height = g_key_file_get_integer(config, "WindowGeometry", "WindowSizeY", NULL);

	gtk_window_resize(GTK_WINDOW(main_window), width, height);
	gtk_window_move(GTK_WINDOW(main_window), posx, posy);

	show_menu = g_key_file_get_boolean(config, "Global", "ShowMenuBar", NULL);

	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(menuitem), show_menu);

	if (show_menu == TRUE)
		gtk_widget_show(GTK_WIDGET(menubar));
	else
		gtk_widget_hide(GTK_WIDGET(menubar));

	g_signal_connect (G_OBJECT (menuitem), "activate",
					  G_CALLBACK (show_menu_toggle), NULL);



	/* Alarm modify dialog templates column */

	GtkWidget *treeview = GTK_WIDGET (gtk_builder_get_object (gxml, "templates_tree"));

	GtkCellRenderer *name_renderer = gtk_cell_renderer_text_new();

	GtkTreeViewColumn *main_column = gtk_tree_view_column_new();

	gtk_tree_view_column_pack_end (main_column,
									 name_renderer, FALSE);

	gtk_tree_view_column_add_attribute  (main_column, name_renderer,
										 "markup", 0);

	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), main_column);

	set_time_format();


	/* Priority */

	GtkWidget *pri_hbox = GTK_WIDGET (gtk_builder_get_object (gxml, "hbox25"));
	priority_combo = gtk_combo_box_new_text();
	gtk_combo_box_append_text(GTK_COMBO_BOX(priority_combo), _("Low"));
	gtk_combo_box_append_text(GTK_COMBO_BOX(priority_combo), _("Medium"));
	gtk_combo_box_append_text(GTK_COMBO_BOX(priority_combo), _("High"));
	gtk_container_add(GTK_CONTAINER(pri_hbox), priority_combo);
	gtk_widget_show(GTK_WIDGET(priority_combo));

	return main_window;
}

/* TODO check method of opening url, and change to new upstream */
void
check_for_updates(void)
{
	g_spawn_command_line_async("xdg-open http://www.alarm-clock.pl/downloads", NULL);
}
