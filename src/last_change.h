/*
 * Copyright (C) 2011 - jCoderz.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LASTCHANGE_H
#define LASTCHANGE_H 1

#include <glib.h>

GString*
last_change_new ();

GString*
last_change_append_gchar (GString *last_change, const gchar *key, const gchar *value);

GString*
last_change_append_gstring (GString *last_change, const gchar *key, GString *value);

GString*
last_change_append_int (GString *last_change, const gchar *key, long int value);

gchar*
last_change_free (GString *last_change);

#endif
