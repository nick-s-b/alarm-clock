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
 
GtkListStore *templates_store;
GtkListStore *birthdays_store;
GtkWidget *templates_tree;
GtkWidget *birthdays_tree;

gint current_groupname;
gboolean editing_template;

void
change_template_selection(void);

void
change_birthday_selection(void);

void
show_templates(void);

void
show_templates_libindicator(void);

void
reload_templates(void);