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

#include "schedule.h"
#include "translate.h"
#include "modify_alarm.h"
#include "main_window.h"

gchar *
get_month_name(gint number)
{
	if (number == 1) return _("January");
	if (number == 2) return _("February");
	if (number == 3) return _("March");
	if (number == 4) return _("April");
	if (number == 5) return _("May");
	if (number == 6) return _("June");
	if (number == 7) return _("July");
	if (number == 8) return _("August");
	if (number == 9) return _("September");
	if (number == 10) return _("October");
	if (number == 11) return _("November");
	if (number == 12) return _("December");

	return _("Invalid");

}

gboolean
check_current_schedule_date_selected(void)
{
	GtkTreeSelection *selection;
	GtkTreeModel *model = GTK_TREE_MODEL(schedule_dates_store);
	gboolean name;
	GtkTreeIter iter;
	
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(schedule_dates_tree));
	if (gtk_tree_selection_get_selected(selection, &model, &iter))
	{
		gtk_tree_model_get (GTK_TREE_MODEL(schedule_dates_store), &iter, 1, &name, -1);
	    return name;
	}
	else
		return FALSE;

	/* Just in case */
	return FALSE;
}

gboolean
check_current_schedule_date_selectedDateDay(void)
{
	GtkTreeSelection *selection;
	GtkTreeModel *model = GTK_TREE_MODEL(schedule_dates_store);
	gint name;
	GtkTreeIter iter;
	
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(schedule_dates_tree));
	if (gtk_tree_selection_get_selected(selection, &model, &iter))
	{
		gtk_tree_model_get (GTK_TREE_MODEL(schedule_dates_store), &iter, 2, &name, -1);
	    return name;
	}
	else
		return FALSE;

	/* Just in case */
	return FALSE;
}

gboolean
check_current_schedule_date_selectedDateMon(void)
{
	GtkTreeSelection *selection;
	GtkTreeModel *model = GTK_TREE_MODEL(schedule_dates_store);
	gint name;
	GtkTreeIter iter;
	
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(schedule_dates_tree));
	if (gtk_tree_selection_get_selected(selection, &model, &iter))
	{
		gtk_tree_model_get (GTK_TREE_MODEL(schedule_dates_store), &iter, 3, &name, -1);
	    return name;
	}
	else
		return FALSE;

	/* Just in case */
	return FALSE;
}


void
schedule_dates_cursor_change()
{
	GtkWidget *button = GTK_WIDGET (gtk_builder_get_object (gxml, "remove_schedule_date_button"));

	gtk_widget_set_sensitive(GTK_WIDGET(button), check_current_schedule_date_selected ());
}

gboolean remove_foreach (GtkTreeModel *model,
                         GtkTreePath *path,
                         GtkTreeIter *iter,
                         gpointer data)
{
	gint month = 0, day = 0;
	gint req_month = check_current_schedule_date_selectedDateMon();
	gint req_day = check_current_schedule_date_selectedDateDay();
	gtk_tree_model_get(GTK_TREE_MODEL(model), iter, 2, &day, 3, &month, -1);

	if (req_month == month && req_day == day)
	{
		gtk_tree_store_remove(GTK_TREE_STORE(schedule_dates_store), iter);
		return TRUE;
	}
	else 
	{
		return FALSE;
	}
}


