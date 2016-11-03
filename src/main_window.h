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

GtkWidget *main_window;
GtkWidget *main_tree;
GtkWidget *ampm_combo;
GtkWidget *priority_combo;

int current_x, current_y;

gboolean show_menu, can_do_show_menu;

GtkBuilder *gxml;

GtkWidget *
create_main_window(void);

GtkWidget *text_size_combo;

void
show_menu_toggle(void);

void
check_missed_alarms(void);
