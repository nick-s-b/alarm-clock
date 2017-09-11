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
#include "configuration.h"
#include "alarm_thread.h"
#include "modify_alarm.h"
#include "main_window.h"
#include "counters.h"

void
activated_row(GtkTreeView * treeview,
	      GtkTreePath * path,
	      GtkTreeViewColumn * column, gpointer user_data)
{
	GtkTreeModel *model;
	GtkTreeIter iter;

	model = gtk_tree_view_get_model(treeview);

	if (gtk_tree_model_get_iter(model, &iter, path)) {
		gchar *name;

		gtk_tree_model_get(model, &iter, 4, &name, -1);

		modify_alarm(name);

		g_free(name);
	}

}

gint get_selected_path()
{
	GtkTreeSelection *selection;
	GtkTreeModel *model = GTK_TREE_MODEL(store);
	GtkTreeIter iter;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(main_tree));
	if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
		gchar *path =
		    gtk_tree_model_get_string_from_iter(GTK_TREE_MODEL(store),
							&iter);
		gint retval = g_ascii_strtoll(path, NULL, 10);
		g_free(path);
		return retval;
	} else
		return -1;
}

gchar *check_current_selected(void)
{
	GtkTreeSelection *selection;
	GtkTreeModel *model = GTK_TREE_MODEL(store);
	gchar *name;
	GtkTreeIter iter;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(main_tree));
	if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
		gtk_tree_model_get(GTK_TREE_MODEL(store), &iter, 4, &name, -1);
		return name;
	} else
		return NULL;

	return NULL;
}

void properties_clicked(void)
{
	gchar *current = check_current_selected();
	modify_alarm(current);
	g_free(current);
}

gboolean
main_list_popup(GtkWidget * tree, GdkEventButton * event, gpointer userdata)
{
	if (event->button != 3)
		return FALSE;

	GtkWidget *menu = gtk_menu_new();

	GtkWidget *add_new_alarm =
	    gtk_image_menu_item_new_with_mnemonic(_("_Add new alarm"));
	GtkWidget *add_new_alarm_image =
	    gtk_image_new_from_stock("gtk-add", GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(add_new_alarm),
				      add_new_alarm_image);

	GtkWidget *add_new_counter =
	    gtk_image_menu_item_new_with_mnemonic(_("_Add new counter"));
	GtkWidget *add_new_counter_image =
	    gtk_image_new_from_stock("gtk-add", GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(add_new_counter),
				      add_new_counter_image);

	GtkWidget *menu_separator_1 = gtk_separator_menu_item_new();

	GtkWidget *remove_alarm =
	    gtk_image_menu_item_new_with_mnemonic(_("_Remove this alarm"));
	GtkWidget *remove_alarm_image =
	    gtk_image_new_from_stock("gtk-remove", GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(remove_alarm),
				      remove_alarm_image);

	GtkWidget *menu_separator_2 = gtk_separator_menu_item_new();

	GtkWidget *properties =
	    gtk_image_menu_item_new_from_stock("gtk-properties", NULL);
	GtkWidget *menu_separator_3 = gtk_separator_menu_item_new();
	if (!show_menu) {
		show_menu_popup_menuitem =
		    gtk_check_menu_item_new_with_mnemonic(_("_Show menu bar"));
	}

	gtk_menu_shell_append(GTK_MENU_SHELL(menu), add_new_alarm);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), add_new_counter);

	if (check_current_selected() != NULL) {
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_separator_1);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), remove_alarm);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_separator_2);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), properties);
	}

	if (!show_menu) {
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_separator_3);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu),
				      show_menu_popup_menuitem);

		g_signal_connect_after(G_OBJECT(show_menu_popup_menuitem),
				       "activate", G_CALLBACK(show_menu_toggle),
				       NULL);
	}

	gtk_widget_show_all(GTK_WIDGET(menu));

	g_signal_connect_after(G_OBJECT(add_new_alarm), "activate",
			       G_CALLBACK(add_alarm_dialog), NULL);

	g_signal_connect_after(G_OBJECT(properties), "activate",
			       G_CALLBACK(properties_clicked), NULL);

	g_signal_connect_after(G_OBJECT(remove_alarm), "activate",
			       G_CALLBACK(remove_alarm_dialog), NULL);

	g_signal_connect_after(G_OBJECT(add_new_counter), "activate",
			       G_CALLBACK(add_counter_dialog), NULL);

	gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, event->button,
		       gdk_event_get_time((GdkEvent *) event));

	return FALSE;
}

