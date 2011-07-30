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

// Enumerations
enum TransportState {
    STOPPED,
    PLAYING,
    TRANSITIONING,
    PAUSED_PLAYBACK,
    PAUSED_RECORDING,
    RECORDING,
    NO_MEDIA_PRESENT
};

enum TransportStatus {
    OK,
    ERROR_OCCURED
};

enum CurrentMediaCategory {
    NO_MEDIA,
    TRACK_AWARE,
    TRACK_UNAWARE
};

enum PlayMode {
    NORMAL,
    SHUFFLE,
    REPEAT_ONE,
    REPEAT_ALL,
    RANDOM,
    DIRECT_1,
    INTRO
};

enum CurrentTransportActions {
    PLAY,
    STOP,
    PAUSE,
    SEEK,
    NEXT,
    PREVIOUS,
    RECORD
};


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

void
set_transport_state (GString *last_change, int ts);
void
set_transport_status (GString *last_change, int ts);
void
set_current_media_category (GString *last_change, int cmc);
void
set_current_play_mode (GString *last_change, int cpm);
void
set_current_play_speed (GString *last_change, int cpm);
void
set_number_of_tracks (GString *last_change, int number_of_tracks);
void
set_current_track (GString *last_change, int current_track);
void
set_current_track_duration (GString *last_change, gint64 current_track_duration);
void
set_current_media_duration (GString *last_change, gint64 media_duration);
void
set_current_track_meta_data (GString *last_change, const gchar* current_track_meta_data);
void
set_current_track_uri (GString *last_change, const gchar* current_track_uri);
void
set_relative_time_position (GString* last_change, gint64 relative_time_position);
void
set_absolute_time_position (GString *last_change, gint64 absolute_time_position);
void
set_relative_counter_position (GString *last_change, gint64 relative_counter_position);
void
set_absolute_counter_position (GString *last_change, gint64 absolute_counter_position);

#endif
