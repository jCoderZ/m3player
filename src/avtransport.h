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

#ifndef AVTRANSPORT_H
#define AVTRANSPORT_H 1

#include <glib.h>
#include <libgupnp/gupnp.h>
#include <libgupnp-av/gupnp-av.h>
#include <gmodule.h>

typedef gchar *(*query_callback) (GUPnPService *service,
                                        gpointer userdata);

void set_avtransport_uri (gchar *current_track_uri);
void play ();

gint
avtransport_init (GMainLoop *main_loop);

void
avtransport_cleanup (GMainLoop *main_loop);

gchar*
query_avtransport_last_change_cb (GUPnPService *service, 
                                  gpointer user_data);
gulong
avtransport_last_change_query_connect (GUPnPService *service,
                                       query_callback callback,
                                       gpointer userdata);

#endif