void create_list_model_and_view(void)
{
	GtkCellRenderer *name_renderer = gtk_cell_renderer_text_new();
	GtkCellRenderer *type_renderer = gtk_cell_renderer_pixbuf_new();
	GtkCellRenderer *type_name_renderer = gtk_cell_renderer_text_new();
	GtkCellRenderer *date_renderer = gtk_cell_renderer_text_new();
	GtkCellRenderer *today_renderer = gtk_cell_renderer_pixbuf_new();
	GtkCellRenderer *timeleft_renderer = gtk_cell_renderer_text_new();

	GtkTreeViewColumn *time_left_column = gtk_tree_view_column_new();
	GtkTreeViewColumn *name_column = gtk_tree_view_column_new();
	GtkTreeViewColumn *type_column = gtk_tree_view_column_new();
	GtkTreeViewColumn *date_column = gtk_tree_view_column_new();

	gtk_tree_view_column_pack_start(name_column, name_renderer, FALSE);

	gtk_tree_view_column_add_attribute(name_column, name_renderer, "text",
					   0);
	gtk_tree_view_column_add_attribute(name_column, name_renderer,
					   "editable", 7);

	gtk_tree_view_column_set_sizing(name_column,
					GTK_TREE_VIEW_COLUMN_FIXED);
	gtk_tree_view_column_set_fixed_width(name_column, 150);
	gtk_tree_view_column_set_expand(GTK_TREE_VIEW_COLUMN(name_column),
					FALSE);

	gtk_tree_view_column_pack_start(type_column, type_name_renderer, FALSE);

	gtk_tree_view_column_pack_start(type_column, type_renderer, FALSE);

	gtk_tree_view_column_add_attribute(type_column, type_renderer, "pixbuf",
					   1);

	gtk_tree_view_column_add_attribute(type_column, type_name_renderer,
					   "markup", 2);

	gtk_tree_view_column_set_sizing(type_column,
					GTK_TREE_VIEW_COLUMN_FIXED);
	gtk_tree_view_column_set_fixed_width(type_column, 150);

	gtk_tree_view_column_pack_start(date_column, date_renderer, FALSE);

	gtk_tree_view_column_pack_start(date_column, today_renderer, FALSE);

	gtk_tree_view_column_set_sizing(date_column,
					GTK_TREE_VIEW_COLUMN_FIXED);
	gtk_tree_view_column_set_fixed_width(date_column, 100);

	gtk_tree_view_column_add_attribute(date_column, date_renderer, "markup",
					   3);

	gtk_tree_view_column_pack_start(time_left_column, timeleft_renderer,
					FALSE);

	gtk_tree_view_column_add_attribute(time_left_column, timeleft_renderer,
					   "markup", 5);

	gtk_tree_view_column_set_sizing(time_left_column,
					GTK_TREE_VIEW_COLUMN_FIXED);
	gtk_tree_view_column_set_fixed_width(time_left_column, 100);

	gtk_tree_view_column_set_title(GTK_TREE_VIEW_COLUMN(name_column),
				       _("Name"));
	gtk_tree_view_column_set_title(GTK_TREE_VIEW_COLUMN(type_column),
				       _("Alarm type"));
	gtk_tree_view_column_set_title(GTK_TREE_VIEW_COLUMN(date_column),
				       _("Date and time"));
	gtk_tree_view_column_set_title(GTK_TREE_VIEW_COLUMN(time_left_column),
				       _("Time left"));

	gtk_tree_view_column_set_min_width(date_column, 130);

	gtk_tree_view_append_column(GTK_TREE_VIEW(main_tree), name_column);
	gtk_tree_view_append_column(GTK_TREE_VIEW(main_tree), type_column);
	gtk_tree_view_append_column(GTK_TREE_VIEW(main_tree), date_column);
	gtk_tree_view_append_column(GTK_TREE_VIEW(main_tree), time_left_column);

	gtk_tree_view_column_set_expand(GTK_TREE_VIEW_COLUMN(name_column),
					TRUE);
/*	gtk_tree_view_column_set_resizable(GTK_TREE_VIEW_COLUMN(name_column), TRUE);
	gtk_tree_view_column_set_resizable(GTK_TREE_VIEW_COLUMN(type_column), TRUE);
	gtk_tree_view_column_set_resizable(GTK_TREE_VIEW_COLUMN(date_column), TRUE);
	gtk_tree_view_column_set_resizable(GTK_TREE_VIEW_COLUMN(time_left_column), TRUE);*/

	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(main_tree), TRUE);

	store = gtk_list_store_new(9, G_TYPE_STRING, GDK_TYPE_PIXBUF,
				   G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
				   G_TYPE_STRING, G_TYPE_INT, G_TYPE_BOOLEAN,
				   G_TYPE_STRING);

	gtk_tree_view_set_model(GTK_TREE_VIEW(main_tree),
				GTK_TREE_MODEL(store));

	g_signal_connect(G_OBJECT(main_tree), "cursor-changed",
			 G_CALLBACK(change_selection), NULL);

	g_signal_connect(G_OBJECT(main_tree), "row_activated",
			 G_CALLBACK(activated_row), NULL);

	g_signal_connect(G_OBJECT(main_tree), "button-press-event",
			 G_CALLBACK(main_list_popup), NULL);

}

