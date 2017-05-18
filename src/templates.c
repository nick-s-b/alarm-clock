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
#include "templates.h"
#include "translate.h"
#include "modify_alarm.h"
#include "alarm_thread.h"
#include "config.h"

gchar *format_date(gint year, gint month, gint day)
{
	gchar *m, *d, *result;

	if (month < 10)
		m = g_strdup_printf("0%i", month);
	else
		m = g_strdup_printf("%i", month);

	if (day < 10)
		d = g_strdup_printf("0%i", day);
	else
		d = g_strdup_printf("%i", day);

	result = g_strdup_printf("%i-%s-%s", year, m, d);

	g_free(m);
	g_free(d);

	return result;

}

void reload_templates(void)
{
	GtkWidget *templates_notebook =
	    GTK_WIDGET(gtk_builder_get_object(gxml, "templates_notebook"));
	gint templates_num = 0;
	gchar **templates_content;
	GtkTreeIter iter;
	gchar *buffer;
	gtk_list_store_clear(GTK_LIST_STORE(templates_store));

	gtk_tree_view_set_model(GTK_TREE_VIEW(templates_tree), NULL);

	GKeyFile *templates = g_key_file_new();

	g_key_file_load_from_file(templates, config_templates, G_KEY_FILE_NONE,
				  NULL);

	templates_content = g_key_file_get_groups(templates, NULL);

	while (TRUE) {
		if (templates_content[templates_num] == NULL)
			break;
		buffer =
		    g_key_file_get_string(templates,
					  templates_content[templates_num],
					  "Title", NULL);
		gtk_list_store_append(GTK_LIST_STORE(templates_store), &iter);
		gtk_list_store_set(GTK_LIST_STORE(templates_store), &iter, 0,
				   buffer, 1, templates_content[templates_num],
				   -1);
		g_free(buffer);

		templates_num++;
	}

	if (templates_num > 0)
		gtk_notebook_set_current_page(GTK_NOTEBOOK(templates_notebook),
					      1);
	else
		gtk_notebook_set_current_page(GTK_NOTEBOOK(templates_notebook),
					      0);

	gtk_tree_view_set_model(GTK_TREE_VIEW(templates_tree),
				GTK_TREE_MODEL(templates_store));

	g_strfreev(templates_content);
	g_key_file_free(templates);
	change_template_selection();
}

void reload_birthdays(void)
{
	GtkWidget *birthdays_notebook =
	    GTK_WIDGET(gtk_builder_get_object(gxml, "birthdays_notebook"));
	gint birthdays_num = 0;
	gchar **birthdays_content, *name = "", *date = "";
	GtkTreeIter iter;

	gtk_list_store_clear(GTK_LIST_STORE(birthdays_store));

	gtk_tree_view_set_model(GTK_TREE_VIEW(birthdays_tree), NULL);

	GKeyFile *birthdays = g_key_file_new();

	g_key_file_load_from_file(birthdays, config_birthdays, G_KEY_FILE_NONE,
				  NULL);

	birthdays_content = g_key_file_get_groups(birthdays, NULL);

	while (TRUE) {
		if (birthdays_content[birthdays_num] == NULL)
			break;
		name =
		    g_key_file_get_string(birthdays,
					  birthdays_content[birthdays_num],
					  "Name", NULL);
		date =
		    g_key_file_get_string(birthdays,
					  birthdays_content[birthdays_num],
					  "Date", NULL);
		gtk_list_store_append(GTK_LIST_STORE(birthdays_store), &iter);
		gtk_list_store_set(GTK_LIST_STORE(birthdays_store), &iter, 0,
				   name, 1, date, 2,
				   birthdays_content[birthdays_num], -1);
		g_free(name);
		g_free(date);
		birthdays_num++;
	}

	if (birthdays_num > 0)
		gtk_notebook_set_current_page(GTK_NOTEBOOK(birthdays_notebook),
					      1);
	else
		gtk_notebook_set_current_page(GTK_NOTEBOOK(birthdays_notebook),
					      0);

	gtk_tree_view_set_model(GTK_TREE_VIEW(birthdays_tree),
				GTK_TREE_MODEL(birthdays_store));

	g_strfreev(birthdays_content);
	g_key_file_free(birthdays);
	change_birthday_selection();

}

