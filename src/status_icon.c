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

#include "status_icon.h"
#include "common.h"
#include "main_window.h"
#include "translate.h"
#include "modify_alarm.h"
#include "templates.h"
#include "counters.h"

#include <config.h>

#ifndef APPINDICATOR

void
change_window_status(void)
{
	gboolean visible;
	
	g_object_get(G_OBJECT(main_window), "visible", &visible, NULL);
	
	if (visible)
	{
		gtk_window_get_position(GTK_WINDOW(main_window), &current_x, &current_y);
		gtk_widget_hide(GTK_WIDGET(main_window));
	}
	else
	{
		gtk_window_present(GTK_WINDOW(main_window));
		gtk_window_move(GTK_WINDOW(main_window), current_x, current_y);
	}
}



void
status_menu (GtkStatusIcon *status_icon,
			 guint button,
			 guint activate_time)

{
	gboolean visible;
	GtkWidget *menu = gtk_menu_new();
	GtkWidget *menu_separator1;
	GtkWidget *win_show;
	GtkWidget *add_alarm;
	GtkWidget *add_alarm_image;
	GtkWidget *add_counter;
	GtkWidget *menu_separator2;
	GtkWidget *birthdays_templates;
	GtkWidget *birthdays_templates_image;
	GtkWidget *missed_alarms;
	GtkWidget *missed_alarms_image;
	GtkWidget *add_counter_image;
	GtkWidget *menu_separator3;
	GtkWidget *show_menubar;
	GtkWidget *menu_separator4;
	GtkWidget *quit;

	win_show = gtk_check_menu_item_new_with_mnemonic(_("_Show main window"));
	g_object_get(G_OBJECT(main_window), "visible", &visible, NULL);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(win_show), visible);
	
	menu_separator1 = gtk_separator_menu_item_new();
	
	add_alarm = gtk_image_menu_item_new_with_mnemonic(_("_Add alarm..."));
	add_alarm_image = gtk_image_new_from_stock("gtk-add", GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(add_alarm), add_alarm_image);
	
	add_counter = gtk_image_menu_item_new_with_mnemonic(_("Add counter..."));
	add_counter_image = gtk_image_new_from_stock("gtk-add", GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(add_counter), add_counter_image);	

	menu_separator2 = gtk_separator_menu_item_new();
	
	birthdays_templates = gtk_image_menu_item_new_with_mnemonic(_("Birthdays & templates"));
	birthdays_templates_image = gtk_image_new_from_stock("gtk-index", GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(birthdays_templates), birthdays_templates_image);	

	missed_alarms = gtk_image_menu_item_new_with_mnemonic(_("Show missed alarms"));
	missed_alarms_image = gtk_image_new_from_stock("gtk-revert-to-saved", GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(missed_alarms), missed_alarms_image);	

	show_menubar = gtk_menu_item_new_with_mnemonic(_("_Show menu bar"));
	menu_separator4 = gtk_separator_menu_item_new();
	
	menu_separator3 = gtk_separator_menu_item_new();

	quit = gtk_image_menu_item_new_from_stock("gtk-quit", NULL);
	

	
	
	
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), win_show);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_separator1);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), add_alarm);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), add_counter);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_separator2);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), birthdays_templates);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), missed_alarms);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_separator3);

	if (!show_menu)
	{
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), show_menubar);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_separator4);
		g_signal_connect (G_OBJECT (show_menubar), "activate",
						  G_CALLBACK (show_menu_toggle), NULL);
	}
	
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), quit);

	gtk_widget_show_all(menu);
	
	g_signal_connect (G_OBJECT (win_show), "activate",
					  G_CALLBACK (change_window_status), NULL);

	g_signal_connect (G_OBJECT (add_alarm), "activate",
					  G_CALLBACK (add_alarm_dialog), NULL);

	g_signal_connect (G_OBJECT (birthdays_templates), "activate",
					  G_CALLBACK (show_templates), NULL);

	g_signal_connect (G_OBJECT (missed_alarms), "activate",
					  G_CALLBACK (show_missed_alarms), NULL);
	
	g_signal_connect (G_OBJECT (add_counter), "activate",
					  G_CALLBACK (add_counter_dialog), NULL);
	
	g_signal_connect (G_OBJECT (quit), "activate",
					  G_CALLBACK (quit_alarm_clock), NULL);
	
	gtk_menu_popup (GTK_MENU(menu), NULL, NULL,
					(GtkMenuPositionFunc)gtk_status_icon_position_menu,
					status_icon, button, activate_time);
	
	
}

void
create_status_icon(void)
{
	status_icon = gtk_status_icon_new_from_file(ALARM_CLOCK_ICON_NORMAL);

	gtk_status_icon_set_tooltip(GTK_STATUS_ICON(status_icon), _("Alarm Clock"));
	
	g_signal_connect (G_OBJECT (status_icon), "activate",
					  G_CALLBACK (change_window_status), NULL);
	
	g_signal_connect (G_OBJECT (status_icon), "popup-menu",
					  G_CALLBACK (status_menu), NULL);

}