gboolean check_include_date_foreach (GtkTreeModel *model,
                                     GtkTreePath *path,
                                     GtkTreeIter *iter,
                                     gpointer data)
{
	gint day, month;
	gboolean ok;

	gchar *string_from_iter = gtk_tree_model_get_string_from_iter(model, iter);
	gchar **split = g_strsplit(string_from_iter, ":", 2);
	gchar *buffer;
	
	gtk_tree_model_get(GTK_TREE_MODEL(model), iter, 1, &ok, 2, &day, 3, &month, -1);

	if (g_strcmp0(split[0], "0") == 0 && ok)
	{
		included++;
		if (included == 1)
			buffer = g_strdup_printf("%i/%i", day, month);
		else
			buffer = g_strdup_printf(";%i/%i", day, month);
		g_string_append(dates_include, buffer);
		g_free(buffer);
	}
	else if (g_strcmp0(split[0], "1") == 0 && ok)
	{
		excluded++;
		if (excluded == 1)
			buffer = g_strdup_printf("%i/%i", day, month);
		else
			buffer = g_strdup_printf(";%i/%i", day, month);
		g_string_append(dates_exclude, buffer);
		g_free(buffer);
	}


	g_strfreev(split);
	g_free(string_from_iter);
	return FALSE;
}


void
remove_schedule_date()
{
	GtkWidget *button = GTK_WIDGET (gtk_builder_get_object (gxml, "remove_schedule_date_button"));
	GtkTreeSelection *sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(schedule_dates_tree));
	GtkWidget *notebook = GTK_WIDGET (gtk_builder_get_object (gxml, "notebook6"));
	gtk_tree_model_foreach(GTK_TREE_MODEL(schedule_dates_store),
	                       remove_foreach, NULL);

	included = 0;
	excluded = 0;
	
	dates_include = g_string_new(NULL);
	dates_exclude = g_string_new(NULL);

	gtk_tree_model_foreach(GTK_TREE_MODEL(schedule_dates_store),
	                       check_include_date_foreach, NULL);

	g_string_free(dates_include, TRUE);
	g_string_free(dates_exclude, TRUE);

	if (included + excluded == 0)
	{
		gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 1);
	}

	if (!gtk_tree_selection_get_selected(GTK_TREE_SELECTION(sel), NULL, NULL))
		gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);
	
}

void
show_schedule_editor(void)
{
	GtkWidget *dialog = GTK_WIDGET (gtk_builder_get_object (gxml, "schedule_dialog"));
	GtkWidget *notebook = GTK_WIDGET (gtk_builder_get_object (gxml, "notebook6"));
	GtkWidget *tree = GTK_WIDGET (gtk_builder_get_object (gxml, "schedule_dates_tree"));
	gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 1);
	gchar **split, **date_split, *buffer;
	gboolean bool_buffer;
	gint i = 0;
	gint month, day;
	GtkTreeIter iter, parent;

	deselect_all_months ();
	deselect_all_weekdays ();
	
	buffer = g_key_file_get_string(current_key, untitled_name, "ScheduleWeekdays", NULL);

	split = g_strsplit(buffer, ":", 7);

	for (i = 0; i <= 7; i++)
	{
		if (g_strcmp0(split[i], NULL) == 0) break;
		if (g_strcmp0(split[i], "T") == 0)
			bool_buffer = TRUE;
		else
			bool_buffer = FALSE;

		widget_selector_single ("week", i + 1, bool_buffer);
	}

	g_strfreev(split);
	g_free(buffer);
	

	buffer = g_key_file_get_string(current_key, untitled_name, "ScheduleMonths", NULL);

	split = g_strsplit(buffer, ":", 12);

	for (i = 0; i <= 12; i++)
	{
		if (g_strcmp0(split[i], NULL) == 0) break;
		if (g_strcmp0(split[i], "T") == 0)
			bool_buffer = TRUE;
		else
			bool_buffer = FALSE;
		widget_selector_single ("month", i + 1, bool_buffer);
	}

	g_strfreev(split);
	g_free(buffer);

	buffer = g_key_file_get_string(current_key, untitled_name, "ScheduleDateInclude", NULL);

	split = g_strsplit(buffer, ";", -1);



	gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(schedule_dates_store),
	                                    &parent, "0");

	g_free(buffer);

	i = 0;
	while (TRUE)
	{
		if (g_strcmp0(split[i], NULL) == 0) break;

		date_split = g_strsplit(split[i], "/", 2);

		day = g_ascii_strtoll(date_split[0], NULL, 10);
		month = g_ascii_strtoll(date_split[1], NULL, 10);

		g_strfreev(date_split);
		
		buffer = g_strdup_printf("%s %i", get_month_name(month), day);
		
		gtk_tree_store_append(GTK_TREE_STORE(schedule_dates_store), &iter, &parent);
		gtk_tree_store_set(GTK_TREE_STORE(schedule_dates_store), &iter, 
		                   0, buffer,
		                   1, TRUE,
		                   2, day, 
		                   3, month,
		                   -1);
		gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 0);
		g_free(buffer);
		i++;
	}

	buffer = g_key_file_get_string(current_key, untitled_name, "ScheduleDateExclude", NULL);

	split = g_strsplit(buffer, ";", -1);

	gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(schedule_dates_store),
	                                    &parent, "1");

	g_free(buffer);


	i = 0;
	while (TRUE)
	{
		if (g_strcmp0(split[i], NULL) == 0) break;

		date_split = g_strsplit(split[i], "/", 2);
		
		day = g_ascii_strtoll(date_split[0], NULL, 10);
		month = g_ascii_strtoll(date_split[1], NULL, 10);

		g_strfreev(date_split);
		
		buffer = g_strdup_printf("%s %i", get_month_name(month), day);
		
		gtk_tree_store_append(GTK_TREE_STORE(schedule_dates_store), &iter, &parent);
		gtk_tree_store_set(GTK_TREE_STORE(schedule_dates_store), &iter, 
		                   0, buffer,
		                   1, TRUE,
		                   2, day, 
		                   3, month,
		                   -1);
		gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 0);
		g_free(buffer);
		i++;
	}

	gtk_tree_view_expand_all(GTK_TREE_VIEW(tree));

	
	gtk_widget_show(GTK_WIDGET(dialog));
}