void add_birthday_cancel(void)
{
	GtkWidget *add_birthday_dialog =
	    GTK_WIDGET(gtk_builder_get_object(gxml, "add_birthday_dialog"));
	gtk_widget_hide(GTK_WIDGET(add_birthday_dialog));
	current_groupname = 0;
}

void add_birthday_ok(void)
{
	GtkWidget *add_birthday_dialog =
	    GTK_WIDGET(gtk_builder_get_object(gxml, "add_birthday_dialog"));
	GtkWidget *name_entry =
	    GTK_WIDGET(gtk_builder_get_object(gxml, "birthday_name_entry"));
	GtkWidget *calendar =
	    GTK_WIDGET(gtk_builder_get_object(gxml, "birthday_calendar"));
	gint result = 0;
	guint year, month, day;
	gchar *keydata, *groupname, *text, *date;
	GKeyFile *birth_key = g_key_file_new();

	text = (gchar *) gtk_entry_get_text(GTK_ENTRY(name_entry));

	if (g_strcmp0(text, "") == 0) {
		GtkWidget *dialog =
		    gtk_message_dialog_new(GTK_WINDOW(add_birthday_dialog),
					   GTK_DIALOG_DESTROY_WITH_PARENT,
					   GTK_MESSAGE_ERROR,
					   GTK_BUTTONS_CLOSE,
					   _("You need to provide the name."));
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);

		return;
	}

	gtk_calendar_get_date(GTK_CALENDAR(calendar), &year, &month, &day);
	month++;
	if (current_groupname == 0) {
		result = g_random_int();
		if (result < 0) {
			result = result * -1;
		}

		groupname = g_strdup_printf("%i", result);
	} else {
		groupname = g_strdup_printf("%i", current_groupname);
	}

	g_key_file_load_from_file(birth_key, config_birthdays, G_KEY_FILE_NONE,
				  NULL);

	text = (gchar *) gtk_entry_get_text(GTK_ENTRY(name_entry));
	date = format_date(year, month, day);
	g_key_file_set_string(birth_key, groupname, "Name", text);
	g_key_file_set_string(birth_key, groupname, "Date", date);

	keydata = g_key_file_to_data(birth_key, NULL, NULL);

	g_file_set_contents(config_birthdays, keydata, -1, NULL);

	gtk_widget_hide(GTK_WIDGET(add_birthday_dialog));

	/* Not freeing "text" as it belongs to entry widget */

	g_key_file_free(birth_key);
	g_free(keydata);
	g_free(groupname);
	g_free(date);

	reload_birthdays();
	check_current_birthdays();
	current_groupname = 0;
}

void add_birthday(void)
{
	GtkWidget *add_birthday_dialog =
	    GTK_WIDGET(gtk_builder_get_object(gxml, "add_birthday_dialog"));
	GtkWidget *calendar =
	    GTK_WIDGET(gtk_builder_get_object(gxml, "birthday_calendar"));
	GtkWidget *name_entry =
	    GTK_WIDGET(gtk_builder_get_object(gxml, "birthday_name_entry"));
	gtk_widget_grab_focus(GTK_WIDGET(name_entry));
	guint year, month, day;
	GKeyFile *key = g_key_file_new();
	time_t rawtime;
	struct tm *timeinfo;
	gchar *iso_date = "", **split = NULL, **date_split = NULL, *buffer;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	g_key_file_load_from_file(key, config_birthdays, G_KEY_FILE_NONE, NULL);

	gtk_entry_set_text(GTK_ENTRY(name_entry), "");

	if (current_groupname == 0) {
		year = timeinfo->tm_year + 1900;
		month = timeinfo->tm_mon + 1;
		day = timeinfo->tm_mday;

		gtk_calendar_select_month(GTK_CALENDAR(calendar), month - 1,
					  year);
		gtk_calendar_select_day(GTK_CALENDAR(calendar), day);
	} else {
		iso_date = g_strdup_printf("%i", current_groupname);
		buffer = g_key_file_get_string(key, iso_date, "Name", NULL);
		gtk_entry_set_text(GTK_ENTRY(name_entry), g_strdup(buffer));
		g_free(buffer);

		buffer = g_key_file_get_string(key, iso_date, "Date", NULL);

		split = g_strsplit(buffer, "T", -1);
		date_split = g_strsplit(split[0], "-", -1);

		year = g_ascii_strtoull(date_split[0], NULL, 10);
		month = g_ascii_strtoull(date_split[1], NULL, 10);
		day = g_ascii_strtoull(date_split[2], NULL, 10);

		gtk_calendar_select_month(GTK_CALENDAR(calendar), month - 1,
					  year);
		gtk_calendar_select_day(GTK_CALENDAR(calendar), day);
		g_free(iso_date);
		g_strfreev(split);
		g_strfreev(date_split);

	}

	gtk_widget_show(GTK_WIDGET(add_birthday_dialog));
}