void
change_birthday_status(gboolean on)
{
	if (on)
		gtk_status_icon_set_from_file(GTK_STATUS_ICON(status_icon), ALARM_CLOCK_ICON_CAUTION);
	else
		gtk_status_icon_set_from_file(GTK_STATUS_ICON(status_icon), ALARM_CLOCK_ICON_NORMAL);
}

void
set_status_tooltip(gchar *tooltip)
{
	gtk_status_icon_set_tooltip(GTK_STATUS_ICON(status_icon), tooltip);
}
#endif

#ifdef APPINDICATOR

void
change_window_status(void)
{
	gtk_window_present(GTK_WINDOW(main_window));
	gtk_window_move(GTK_WINDOW(main_window), current_x, current_y);

}

void
create_status_icon(void)
{
	GtkWidget *menu = gtk_menu_new();
	GtkWidget *menu_separator1;
	GtkWidget *win_show;
	GtkWidget *add_alarm;
	GtkWidget *add_alarm_image;
	GtkWidget *add_counter;
	GtkWidget *menu_separator2;
	GtkWidget *birthdays_templates;
	GtkWidget *birthdays_templates_image;
	GtkWidget *missed_alarms;
	GtkWidget *missed_alarms_image;
	GtkWidget *add_counter_image;
	GtkWidget *menu_separator3;
	GtkWidget *show_menubar;
	GtkWidget *menu_separator4;
	GtkWidget *quit;

	win_show = gtk_menu_item_new_with_mnemonic(_("_Show main window"));
	
	menu_separator1 = gtk_separator_menu_item_new();
	
	add_alarm = gtk_image_menu_item_new_with_mnemonic(_("_Add alarm..."));
	add_alarm_image = gtk_image_new_from_stock("gtk-add", GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(add_alarm), add_alarm_image);
	
	add_counter = gtk_image_menu_item_new_with_mnemonic(_("Add counter..."));
	add_counter_image = gtk_image_new_from_stock("gtk-add", GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(add_counter), add_counter_image);	

	menu_separator2 = gtk_separator_menu_item_new();
	
	birthdays_templates = gtk_image_menu_item_new_with_mnemonic(_("Birthdays & templates"));
	birthdays_templates_image = gtk_image_new_from_stock("gtk-index", GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(birthdays_templates), birthdays_templates_image);	

	missed_alarms = gtk_image_menu_item_new_with_mnemonic(_("Show missed alarms"));
	missed_alarms_image = gtk_image_new_from_stock("gtk-revert-to-saved", GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(missed_alarms), missed_alarms_image);	

	show_menubar = gtk_menu_item_new_with_mnemonic(_("_Show menu bar"));
	menu_separator4 = gtk_separator_menu_item_new();
	
	menu_separator3 = gtk_separator_menu_item_new();

	quit = gtk_image_menu_item_new_from_stock("gtk-quit", NULL);
	

	
	
	
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), win_show);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_separator1);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), add_alarm);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), add_counter);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_separator2);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), birthdays_templates);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), missed_alarms);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_separator3);

	if (!show_menu)
	{
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), show_menubar);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_separator4);
		g_signal_connect (G_OBJECT (show_menubar), "activate",
						  G_CALLBACK (show_menu_toggle), NULL);
	}
	
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), quit);

	gtk_widget_show_all(menu);
	
	g_signal_connect (G_OBJECT (win_show), "activate",
					  G_CALLBACK (change_window_status), NULL);

	g_signal_connect (G_OBJECT (add_alarm), "activate",
					  G_CALLBACK (add_alarm_dialog), NULL);

	g_signal_connect (G_OBJECT (birthdays_templates), "activate",
					  G_CALLBACK (show_templates_libindicator), NULL);

	g_signal_connect (G_OBJECT (missed_alarms), "activate",
					  G_CALLBACK (show_missed_alarms), NULL);
	
	g_signal_connect (G_OBJECT (add_counter), "activate",
					  G_CALLBACK (add_counter_dialog), NULL);
	
	g_signal_connect (G_OBJECT (quit), "activate",
					  G_CALLBACK (quit_alarm_clock), NULL);
	




	gtk_widget_show_all(GTK_WIDGET(menu));
	indicator = app_indicator_new("alarm-clock", "indicator-alarm-clock", APP_INDICATOR_CATEGORY_APPLICATION_STATUS);
	app_indicator_set_attention_icon(indicator, "indicator-alarm-caution");
	app_indicator_set_status(APP_INDICATOR(indicator), APP_INDICATOR_STATUS_ACTIVE);
	app_indicator_set_menu (APP_INDICATOR(indicator), GTK_MENU (menu));
}

void
change_birthday_status(gboolean on, gchar *text)
{
	GtkWidget *statusbar = GTK_WIDGET (gtk_builder_get_object (gxml, "statusbar1"));
	if (on)
	{
		gtk_statusbar_remove_all(GTK_STATUSBAR(statusbar), context_id);
		context_id = gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar), "Birthdays");
		app_indicator_set_status(indicator, APP_INDICATOR_STATUS_ATTENTION);
		gtk_statusbar_push(GTK_STATUSBAR(statusbar), context_id, text);
	}
	else
	{
		app_indicator_set_status(indicator, APP_INDICATOR_STATUS_ACTIVE);
		gtk_statusbar_remove_all(GTK_STATUSBAR(statusbar), context_id);
	}
}

#endif