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

#include "presets.h"

#include <stdio.h>
#include <signal.h>
#include <string.h>

#define LINE_BUFFER_SIZE 8192

static const char presets_last_filename[] = "preset.last";

static GString *presets_last = NULL;
static gsize presets_count = 0;
static gchar **presets = NULL;
static gint presets_current_index = -1;

/**
  Reads the file presets.list into
 */
gint
presets_read_list (GString *last, GKeyFile *iniFile)
{
    gint previous_index = -1;

    g_debug ("Getting goup/key Presets/List... (%ld)", (long int) iniFile);
    presets = g_key_file_get_string_list (iniFile, "Presets", "List", &presets_count, NULL);
    if (!presets)
    {
        g_printerr ("Could not read presets");
        return -1;
    }

    gint i = 0;
    gint index = -1;
    for (i = 0; i < presets_count; i++)
    {
        g_debug ("preset[%d]=%s", i, presets[i]);
        if (last)
        {
            if (strcmp (presets[i], last->str) == 0)
            {
                g_debug ("Found last preset at index %d: %s", index + 1, presets[i]);
                previous_index = index;
            }
        }

        g_debug ("list[%d]: '%s'", i, presets[i]);
        index++;
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

        g_debug ("last: %s", result->str);

        fclose (file);
    }
    else
    {
        g_warning ("Could not read %s", presets_last_filename);
    }

    // TODO: Read the state file with the last index numbers
    presets_current_index = -1;

    return result;
}

void
presets_write_last (gchar *preset)
{
    FILE *file = fopen ( presets_last_filename, "w" );
    if (file != NULL)
    {
        fputs (preset, file);
        fclose (file);
    }
    else
    {
        g_warning ("Could not write %s", presets_last_filename);
    }
}

gchar*
presets_next ()
{
    gchar *result = NULL;
    if (presets_current_index == -1)
    {
        presets_current_index = 0;
    }
    else if (presets_current_index >= presets_count - 1)
    {
        presets_current_index = 0;
    }
    else
    {
        presets_current_index++;
    }
    result = presets[presets_current_index];
    presets_write_last (result);
    return result;
}

void
presets_init (void (*signal_handler)(int), GKeyFile* iniFile)
{
    g_debug ("Read last preset...");
    presets_last = presets_read_last ();
    g_debug ("Read preset list...");
    presets_current_index = presets_read_list (presets_last, iniFile);
    
    g_debug ("Installing signal handler for USR1...");
    signal (SIGUSR1, signal_handler);
}

void
presets_cleanup ()
{
    g_string_free (presets_last, TRUE);

    g_strfreev (presets);
}

