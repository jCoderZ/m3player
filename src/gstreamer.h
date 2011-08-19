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

#ifndef GSTREAMER_H
#define GSTREAMER_H 1

#include <glib.h>

GString*
gstreamer_format_time (gint64);

void
gstreamer_set_uri (gchar *);

void
gstreamer_play ();

void
gstreamer_pause ();

void
gstreamer_stop ();

gboolean
gstreamer_get_element_info (gint64 *, gint64 *);

const char*
gstreamer_get_mime_types ();

void
gstreamer_init ();

void
gstreamer_cleanup ();

#endif