void
close_schedule_editor(void)
{
	GtkWidget *dialog = GTK_WIDGET (gtk_builder_get_object (gxml, "schedule_dialog"));
	GtkTreeIter iter;
	gtk_tree_store_clear(GTK_TREE_STORE(schedule_dates_store));

	gtk_tree_store_append(GTK_TREE_STORE(schedule_dates_store), &iter, NULL);
	gtk_tree_store_set(GTK_TREE_STORE(schedule_dates_store), &iter, 0, _("<b>Include</b>"), 1, FALSE, -1);
	gtk_tree_store_append(GTK_TREE_STORE(schedule_dates_store), &iter, NULL);
	gtk_tree_store_set(GTK_TREE_STORE(schedule_dates_store), &iter, 0, _("<b>Exclude</b>"), 1, FALSE, -1);
	
	gtk_widget_hide(GTK_WIDGET(dialog));
}

gboolean
widget_checker(gchar *name, gint max)
{
	gint i = 1;

	GtkWidget *buffer_widget;
	gchar *widget_name;

	gboolean active = FALSE;
	
	for (i = 1; i <= max; i++)
	{
		widget_name = g_strdup_printf("%s%i", name, i);
		buffer_widget = GTK_WIDGET (gtk_builder_get_object (gxml, widget_name));

		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(buffer_widget)))
			active = TRUE;
		
		g_free(widget_name);
	}

	return active;

}

gboolean
hide_schedule_date(void)
{
	GtkWidget *dates_window = GTK_WIDGET (gtk_builder_get_object (gxml, "schedule_date_dialog"));
	gtk_widget_hide(GTK_WIDGET(dates_window));
	return FALSE;
}

gboolean check_foreach (GtkTreeModel *model,
                         GtkTreePath *path,
                         GtkTreeIter *iter,
                         gpointer data)
{
	gint d, m;
	gtk_tree_model_get(GTK_TREE_MODEL(model), iter, 2, &d, 3, &m, -1);
	
	if (d == ex_day && m == ex_mon)
		date_exists = TRUE;
	else
		date_exists = FALSE;
	
	return date_exists;
}

