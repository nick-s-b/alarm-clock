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

#include "configuration.h"
#include <glib/gstdio.h>

void
initialize_config(void)
{
	config_dir = g_strdup_printf("%s/alarm-clock", g_get_user_config_dir());
	config_global = g_strdup_printf("%s/global.conf", config_dir);
	config_templates = g_strdup_printf("%s/templates.conf", config_dir);
	config_alarms = g_strdup_printf("%s/alarms.conf", config_dir);
	config_birthdays = g_strdup_printf("%s/birthdays.conf", config_dir);
	config_missed = g_strdup_printf("%s/missed.conf", config_dir);

	if (!g_file_test (config_dir, G_FILE_TEST_EXISTS))
	{
		g_mkdir_with_parents(config_dir, S_IRUSR | S_IWUSR | S_IXUSR);
	}

	if (!g_file_test(config_global, G_FILE_TEST_EXISTS))
	{
		GKeyFile *key = g_key_file_new();
		g_key_file_set_integer(key, "WindowGeometry", "WindowPositionX", 0);
		g_key_file_set_integer(key, "WindowGeometry", "WindowPositionY", 0);
		g_key_file_set_integer(key, "WindowGeometry", "WindowSizeX", 770);
		g_key_file_set_integer(key, "WindowGeometry", "WindowSizeY", 400);
		g_key_file_set_integer(key, "Global", "TimeType", 24);
		g_key_file_set_boolean(key, "Global", "ShowMenuBar", TRUE);
		g_key_file_set_boolean(key, "Global", "StartMinimized", FALSE);

		gchar *keystring = g_key_file_to_data(key, NULL, NULL);

		g_key_file_free(key);

		g_file_set_contents (config_global, keystring, -1, NULL);
		g_free(keystring);
	}

	if (!g_file_test (config_alarms, G_FILE_TEST_EXISTS))
	{
		g_file_set_contents(config_alarms, "", -1, NULL);
	}

	if (!g_file_test (config_birthdays, G_FILE_TEST_EXISTS))
	{
		g_file_set_contents(config_birthdays, "", -1, NULL);
	}

	if (!g_file_test (config_templates, G_FILE_TEST_EXISTS))
	{
		g_file_set_contents(config_templates, "", -1, NULL);
	}

	if (!g_file_test (config_missed, G_FILE_TEST_EXISTS))
	{
		g_file_set_contents(config_missed, "", -1, NULL);
	}

}