gchar *check_birthday_selected(void)
{
	GtkTreeSelection *selection;
	GtkTreeModel *model = GTK_TREE_MODEL(birthdays_store);
	gchar *name;
	GtkTreeIter iter;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(birthdays_tree));
	if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
		gtk_tree_model_get(GTK_TREE_MODEL(birthdays_store), &iter, 2,
				   &name, -1);
		return name;
	} else
		return NULL;

	/* Just in case */
	return NULL;
}

gchar *check_template_selected(void)
{
	GtkTreeSelection *selection;
	GtkTreeModel *model = GTK_TREE_MODEL(templates_store);
	gchar *name;
	GtkTreeIter iter;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(templates_tree));
	if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
		gtk_tree_model_get(GTK_TREE_MODEL(templates_store), &iter, 1,
				   &name, -1);
		return name;
	} else
		return NULL;

	/* Just in case */
	return NULL;
}

void change_birthday_selection(void)
{
	GtkWidget *remove_birthday =
	    GTK_WIDGET(gtk_builder_get_object(gxml, "remove_birthday_button"));
	GtkWidget *birthday_props =
	    GTK_WIDGET(gtk_builder_get_object
		       (gxml, "birthday_properties_button"));
	gchar *birth = check_birthday_selected();

	if (g_strcmp0(birth, NULL) != 0) {
		gtk_widget_set_sensitive(GTK_WIDGET(remove_birthday), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(birthday_props), TRUE);
	} else {
		gtk_widget_set_sensitive(GTK_WIDGET(remove_birthday), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(birthday_props), FALSE);
	}

}

void change_template_selection(void)
{
	GtkWidget *remove_template =
	    GTK_WIDGET(gtk_builder_get_object(gxml, "remove_template_button"));
	GtkWidget *template_props =
	    GTK_WIDGET(gtk_builder_get_object
		       (gxml, "template_properties_button"));
	gchar *birth = check_template_selected();

	if (g_strcmp0(birth, NULL) != 0) {
		gtk_widget_set_sensitive(GTK_WIDGET(remove_template), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(template_props), TRUE);
	} else {
		gtk_widget_set_sensitive(GTK_WIDGET(remove_template), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(template_props), FALSE);
	}
}

void show_templates_libindicator(void)
{
	/* Why do I have to do this? */

	gdk_threads_enter();
	show_templates();
	gdk_threads_leave();
}

void show_templates(void)
{
	GtkWidget *dialog =
	    GTK_WIDGET(gtk_builder_get_object
		       (gxml, "templates_birthdays_dialog"));

	GtkWidget *templates_event =
	    GTK_WIDGET(gtk_builder_get_object(gxml, "templates_event"));
	GtkWidget *birthdays_event =
	    GTK_WIDGET(gtk_builder_get_object(gxml, "birthdays_event"));
	templates_tree =
	    GTK_WIDGET(gtk_builder_get_object(gxml, "templates_tre"));
	birthdays_tree =
	    GTK_WIDGET(gtk_builder_get_object(gxml, "birthdays_tree"));

	current_groupname = 0;

	GdkColor color;

	reload_templates();
	reload_birthdays();

	color.red = 65535;
	color.green = 65535;
	color.blue = 65535;

	gtk_widget_modify_bg(GTK_WIDGET(templates_event), GTK_STATE_NORMAL,
			     &color);
	gtk_widget_modify_bg(GTK_WIDGET(birthdays_event), GTK_STATE_NORMAL,
			     &color);

	change_birthday_selection();
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_hide(GTK_WIDGET(dialog));
}

