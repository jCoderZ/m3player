/*
 * Copyright (C) 2011 - Michael Rumpf
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

#include "presets.h"

#include <stdio.h>
#include <signal.h>
#include <string.h>

#define LINE_BUFFER_SIZE 8192

static const char presets_list_filename[] = "presets.list";
static const char presets_last_filename[] = "preset.last";

GString *presets_last = NULL;
GList *presets = NULL;
gint presets_current_index = -1;

gint
presets_read_list (GString *last)
{
    gint index = -1;
    gint previous_index = -1;

    FILE *file = fopen ( presets_list_filename, "r" );
    if (file != NULL)
    {
        char line[LINE_BUFFER_SIZE];

        while (fgets (line, sizeof line, file) != NULL)
        {
            size_t len = strcspn (line, "\n");
            line[len] = '\0';

            if (strcmp (line, last->str) == 0)
            {
                g_debug ("Found preset at index %d: %s", index + 1, line);
                previous_index = index;
            }

            index++;

            GString *url = g_string_new (line);
            presets = g_list_append (presets, url);

            g_debug ("list[%d]: '%s'", index, line);
        }

        fclose (file);
    }
    else
    {
        g_warning ("Could not read %s", presets_list_filename);
    }

    return previous_index;
}

GString*
presets_read_last ()
{
    GString *result = NULL;
    FILE *file = fopen ( presets_last_filename, "r" );
    if (file != NULL)
    {
        char line[LINE_BUFFER_SIZE];

        while (fgets (line, sizeof line, file) != NULL)
        {
            size_t len = strcspn (line, "\n");
            line[len] = '\0';

            result = g_string_new (line);
        }

        fclose (file);
    }
    else
    {
        g_warning ("Could not read %s", presets_list_filename);
    }

    // TODO: Read the state file with the last index numbers
    presets_current_index = -1;

    g_debug ("last: %s", result->str);

    return result;
}

GString*
presets_write_last (GString *preset)
{
    GString *result = NULL;
    FILE *file = fopen ( presets_last_filename, "w" );
    if (file != NULL)
    {
        fputs (preset->str, file);
        fclose (file);
    }
    else
    {
        g_warning ("Could not write %s", presets_last_filename);
    }
    return result;
}

GString*
presets_next ()
{
    GString *result = NULL;
    gint len = g_list_length (presets);
    if (presets_current_index == -1)
    {
        presets_current_index = 0;
    }
    else if (presets_current_index >= len - 1)
    {
        presets_current_index = 0;
    }
    else
    {
        presets_current_index++;
    }
    result = (GString*) g_list_nth_data (presets, presets_current_index);
    presets_write_last (result);
    return result;
}

void
presets_init (void (*signal_handler)(int))
{
    presets_last = presets_read_last ();
    presets_current_index = presets_read_list (presets_last);
    
    signal (SIGUSR1, signal_handler);
}

void
presets_cleanup ()
{
    g_string_free (presets_last, TRUE);
    
    g_list_foreach (presets, (GFunc) g_free, NULL);
    g_list_free (presets);
}