void
schedule_date_ok(void)
{
	GtkWidget *calendar = GTK_WIDGET (gtk_builder_get_object (gxml, "calendar2"));
	GtkWidget *dialog = GTK_WIDGET (gtk_builder_get_object (gxml, "schedule_date_dialog"));
	GtkWidget *radio_include = GTK_WIDGET (gtk_builder_get_object (gxml, "include_date_radio"));
	GtkWidget *radio_exclude = GTK_WIDGET (gtk_builder_get_object (gxml, "exclude_date_radio"));
	GtkWidget *notebook = GTK_WIDGET (gtk_builder_get_object (gxml, "notebook6"));
	guint day, month;
	gchar *date, *path;
	GtkTreeIter iter, parent;
	
	gtk_calendar_get_date(GTK_CALENDAR(calendar), NULL, &month, &day);

	date_exists = FALSE;

	ex_day = day;
	ex_mon = month + 1;
	
	gtk_tree_model_foreach(GTK_TREE_MODEL(schedule_dates_store),
	                       check_foreach, NULL);

	if (date_exists)
	{
		GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(dialog),
                                  GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
                                  GTK_MESSAGE_WARNING,
                                  GTK_BUTTONS_CLOSE,
                                  NULL);
		gtk_message_dialog_set_markup(GTK_MESSAGE_DIALOG(error_dialog),
									  _("<b>Cannot add date</b>"));
		gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(error_dialog),
												 _("This date already exists on the list."));
		gtk_dialog_run (GTK_DIALOG (error_dialog));
		gtk_widget_destroy (GTK_WIDGET(error_dialog));
		
		return;
	}
	
	
	date = g_strdup_printf("%s %i", get_month_name(month + 1), day);
	
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_include)))
	    path = g_strdup("0");
	else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_exclude)))
	    path = g_strdup("1");
	else
		path = g_strdup("0");

	gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(schedule_dates_store),
	                                    &parent, path);
	
	gtk_tree_store_append(GTK_TREE_STORE(schedule_dates_store), &iter, &parent);
	gtk_tree_store_set(GTK_TREE_STORE(schedule_dates_store), &iter, 
	                   0, date,
	                   1, TRUE,
	                   2, day, 
	                   3, month + 1,
	                   -1);

	hide_schedule_date();

	gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 0);
	gtk_tree_view_expand_all(GTK_TREE_VIEW(schedule_dates_tree));
	
	g_free(date);
	g_free(path);
}

void
add_single_date(void)
{
	GDate *date = g_date_new();
	g_date_set_time_t (date, time (NULL)); 
	GtkWidget *dates_window = GTK_WIDGET (gtk_builder_get_object (gxml, "schedule_date_dialog"));
	GtkWidget *radio = GTK_WIDGET (gtk_builder_get_object (gxml, "include_date_radio"));

	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio), TRUE);

	gtk_widget_show(GTK_WIDGET(dates_window));
	g_date_free(date);
}