void update_list_entries(void)
{
	GtkTreeSelection *selection =
	    gtk_tree_view_get_selection(GTK_TREE_VIEW(main_tree));
	gint path_selected = get_selected_path();
	gchar **groups;
	gsize number_of_groups;
	gint count, hour, minute, month, day, year;
	GtkTreeIter iter;
	gboolean valid;
	gint row_count = 0;
	gulong datetime;
	GKeyFile *keyfile = g_key_file_new();
	g_key_file_load_from_file(keyfile, config_global, G_KEY_FILE_NONE,
				  NULL);

	gint timetype =
	    g_key_file_get_integer(keyfile, "Global", "TimeType", NULL);

	GTimeVal current_time;
	struct tm *timeinfo;
	timeinfo = localtime((const time_t *)&current_time);

	gchar *title = "", *gtime = "", *ghour = "", *gminute = "", *gmonth =
	    "", *gday = "", *gdate = "", *buffer = "", *to_set = "";
	GtkWidget *note =
	    GTK_WIDGET(gtk_builder_get_object(gxml, "main_notebook"));
	GtkWidget *alarm_test_toolbutton =
	    GTK_WIDGET(gtk_builder_get_object(gxml, "alarm_test_toolbutton"));
	GtkWidget *alarm_properties_toolbutton =
	    GTK_WIDGET(gtk_builder_get_object
		       (gxml, "alarm_properties_toolbutton"));
	GtkWidget *remove_alarm_toolbutton =
	    GTK_WIDGET(gtk_builder_get_object(gxml, "remove_alarm_toolbutton"));
	GtkWidget *remove_alarm_menu =
	    GTK_WIDGET(gtk_builder_get_object(gxml, "remove_alarm_menuitem"));
	gboolean snoozed;
	gchar *snoozed_old;

	gtk_tree_view_set_model(GTK_TREE_VIEW(main_tree), NULL);

	gtk_list_store_clear(GTK_LIST_STORE(store));

	groups = g_key_file_get_groups(loaded_alarms, &number_of_groups);

	if (number_of_groups == 0) {
		gtk_notebook_set_current_page(GTK_NOTEBOOK(note), 1);
		gtk_widget_set_sensitive(GTK_WIDGET(alarm_test_toolbutton),
					 FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET
					 (alarm_properties_toolbutton), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(remove_alarm_toolbutton),
					 FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(remove_alarm_menu), FALSE);
		g_strfreev(groups);
		return;
	} else {
		gtk_notebook_set_current_page(GTK_NOTEBOOK(note), 0);
		gtk_widget_set_sensitive(GTK_WIDGET(alarm_test_toolbutton),
					 TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET
					 (alarm_properties_toolbutton), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(remove_alarm_toolbutton),
					 TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(remove_alarm_menu), TRUE);
	}

	for (count = 0; count < number_of_groups; count++) {
		datetime =
		    g_key_file_get_double(loaded_alarms, groups[count],
					  "DateTime", NULL);
		snoozed =
		    g_key_file_get_boolean(loaded_alarms, groups[count],
					   "Snoozed", NULL);
		snoozed_old =
		    g_key_file_get_string(loaded_alarms, groups[count],
					  "SnoozedOldTime", NULL);
		timeinfo = localtime((const time_t *)&datetime);
		hour = timeinfo->tm_hour;
		minute = timeinfo->tm_min;

		year = timeinfo->tm_year + 1900;
		month = timeinfo->tm_mon + 1;
		day = timeinfo->tm_mday;

		if (hour < 10)
			ghour = g_strdup_printf("0%i", hour);
		else
			ghour = g_strdup_printf("%i", hour);

		if (minute < 10)
			gminute = g_strdup_printf("0%i", minute);
		else
			gminute = g_strdup_printf("%i", minute);

		if (month < 10) {
			gmonth = g_strdup_printf("0%i", month);
		} else {
			gmonth = g_strdup_printf("%i", month);
		}

		if (day < 10) {
			gday = g_strdup_printf("0%i", day);
		} else {
			gday = g_strdup_printf("%i", day);
		}

		gdate = g_strdup_printf("%i-%s-%s", year, gmonth, gday);

		if (snoozed) {
			if (timetype == 12) {
				if (hour < 12 && hour > 0) {
					gtime =
					    g_strdup_printf
					    ("<i><b>%s:%s %s</b>\n<small>%s</small></i>",
					     ghour, gminute, _("AM"), gdate);
				}

				if (hour > 12) {
					gtime =
					    g_strdup_printf
					    ("<i><b>%i:%s %s</b>\n<small>%s</small></i>",
					     hour - 12, gminute, _("PM"),
					     gdate);
				}

				if (hour == 12) {
					gtime =
					    g_strdup_printf
					    ("<i><b>%i:%s %s</b>\n<small>%s</small></i>",
					     12, gminute, _("PM"), gdate);
				}
				if (hour == 24) {
					gtime =
					    g_strdup_printf
					    ("<i><b>%i:%s %s</b>\n<small>%s</small></i>",
					     12, gminute, _("AM"), gdate);
				}

				if (hour == 0) {
					gtime =
					    g_strdup_printf
					    ("<i><b>%i:%s %s</b>\n<small>%s</small></i>",
					     12, gminute, _("AM"), gdate);
				}
			} else {
				gtime =
				    g_strdup_printf
				    ("<i><b>%s:%s</b>\n<small>%s</small></i>",
				     ghour, gminute, gdate);
			}

		} else {
			if (timetype == 12) {
				if (hour < 12 && hour > 0) {
					gtime =
					    g_strdup_printf
					    ("<b>%s:%s %s</b>\n<small>%s</small>",
					     ghour, gminute, _("AM"), gdate);
				}

				if (hour > 12) {
					gtime =
					    g_strdup_printf
					    ("<b>%i:%s %s</b>\n<small>%s</small>",
					     hour - 12, gminute, _("PM"),
					     gdate);
				}

				if (hour == 12) {
					gtime =
					    g_strdup_printf
					    ("<b>%i:%s %s</b>\n<small>%s</small>",
					     12, gminute, _("PM"), gdate);
				}
				if (hour == 24) {
					gtime =
					    g_strdup_printf
					    ("<b>%i:%s %s</b>\n<small>%s</small>",
					     12, gminute, _("AM"), gdate);
				}

				if (hour == 0) {
					gtime =
					    g_strdup_printf
					    ("<b>%i:%s %s</b>\n<small>%s</small>",
					     12, gminute, _("AM"), gdate);
				}
			} else {
				gtime =
				    g_strdup_printf
				    ("<b>%s:%s</b>\n<small>%s</small>", ghour,
				     gminute, gdate);
			}
		}

		buffer =
		    g_key_file_get_string(loaded_alarms, groups[count],
					  "AlarmType", NULL);

		if (g_strcmp0(buffer, "Single") == 0) {
			if (snoozed)
				to_set = g_strdup(_("<i>Single</i>"));
			else
				to_set = g_strdup(_("Single"));
		}
		if (g_strcmp0(buffer, "Counter") == 0) {
			if (snoozed)
				to_set = g_strdup(_("<i>Counter</i>"));
			else
				to_set = g_strdup(_("Counter"));
		}
		if (g_strcmp0(buffer, "Schedule") == 0) {
			if (snoozed)
				to_set = g_strdup(_("<i>Scheduled</i>"));
			else
				to_set = g_strdup(_("Scheduled"));
		}

		if (snoozed) {
			gchar *buf =
			    g_key_file_get_string(loaded_alarms, groups[count],
						  "Title",
						  NULL);
			title =
			    g_strdup_printf("<i>%s\n<small>Snoozed</small></i>",
					    buf);
		} else {
			title =
			    g_key_file_get_string(loaded_alarms, groups[count],
						  "Title", NULL);
		}

		gtk_list_store_append(GTK_LIST_STORE(store), &iter);
		gtk_list_store_set(store, &iter, 0, title, 2, to_set, 3, gtime,
				   4, groups[count], 6, count, 7, FALSE, -1);
	}

	g_free(title);
	g_free(ghour);
	g_free(gtime);
	g_free(gminute);
	gtk_tree_view_set_model(GTK_TREE_VIEW(main_tree),
				GTK_TREE_MODEL(store));
	change_selection();

	if (count > 0 && check_current_selected() == NULL) {
		GtkTreeSelection *selection =
		    gtk_tree_view_get_selection(GTK_TREE_VIEW(main_tree));
		GtkTreePath *zero_path = gtk_tree_path_new_first();
		gtk_tree_selection_select_path(GTK_TREE_SELECTION(selection),
					       zero_path);

	}
	if (path_selected < 0)
		return;

	valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store), &iter);
	while (valid) {
		row_count++;
		valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(store), &iter);
	}
	row_count--;

	if (path_selected <= row_count) {
		gchar *buf = g_strdup_printf("%i", path_selected);
		GtkTreePath *path = gtk_tree_path_new_from_string(buf);
		gtk_tree_selection_select_path(GTK_TREE_SELECTION(selection),
					       path);
		g_free(buf);
		gtk_tree_path_free(path);
	} else if (path_selected > 0) {
		gchar *buf = g_strdup_printf("%i", path_selected - 1);
		GtkTreePath *path = gtk_tree_path_new_from_string(buf);
		gtk_tree_selection_select_path(GTK_TREE_SELECTION(selection),
					       path);
		g_free(buf);
		gtk_tree_path_free(path);
	}

}

void change_selection(void)
{
	gchar *sel = check_current_selected();
	g_free(sel);
	sel = check_current_selected();
	GtkWidget *remove =
	    GTK_WIDGET(gtk_builder_get_object(gxml, "remove_alarm_toolbutton"));
	GtkWidget *remove_menu =
	    GTK_WIDGET(gtk_builder_get_object(gxml, "remove_alarm_menuitem"));
	GtkWidget *props =
	    GTK_WIDGET(gtk_builder_get_object
		       (gxml, "alarm_properties_toolbutton"));
	GtkWidget *test =
	    GTK_WIDGET(gtk_builder_get_object(gxml, "alarm_test_toolbutton"));

	if (sel == NULL) {
		gtk_widget_set_sensitive(GTK_WIDGET(remove), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(props), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(test), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(remove_menu), FALSE);
	} else {
		gtk_widget_set_sensitive(GTK_WIDGET(remove), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(props), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(test), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(remove_menu), TRUE);
	}
}