void remove_birthday(void)
{
	gchar *number = check_birthday_selected();
	GKeyFile *key = g_key_file_new();
	gchar *keydata, *buffer, *markup;
	gint result = 0;
	GtkWidget *add_birthday_dialog =
	    GTK_WIDGET(gtk_builder_get_object
		       (gxml, "templates_birthdays_dialog"));

	g_key_file_load_from_file(key, config_birthdays, G_KEY_FILE_NONE, NULL);

	buffer = g_key_file_get_string(key, number, "Name", NULL);

	markup =
	    g_strdup_printf(_
			    ("Are you sure you want to remove birthday for %s?"),
			    buffer);

	GtkWidget *dialog =
	    gtk_message_dialog_new_with_markup(GTK_WINDOW(add_birthday_dialog),
					       GTK_DIALOG_DESTROY_WITH_PARENT,
					       GTK_MESSAGE_QUESTION,
					       GTK_BUTTONS_YES_NO, NULL);

	gtk_message_dialog_set_markup(GTK_MESSAGE_DIALOG(dialog),
				      "<b>Are you sure?</b>");
	gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog),
						 markup, NULL);

	result = gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);

	if (result != GTK_RESPONSE_YES) {
		g_key_file_free(key);
		g_free(buffer);
		g_free(number);
		g_free(markup);
		check_current_birthdays();
		return;
	}

	g_key_file_remove_group(key, number, NULL);

	keydata = g_key_file_to_data(key, NULL, NULL);

	g_file_set_contents(config_birthdays, keydata, -1, NULL);

	reload_birthdays();
	check_current_birthdays();
	g_key_file_free(key);
	g_free(buffer);
	g_free(number);
	g_free(markup);
	g_free(keydata);
}

void birthday_properties(void)
{
	gchar *number = check_birthday_selected();
	current_groupname = g_ascii_strtoull(number, NULL, 10);
	add_birthday();
	check_current_birthdays();
}

void add_template_dialog(void)
{
	editing_template = TRUE;
	add_alarm_dialog();
	reload_templates();
}

void remove_template(void)
{
	gchar *number = check_template_selected();
	GKeyFile *key = g_key_file_new();
	gchar *keydata, *buffer, *markup;
	gint result = 0;
	GtkWidget *add_birthday_dialog =
	    GTK_WIDGET(gtk_builder_get_object
		       (gxml, "templates_birthdays_dialog"));

	g_key_file_load_from_file(key, config_templates, G_KEY_FILE_NONE, NULL);

	buffer = g_key_file_get_string(key, number, "Title", NULL);

	markup =
	    g_strdup_printf(_("Are you sure you want to remove template %s?"),
			    buffer);

	GtkWidget *dialog =
	    gtk_message_dialog_new_with_markup(GTK_WINDOW(add_birthday_dialog),
					       GTK_DIALOG_DESTROY_WITH_PARENT,
					       GTK_MESSAGE_QUESTION,
					       GTK_BUTTONS_YES_NO, NULL);

	gtk_message_dialog_set_markup(GTK_MESSAGE_DIALOG(dialog),
				      _("<b>Are you sure?</b>"));
	gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog),
						 markup, NULL);

	result = gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);

	if (result != GTK_RESPONSE_YES) {
		g_key_file_free(key);
		g_free(buffer);
		g_free(number);
		g_free(markup);
		return;
	}

	g_key_file_remove_group(key, number, NULL);

	keydata = g_key_file_to_data(key, NULL, NULL);

	g_file_set_contents(config_templates, keydata, -1, NULL);

	reload_templates();

	g_key_file_free(key);
	g_free(buffer);
	g_free(number);
	g_free(markup);
	g_free(keydata);
}

void template_properties(void)
{
	gchar *number = check_template_selected();
	editing_template = TRUE;
	modify_alarm(number);
}