void
schedule_editor_ok(void)
{
	GString *schedule_weekdays = g_string_new(NULL);
	GString *schedule_months = g_string_new(NULL);
	gchar *widget_name = "";
	GtkWidget *buffer_widget;
	gboolean ok = FALSE;
	gint i = 1;
	gchar *error_message = g_strdup(_("You need to select at least one month and day or include one date to the schedule."));
	
	GtkWidget *dialog = GTK_WIDGET (gtk_builder_get_object (gxml, "schedule_dialog"));

	included = 0;
	excluded = 0;

	dates_include = g_string_new(NULL);
	dates_exclude = g_string_new(NULL);

	gtk_tree_model_foreach(GTK_TREE_MODEL(schedule_dates_store),
	                       check_include_date_foreach, NULL);

	if (included) ok = TRUE;
	if (widget_checker("week", 7) && widget_checker("month", 12)) ok = TRUE;
	if (included && widget_checker("week", 7) && !widget_checker("month", 12)) ok = FALSE;
	if (included && !widget_checker("week", 7) && widget_checker("month", 12)) ok = FALSE;
	
	if (!ok)
	{
		GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(dialog),
                                  GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
                                  GTK_MESSAGE_WARNING,
                                  GTK_BUTTONS_CLOSE,
                                  NULL);
		gtk_message_dialog_set_markup(GTK_MESSAGE_DIALOG(error_dialog),
									  _("<b>Cannot proceed</b>"));
		gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(error_dialog),
		                                         "%s", error_message);
		gtk_dialog_run (GTK_DIALOG (error_dialog));
		gtk_widget_destroy (GTK_WIDGET(error_dialog));
		g_free(error_message);
		
		return;
	}

	/* Checking months */

	for (i = 1; i <= 7; i++)
	{
		widget_name = g_strdup_printf("week%i", i);
		buffer_widget = GTK_WIDGET (gtk_builder_get_object (gxml, widget_name));
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(buffer_widget)))
		{
			if (i < 7)
				g_string_append(schedule_weekdays, "T:");
			else
				g_string_append(schedule_weekdays, "T");
		}
		else
		{
			if (i < 7)
				g_string_append(schedule_weekdays, "F:");
			else
				g_string_append(schedule_weekdays, "F");
		}
		g_free(widget_name);
	}

	for (i = 1; i <= 12; i++)
	{
		widget_name = g_strdup_printf("month%i", i);
		buffer_widget = GTK_WIDGET (gtk_builder_get_object (gxml, widget_name));
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(buffer_widget)))
		{
			if (i < 12)
				g_string_append(schedule_months, "T:");
			else
				g_string_append(schedule_months, "T");
		}
		else
		{
			if (i < 12)
				g_string_append(schedule_months, "F:");
			else
				g_string_append(schedule_months, "F");
		}
		g_free(widget_name);
	}



	g_key_file_set_string(current_key, untitled_name, "ScheduleWeekdays", schedule_weekdays->str);
	g_key_file_set_string(current_key, untitled_name, "ScheduleMonths", schedule_months->str);
	g_key_file_set_string(current_key, untitled_name, "ScheduleDateInclude", dates_include->str);
	g_key_file_set_string(current_key, untitled_name, "ScheduleDateExclude", dates_exclude->str);
	g_key_file_set_string(current_key, untitled_name, "AlarmType", "Schedule");

	g_string_free(schedule_weekdays, TRUE);
	g_string_free(schedule_months, TRUE);

	g_string_free(dates_include, TRUE);
	g_string_free(dates_exclude, TRUE);
	close_schedule_editor();
}

void
widget_selector(gchar *name, gint max, gboolean select)
{
	gint i = 1;

	GtkWidget *buffer_widget;
	gchar *widget_name;
	
	for (i = 1; i <= max; i++)
	{
		widget_name = g_strdup_printf("%s%i", name, i);
		buffer_widget = GTK_WIDGET (gtk_builder_get_object (gxml, widget_name));

		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(buffer_widget), select);
		
		g_free(widget_name);
	}

}

void
widget_selector_single(gchar *name, gint number, gboolean select)
{
	GtkWidget *buffer_widget;
	gchar *widget_name;
	
	widget_name = g_strdup_printf("%s%i", name, number);
	
	buffer_widget = GTK_WIDGET (gtk_builder_get_object (gxml, widget_name));

	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(buffer_widget), select);
	g_free(widget_name);
}


void
select_all_weekdays(void)
{
	widget_selector("week", 7, TRUE);
}

void
deselect_all_weekdays(void)
{
	widget_selector("week", 7, FALSE);
}

void
select_all_months(void)
{
	widget_selector("month", 12, TRUE);
}

void
deselect_all_months(void)
{
	widget_selector("month", 12, FALSE);
}
