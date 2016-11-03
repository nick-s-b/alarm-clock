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

void
quit_alarm_clock(void);

void
show_missed_alarms(void);

GtkListStore *missed_store;

#define ALARM_CLOCK_ICON_NORMAL PACKAGE_DATA_DIR"/alarm-clock.svg"
#define ALARM_CLOCK_ICON_CAUTION PACKAGE_DATA_DIR"/alarm-caution.svg"
#define ALARM_CLOCK_DEFAULT_SOUND PACKAGE_DATA_DIR"/ring.wav"
#define GTK_BUILDER_FILE PACKAGE_DATA_DIR"/alarm-clock.ui"

/*
#define ALARM_CLOCK_ICON_NORMAL "gfx/alarm-clock.svg"
#define ALARM_CLOCK_ICON_CAUTION "gfx/alarm-caution.svg"
#define ALARM_CLOCK_DEFAULT_SOUND "/sounds/ring.wav"
#define GTK_BUILDER_FILE "interface/alarm-clock.ui"
*/
