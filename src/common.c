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
#include "translate.h"
#include "version.h"
#include "common.h"
#include "main_list.h"
#include "alarm_runner.h"

void
quit_alarm_clock(void)
{
	GKeyFile *config = g_key_file_new();
	gchar *data;
	gint posx, posy, width, height;

	g_key_file_load_from_file(config, config_global, G_KEY_FILE_NONE, NULL);

	gtk_window_get_size(GTK_WINDOW(main_window), &width, &height);
	gtk_window_get_position(GTK_WINDOW(main_window), &posx, &posy);

	g_key_file_set_integer(config, "WindowGeometry", "WindowPositionX", posx);
	g_key_file_set_integer(config, "WindowGeometry", "WindowPositionY", posy);
	g_key_file_set_integer(config, "WindowGeometry", "WindowSizeX", width);
	g_key_file_set_integer(config, "WindowGeometry", "WindowSizeY", height);
	g_key_file_set_boolean(config, "Global", "ShowMenuBar", show_menu);

	data = g_key_file_to_data(config, NULL, NULL);

	g_file_set_contents (config_global, data, -1, NULL);

	/* Need to close threads here... */

	if (notify != NULL) notify_notification_close(notify, NULL);

	g_key_file_free(config);
	g_free(data);
	gtk_main_quit();
}

void
about_alarm_clock(void)
{
	GtkWidget *about = gtk_about_dialog_new();
	GtkWidget *image = gtk_image_new_from_file(ALARM_CLOCK_ICON_NORMAL);
	GdkPixbuf *logo = gtk_image_get_pixbuf(GTK_IMAGE(image));
	gchar **authors = g_strsplit("Programming\n\tTomasz Sałaciński <tsalacinski@gmail.com>\n\tJohnathan Jenkins <twodopeshaggy@gmail.com>", "\n", -1);
	gchar **artists = g_strsplit("Icons\n\t(c) Tango Desktop Project\nUbuntu support icons\n\tBui Arantsson", "\n", -1);

	gchar *license =

	"Alarm Clock is free software; you can redistribute it and/or modify\n"
	"it under the terms of the GNU General Public License as published by\n"
	"the Free Software Foundation; either version 2 of the License, or\n"
	"(at your option) any later version.\n"
	"\n"
	"Alarm Clock is distributed in the hope that it will be useful,\n"
	"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
	"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
	"GNU General Public License for more details.\n"
	"\n"
	"You should have received a copy of the GNU General Public License\n"
	"along with Alarm Clock; if not, write to the Free Software Foundation,\n"
	"Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA\n";

	gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(about), _("Alarm Clock"));
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about), ALARM_CLOCK_VERSION);
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(about), "(c) 2008-2010 Tomasz Sałaciński <tsalacinskI@gmail.com>\n(c) 2016-2017 Johnathan Jenkins <twodopeshaggy@gmail.com>");
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(about), _("A simple alarm clock."));
	gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(about), logo);
	gtk_about_dialog_set_license(GTK_ABOUT_DIALOG(about), license);

	gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(about), (const gchar**)authors);
	gtk_about_dialog_set_artists(GTK_ABOUT_DIALOG(about), (const gchar**)artists);
	gtk_about_dialog_set_translator_credits(GTK_ABOUT_DIALOG(about), _("translator-credits"));
	gtk_about_dialog_set_website_label(GTK_ABOUT_DIALOG(about), "homepage");
	gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(about), "https://github.com/shaggytwodope/alarm-clock");

	gtk_window_set_skip_taskbar_hint(GTK_WINDOW(about), TRUE);
   	gtk_window_set_skip_pager_hint(GTK_WINDOW(about), TRUE);

	gtk_dialog_run(GTK_DIALOG(about));

	gtk_widget_destroy(image);
	gtk_widget_destroy(GTK_WIDGET(about));
}

void
test_alarm(void)
{
	GtkTreeSelection *selection;
	GtkTreeIter iter;
	GtkTreeModel *model;

	model = GTK_TREE_MODEL(store);
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(main_tree));
	if (gtk_tree_selection_get_selected(selection, &model, &iter))
	{
		gchar *name;
		gtk_tree_model_get (GTK_TREE_MODEL(model), &iter, 4, &name, -1);
		run_alarm(name);
	}
	else
	{
		return;
	}
}

void
show_missed_alarms(void)
{
	GtkWidget *dialog = GTK_WIDGET (gtk_builder_get_object (gxml, "missed_alarms_dialog"));
	GtkWidget *notebook = GTK_WIDGET (gtk_builder_get_object (gxml, "missed_alarm_notebook"));
	GtkWidget *mbutton = GTK_WIDGET (gtk_builder_get_object (gxml, "clear_missed_button"));
	GKeyFile *key = g_key_file_new();
	gchar **groups, *buffer;
	gsize num_groups;
	gint x = 0;
	g_key_file_load_from_file(key, config_missed, G_KEY_FILE_NONE, NULL);
	GtkTreeIter iter;

	groups = g_key_file_get_groups(key, &num_groups);

	gtk_list_store_clear(GTK_LIST_STORE(missed_store));




	if (num_groups > 0)
	{
		gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 0);
		for (x = 0; x < num_groups; x++)
		{
			buffer = g_key_file_get_string(key, groups[x], "Title", NULL);
			gtk_list_store_append(GTK_LIST_STORE(missed_store), &iter);
			gtk_list_store_set(GTK_LIST_STORE(missed_store), &iter, 0, buffer, 1, groups[x], -1);
			gtk_widget_set_sensitive(GTK_WIDGET(mbutton), TRUE);
			g_free(buffer);
		}
	}
	else
	{

		gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 1);
		gtk_widget_set_sensitive(GTK_WIDGET(mbutton), FALSE);
	}

	g_strfreev(groups);
	g_key_file_free(key);
	gtk_widget_show(GTK_WIDGET(dialog));
}

void
hide_missed(void)
{
	GtkWidget *dialog = GTK_WIDGET (gtk_builder_get_object (gxml, "missed_alarms_dialog"));
	gtk_widget_hide(GTK_WIDGET(dialog));
}

void
clear_missed_alarms(void)
{
	GtkWidget *notebook = GTK_WIDGET (gtk_builder_get_object (gxml, "missed_alarm_notebook"));
	GtkWidget *mbutton = GTK_WIDGET (gtk_builder_get_object (gxml, "clear_missed_button"));
	gtk_list_store_clear(GTK_LIST_STORE(missed_store));
	g_remove(config_missed);
	g_file_set_contents(config_missed, "", -1, NULL);
	gtk_widget_set_sensitive(GTK_WIDGET(mbutton), FALSE);
	gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 1);
}
