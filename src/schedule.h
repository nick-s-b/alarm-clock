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
 
GtkListStore *schedule_exceptions_store;
GtkTreeStore *schedule_dates_store;
GtkWidget *schedule_dates_tree;
gboolean date_exists;
guint ex_day, ex_mon, included, excluded;
GString *dates_include, *dates_exclude;

void
schedule_dates_cursor_change();

void
widget_selector_single(gchar *name, gint number, gboolean select);

void
deselect_all_months(void);

void
deselect_all_weekdays(void);