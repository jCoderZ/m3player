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

#include <stdio.h>

#include "last_change.h"

const gchar *LAST_CHANGE_HEADER = "<Event xmlns=\"urn:schemas-upnp-org:metadata-1-0/AVT/\"><InstanceID val=\"0\">";
const gchar *LAST_CHANGE_FOOTER = "</InstanceID></Event>";

GString*
last_change_new ()
{
    GString *change = g_string_new(LAST_CHANGE_HEADER);
    return change;
}

GString*
last_change_append_gchar (GString *last_change, const gchar *key, const gchar *value)
{
    if (value != NULL)
    {
        g_string_append(last_change, "<");
        g_string_append(last_change, key);
        g_string_append(last_change, " val=\"");
        g_string_append(last_change, value);
        g_string_append(last_change, "\"/>");
    }
    return last_change;
}

GString*
last_change_append_gstring (GString *last_change, const gchar *key, GString *value)
{
    GString *result = NULL;
    if (last_change != NULL && value != NULL)
    {
        result = last_change_append_gchar(last_change, key, (const gchar*) value->str);
    }
    return result;
}

GString*
last_change_append_int (GString *last_change, const gchar *key, long int value)
{
    GString *result = NULL;
    if (last_change != NULL)
    {
        gchar str[20];
        sprintf(str, "%ld", value);
        result = last_change_append_gchar(last_change, key, str);
    }
    return result;
}

gchar*
last_change_free (GString *last_change)
{
    g_string_append(last_change, LAST_CHANGE_FOOTER);
    gchar *result = last_change->str;
    g_string_free(last_change, FALSE);
    return result;
}

