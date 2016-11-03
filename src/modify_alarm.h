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

GKeyFile *current_key;
gchar *untitled_name;
GtkListStore *modify_templates_store;
gboolean load, unload_boxes;

void
modify_alarm(gchar *name);

void
add_alarm_dialog(void);

void
set_alarm_properties(gchar *name);

void
remove_alarm(gchar *current);

void
remove_alarm_dialog(void);

void
translate_keys(GKeyFile *from, GKeyFile *to);

void
set_time_format(void);