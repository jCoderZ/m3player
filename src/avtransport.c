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

#include "avtransport.h"
#include "avtransport.gen.c"

#include "renderingcontrol.h"
#include "gstreamer.h"
#include "last_change.h"

#include <limits.h> 

// Constants
const gchar *NOT_IMPLEMENTED = "NOT_IMPLEMENTED";
const gchar *ZERO_DURATION = "00:00:00";
const gchar *EMPTY = "";

const gchar *LAST_CHANGE_KEY_TRANSPORT_STATE = "TransportState";
const gchar *LAST_CHANGE_KEY_TRASNPORT_STATUS = "TransportStatus";
const gchar *LAST_CHANGE_KEY_CURRENT_MEDIA_CATEGORY = "CurrentMediaCategory";
const gchar *LAST_CHANGE_KEY_PLAY_MODE = "PlayMode";
const gchar *LAST_CHANGE_KEY_TRANSPORT_PLAY_SPEED = "TransportPlaySpeed";
const gchar *LAST_CHANGE_KEY_NUMBER_OF_TRACKS = "NumberOfTracks";
const gchar *LAST_CHANGE_KEY_CURRENT_TRACK = "CurrentTrack";
const gchar *LAST_CHANGE_KEY_CURRENT_TRACK_DURATION = "CurrentTrackDuration";
const gchar *LAST_CHANGE_KEY_CURRENT_MEDIA_DURATION = "CurrentMediaDuration";
const gchar *LAST_CHANGE_KEY_CURRENT_TRACK_META_DATA = "CurrentTrackMetaData";
const gchar *LAST_CHANGE_KEY_CURRENT_TRACK_URI = "CurrentTrackURI";
const gchar *LAST_CHANGE_KEY_RELATIVE_TIME_POSITION = "RelativeTimePosition";
const gchar *LAST_CHANGE_KEY_ABSOLUTE_TIME_POSITION = "AbsoluteTimePosition";
const gchar *LAST_CHANGE_KEY_RELATIVE_COUNTER_POSITION = "RelativeCounterPosition";
const gchar *LAST_CHANGE_KEY_ABSOLUTE_COUNTER_POSITION = "AbsoluteCounterPosition";
const gchar *LAST_CHANGE_KEY_CURRENT_TRANSPORT_ACTIONS = "CurrentTransportActions";

///////////////////////////////////////////////////////////////////////////////
// State-Variables
///////////////////////////////////////////////////////////////////////////////

// 2.2.1 TransportState
enum TransportState sv_transport_state = NO_MEDIA_PRESENT;

// 2.2.2 TransportStatus
enum TransportStatus sv_transport_status = OK;

// 2.2.3 CurrentMediaCategory
enum CurrentMediaCategory sv_current_media_category = TRACK_AWARE;

// 2.2.4 PlaybackStorageMedium (NOT_IMPLEMENTED)
// 2.2.5 RecordStorageMedium (NOT_IMPLEMENTED)
// 2.2.6 PossiblePlaybackStorageMedia (NOT_IMPLEMENTED)
// 2.2.7 PossibleRecordStorageMedia (NOT_IMPLEMENTED)

// 2.2.8 PlayMode
enum PlayMode sv_current_play_mode = NORMAL;

// 2.2.9 TransportPlaySpeed
gint sv_current_play_speed = 1;
const gchar *sv_current_play_speed_string = "1";

// 2.2.10 RecordMediumWriteStatus (NOT_IMPLEMENTED)
// 2.2.11 CurrentRecordQualityMode (NOT_IMPLEMENTED)
// 2.2.12 PossibleRecordQualityModes (NOT_IMPLEMENTED)

// 2.2.13 NumberOfTracks (ui4)
guint sv_number_of_tracks = -1;

// 2.2.14 CurrentTrack (ui4)
guint sv_current_track = -1;

// 2.2.15 CurrentTrackDuration
gint64 sv_current_track_duration = -1;
GString *sv_current_track_duration_string = NULL;

// 2.2.16 CurrentMediaDuration ???
gint64 sv_current_media_duration = -1;
GString *sv_current_media_duration_string = NULL;

// 2.2.17 CurrentTrackMetaData
GString *sv_current_track_meta_data = NULL;

// 2.2.18 CurrentTrackURI
GString *sv_current_track_uri = NULL;

// 2.2.19 AVTransportURI (NOT_IMPLEMENTED)
// 2.2.20 AVTransportURIMetaData (NOT_IMPLEMENTED)
// 2.2.21 NextAVTransportURI (NOT_IMPLEMENTED)
// 2.2.22 NextAVTransportURIMetaData (NOT_IMPLEMENTED)

// 2.2.23 RelativeTimePosition
gint64 sv_relative_time_position;
GString *sv_relative_time_position_string = NULL;

// 2.2.24 AbsoluteTimePosition (same as RelativeTimePosition)
gint64 sv_absolute_time_position;
GString *sv_absolute_time_position_string = NULL;

// 2.2.25 RelativeCounterPosition (i4)
GString *sv_relative_counter_position_string = NULL;
// 2.2.26 AvsoluteCounterPosition (ui4)
GString *sv_absolute_counter_position_string = NULL;

// 2.2.27 CurrentTransportActions
enum CurrentTransportActions sv_current_transport_action = PLAY;

// 2.2.28 LastChange (implemented dynamically)
// 2.2.29 DRMState (NOT_IMPLEMENTED)



///////////////////////////////////////////////////////////////////////////////
// Helper functions
///////////////////////////////////////////////////////////////////////////////

const gchar*
transport_state_to_string (int ts)
{
    const gchar* result = NULL;
    switch (ts)
    {
        case STOPPED:
            result = "STOPPED";
            break;
        case PLAYING:
            result = "PLAYING";
            break;
        case TRANSITIONING:
            result = "TRANSITIONING";
            break;
        case PAUSED_PLAYBACK:
            result = "PAUSED_PLAYBACK";
            break;
        case PAUSED_RECORDING:
            result = "PAUSED_RECORDING";
            break;
        case RECORDING:
            result = "RECORDING";
            break;
        case NO_MEDIA_PRESENT:
            result = "NO_MEDIA_PRESENT";
            break;
        default:
            g_warning("Unknown TransportState: %d", ts);
            break;
    }
    return result;
}

const gchar* 
transport_status_to_string (int ts)
{
    const gchar* result = NULL;
    switch (ts)
    {
        case OK:
            result = "OK";
            break;
        case ERROR_OCCURED:
            result = "ERROR_OCCURED";
            break;
        default:
            g_warning("Unknown TransportStatus: %d", ts);
            break;
    }
    return result;
}

const gchar*
current_transport_actions_to_string (int cta)
{
    const gchar* result = NULL;
    switch (cta)
    {
        case PLAY:
            result = "PLAY";
            break;
        case STOP:
            result = "STOP";
            break;
        case PAUSE:
            result = "PAUSE";
            break;
        case SEEK:
            result = "SEEK";
            break;
        case NEXT:
            result = "NEXT";
            break;
        case PREVIOUS:
            result = "PREVIOUS";
            break;
        case RECORD:
            result = "RECORD";
            break;
        default:
            g_warning("Unknown CurrentTransportActions: %d", cta);
            break;
    }
    return result;
}

const gchar*
current_transport_actions_from_transport_state (int transport_state)
{
    const gchar* result = NULL;
    switch (transport_state)
    {
        case STOPPED:
            result = "PLAY";
            break;
        case PLAYING:
            result = "STOP,PAUSE";
            break;
        case TRANSITIONING:
            result = "STOP";
            break;
        case PAUSED_PLAYBACK:
            result = "PLAY,STOP";
            break;
        case PAUSED_RECORDING:
            result = "RECORD,STOP";
            break;
        case RECORDING:
            result = "STOP";
            break;
        case NO_MEDIA_PRESENT:
            result = "";
            break;
        default:
            g_warning("Unknown TransportState: %d", transport_state);
            break;
    }
    return result;
}

const gchar*
play_mode_to_string (int pm)
{
    const gchar* result = NULL;
    switch (pm)
    {
        case NORMAL:
            result = "NORMAL";
            break;
        case SHUFFLE:
            result = "SHUFFLE";
            break;
        case REPEAT_ONE:
            result = "REPEAT_ONE";
            break;
        case REPEAT_ALL:
            result = "REPEAT_ALL";
            break;
        case RANDOM:
            result = "RANDOM";
            break;
        case DIRECT_1:
            result = "DIRECT_1";
            break;
        case INTRO:
            result = "INTRO";
            break;
        default:
            g_warning("Unknown PlayMode: %d", pm);
            break;
    }
    return result;
}

const gchar*
current_media_category_to_string (int cmc)
{
    const gchar* result = NULL;
    switch (cmc)
    {
        case NO_MEDIA:
            result = "NO_MEDIA";
            break;
        case TRACK_AWARE:
            result = "TRACK_AWARE";
            break;
        case TRACK_UNAWARE:
            result = "TRACK_UNAWARE";
            break;
        default:
            g_warning("Unknown CurrentMediaCategory: %d", cmc);
            break;
    }
    return result;
}

//////////////////////////////////////////////////////////
// State Variable Callback Handlers
//////////////////////////////////////////////////////////

/*
 Optional:
 <name>DRMState</name>
 */

const gchar*
query_transport_state_cb (GUPnPService *service, 
                          gpointer user_data)
{
    g_debug ("query_transport_state_cb");

    return transport_state_to_string (sv_transport_state);
}

void
set_transport_state (GString *last_change, int ts)
{
    sv_transport_state = ts;

    if (last_change)
    {
        last_change_append_gchar (last_change, LAST_CHANGE_KEY_TRANSPORT_STATE, 
                                  transport_state_to_string (sv_transport_state));
    }
}



const gchar*
query_transport_status_cb (GUPnPService *service, 
                           gpointer user_data)
{
    g_debug ("query_transport_status_cb");

    return transport_status_to_string (sv_transport_status);
}

void
set_transport_status (GString *last_change, int ts)
{
    sv_transport_status = ts;

    if (last_change)
    {
        last_change_append_gchar (last_change, LAST_CHANGE_KEY_TRASNPORT_STATUS, 
                                  transport_status_to_string (sv_transport_status));
    }
}



const gchar*
query_current_media_category_cb (GUPnPService *service, 
                                 gpointer user_data)
{
    g_debug ("query_current_media_category_cb");

    return current_media_category_to_string (sv_current_media_category);
}

void
set_current_media_category (GString *last_change, int cmc)
{
    sv_current_media_category = cmc;

    if (last_change)
    {
        last_change_append_gchar (last_change, LAST_CHANGE_KEY_CURRENT_MEDIA_CATEGORY, 
                                  current_media_category_to_string (sv_current_media_category));
    }

    // 2.2.13: For track-unaware media, this state variable will always be set to 1.
    // 2.2.14: For track-unaware media, this state variable is always 1
    if (sv_current_media_category == TRACK_UNAWARE)
    {
        sv_number_of_tracks = 1;

        if (last_change)
        {
            last_change_append_int (last_change, LAST_CHANGE_KEY_NUMBER_OF_TRACKS, sv_number_of_tracks);
        }

        sv_current_track = 1;

        if (last_change)
        {
            last_change_append_int (last_change, LAST_CHANGE_KEY_CURRENT_TRACK, sv_current_track);
        }
    }
}



const gchar*
query_current_play_mode_cb (GUPnPService *service, 
                            gpointer user_data)
{
    g_debug ("query_current_play_mode_cb");

    return play_mode_to_string (sv_current_play_mode);
}

void
set_current_play_mode (GString *last_change, int cpm)
{
    sv_current_play_mode = cpm;

    if (last_change)
    {
       last_change_append_int (last_change, LAST_CHANGE_KEY_PLAY_MODE, 
                               sv_current_play_mode);
    }
}



const gchar*
query_transport_play_speed_cb (GUPnPService *service, 
                               gpointer user_data)
{
    g_debug ("query_transport_play_speed_cb");

    return sv_current_play_speed_string;
}

void
set_current_play_speed (GString *last_change, int cpm)
{
    if (cpm != 1) {
        g_warning("Error: 717: Play speed \"%d\" not supported", cpm);
    }

    sv_current_play_speed = cpm;

    if (last_change)
    {
        last_change_append_int (last_change, LAST_CHANGE_KEY_TRANSPORT_PLAY_SPEED, 
                                sv_current_play_speed);
    }
}



guint
query_number_of_tracks_cb (GUPnPService *service, 
                           gpointer user_data)
{
    g_debug ("query_number_of_tracks_cb");

    return sv_number_of_tracks;
}

void
set_number_of_tracks (GString *last_change, int number_of_tracks)
{
    sv_number_of_tracks = number_of_tracks;

    if (last_change)
    {
        last_change_append_int (last_change, LAST_CHANGE_KEY_NUMBER_OF_TRACKS, sv_number_of_tracks);
    }

    if (sv_number_of_tracks == 0)
    {
        sv_current_track = 0;
        
        if (last_change)
        {
            last_change_append_int (last_change, LAST_CHANGE_KEY_CURRENT_TRACK, sv_current_track);
        }
    }
}



guint
query_current_track_cb (GUPnPService *service, 
                        gpointer user_data)
{
    g_debug ("query_current_track_cb");

    return sv_current_track;
}

void
set_current_track (GString *last_change, int current_track)
{
    if (sv_number_of_tracks == 0)
    {
        g_warning("Setting current track to 0 because number of tracks is 0");
        sv_current_track = 0;
    }
    else if (1 <= current_track && current_track <= sv_number_of_tracks)
    {
        sv_current_track = current_track;
    }
    else
    {
        g_warning("Invalid current track: %d (#%d)", current_track, sv_number_of_tracks);
    }

    if (last_change)
    {
        last_change_append_int (last_change, LAST_CHANGE_KEY_CURRENT_TRACK, sv_current_track);
    }
}



const gchar*
query_current_track_duration_cb (GUPnPService *service, 
                                 gpointer user_data)
{
    g_debug ("query_current_track_duration_cb");

    return sv_current_track_duration_string->str;
}

void
set_current_track_duration (GString *last_change, gint64 current_track_duration)
{
    sv_current_track_duration = current_track_duration;

    sv_current_track_duration_string = gstreamer_format_time (sv_current_track_duration);

    if (last_change)
    {
        last_change_append_gstring (last_change, LAST_CHANGE_KEY_CURRENT_TRACK_DURATION, 
                                    sv_current_track_duration_string);
    }
}



const gchar*
query_current_media_duration_cb (GUPnPService *service, 
                                 gpointer user_data)
{
    g_debug ("query_current_media_duration_cb");

    return sv_current_media_duration_string->str;
}

void
set_current_media_duration (GString *last_change, gint64 media_duration)
{
    if (sv_number_of_tracks == 1)
    {
        g_warning("Setting media duration to track duration as number of tracks is 1");

        sv_current_media_duration = sv_current_track_duration;
    }

    sv_current_media_duration_string = gstreamer_format_time (sv_current_media_duration);

    if (last_change)
    {
        last_change_append_gstring (last_change, LAST_CHANGE_KEY_CURRENT_MEDIA_DURATION, 
                                   sv_current_media_duration_string);
    }
}



const gchar*
query_current_track_meta_data_cb (GUPnPService *service, 
                                  gpointer user_data)
{
    g_debug ("query_current_track_meta_data_cb");

    return sv_current_track_meta_data->str;
}

void 
set_current_track_meta_data (GString *last_change, const gchar* current_track_meta_data)
{
    sv_current_track_meta_data = g_string_new (current_track_meta_data);

    if (last_change)
    {
        last_change_append_gstring (last_change, LAST_CHANGE_KEY_CURRENT_TRACK_META_DATA, 
                                    sv_current_track_meta_data);
    }
}



const gchar*
query_current_track_uri_cb (GUPnPService *service, 
                            gpointer user_data)
{
    g_debug ("query_current_track_uri_cb");

    return sv_current_track_uri->str;
}

void 
set_current_track_uri (GString *last_change, const gchar* current_track_uri)
{
    sv_current_track_uri = g_string_new (current_track_uri);

    if (last_change)
    {
        last_change_append_gstring (last_change, LAST_CHANGE_KEY_CURRENT_TRACK_URI, 
                                    sv_current_track_uri);
    }
}



const gchar*
query_relative_time_position_cb (GUPnPService *service, 
                                 gpointer user_data)
{
    g_debug ("query_relative_time_position_cb");

    return sv_relative_time_position_string->str;
}

void
set_relative_time_position (GString* last_change, gint64 relative_time_position)
{
    sv_relative_time_position = relative_time_position;

    sv_relative_time_position_string = gstreamer_format_time (sv_relative_time_position);
    
    if (last_change)
    {
        last_change_append_gstring (last_change, LAST_CHANGE_KEY_RELATIVE_TIME_POSITION, 
                                    sv_relative_time_position_string);
    }

    sv_relative_counter_position_string = gstreamer_format_time (sv_relative_time_position);
    
    last_change_append_gstring (last_change, LAST_CHANGE_KEY_RELATIVE_COUNTER_POSITION, 
                                sv_relative_counter_position_string);
}



const gchar*
query_absolute_time_position_cb (GUPnPService *service, 
                                 gpointer user_data)
{
    g_debug ("query_absolute_time_position_cb");

    return sv_relative_time_position_string->str;
}

void
set_absolute_time_position (GString *last_change, gint64 absolute_time_position)
{
    sv_absolute_time_position = absolute_time_position;

    sv_absolute_time_position_string = gstreamer_format_time (sv_absolute_time_position);

    if (last_change)
    {
        last_change_append_gstring (last_change, LAST_CHANGE_KEY_ABSOLUTE_TIME_POSITION, 
                                    sv_absolute_time_position_string);
    }

    sv_absolute_counter_position_string = gstreamer_format_time (sv_absolute_time_position);

    if (last_change)
    {
        last_change_append_gstring (last_change, LAST_CHANGE_KEY_ABSOLUTE_COUNTER_POSITION, 
                                    sv_absolute_counter_position_string);
    }
}



gint
query_relative_counter_position_cb (GUPnPService *service, 
                                    gpointer user_data)
{
    g_debug ("query_relative_counter_position_cb");

    return INT_MAX;
}

void
set_relative_counter_position (GString *last_change, gint64 relative_counter_position)
{
    g_warning("Ignoring value, use set_relative_time_position");
}



gint
query_absolute_counter_position_cb (GUPnPService *service, 
                                    gpointer user_data)
{
    g_debug ("query_absolute_counter_position_cb");

    return INT_MAX;
}

void
set_absolute_counter_position (GString *last_change, gint64 absolute_counter_position)
{
    g_warning("Ignoring value, use set_absolute_time_position");
}



const gchar*
query_current_transport_actions_cb (GUPnPService *service, 
                                    gpointer user_data)
{
    g_debug ("query_current_transport_actions_cb");

    return current_transport_actions_from_transport_state (sv_transport_state);
}


/**
 * This function gets called by control points to query all internal state variables.
 */
gchar*
query_avtransport_last_change_cb (GUPnPService *service, 
                      gpointer user_data)
{
    g_debug ("query_last_change_cb");

    gchar *result = NULL;

    GString *last_change = last_change_new();

    last_change_append_gchar (last_change, LAST_CHANGE_KEY_TRANSPORT_STATE, 
                              transport_state_to_string (sv_transport_state));

    last_change_append_gchar (last_change, LAST_CHANGE_KEY_TRASNPORT_STATUS, 
                              transport_status_to_string (sv_transport_status));

    last_change_append_gchar (last_change, LAST_CHANGE_KEY_CURRENT_MEDIA_CATEGORY, 
                              current_media_category_to_string (sv_current_media_category));

    last_change_append_gchar (last_change, LAST_CHANGE_KEY_PLAY_MODE, 
                              play_mode_to_string (sv_current_play_mode));

    last_change_append_gchar (last_change, LAST_CHANGE_KEY_TRANSPORT_PLAY_SPEED, 
                              sv_current_play_speed_string);

    last_change_append_int (last_change, LAST_CHANGE_KEY_NUMBER_OF_TRACKS, 
                            sv_number_of_tracks);

    last_change_append_int (last_change, LAST_CHANGE_KEY_CURRENT_TRACK, 
                            sv_current_track);

    last_change_append_gstring (last_change, LAST_CHANGE_KEY_CURRENT_TRACK_DURATION, 
                                sv_current_track_duration_string);

    last_change_append_gstring (last_change, LAST_CHANGE_KEY_CURRENT_MEDIA_DURATION, 
                                sv_current_media_duration_string);

    last_change_append_gstring (last_change, LAST_CHANGE_KEY_CURRENT_TRACK_META_DATA, 
                                sv_current_track_meta_data);
    last_change_append_gstring (last_change, LAST_CHANGE_KEY_CURRENT_TRACK_URI, 
                                sv_current_track_uri);

    last_change_append_gchar (last_change, "AVTransportURIMetaData", 
                              NOT_IMPLEMENTED);
    last_change_append_gchar (last_change, "AVTransportURI", 
                              NOT_IMPLEMENTED);
    last_change_append_gchar (last_change, "RecordMediumWriteStatus", 
                              NOT_IMPLEMENTED);
    last_change_append_gchar (last_change, "CurrentRecordQualityMode", 
                              NOT_IMPLEMENTED);
    last_change_append_gchar (last_change, "PossibleRecordQualityModes", 
                              NOT_IMPLEMENTED);
    last_change_append_gchar (last_change, "PlaybackStorageMedium", 
                              NOT_IMPLEMENTED);
    last_change_append_gchar (last_change, "RecordStorageMedium", 
                              NOT_IMPLEMENTED);
    last_change_append_gchar (last_change, "PossiblePlaybackStorageMedia", 
                              NOT_IMPLEMENTED);
    last_change_append_gchar (last_change, "PossibleRecordStorageMedia", 
                              NOT_IMPLEMENTED);
    last_change_append_gchar (last_change, "NextAVTransportURI", 
                              NOT_IMPLEMENTED);
    last_change_append_gchar (last_change, "NextAVTransportURIMetaData", 
                              NOT_IMPLEMENTED);

    result = last_change_free (last_change);

    g_debug ("\t%s", result);

    return result;
}

void
avtransport_notify_last_change (GUPnPService *service, GString *last_change)
{
    g_debug ("notify_last_change");
    g_debug ("%s", last_change->str);

    avtransport_last_change_variable_notify(service, last_change->str);

    g_string_free (last_change, TRUE);
}

// Constructor
void
avtransport_state_variables_init ()
{
    set_transport_state (NULL, NO_MEDIA_PRESENT);
    set_transport_status (NULL, OK);
    set_current_media_category (NULL, NO_MEDIA);
    set_current_play_mode (NULL, NORMAL);

    set_number_of_tracks (NULL, 0);
    set_current_track (NULL, 0);

    set_current_track_duration (NULL, 0);
    set_current_media_duration (NULL, 0);
    set_current_track_meta_data (NULL, EMPTY);
    set_current_track_uri (NULL, EMPTY);
    set_relative_time_position (NULL, 0);
    set_absolute_time_position (NULL, 0);
}

// Destructor
void
avtransport_state_variables_cleanup ()
{
    g_string_free (sv_current_track_duration_string, TRUE);
    g_string_free (sv_current_media_duration_string, TRUE);
    g_string_free (sv_current_track_meta_data, TRUE);
    g_string_free (sv_current_track_uri, TRUE);
    g_string_free (sv_relative_time_position_string, TRUE);
    g_string_free (sv_absolute_time_position_string, TRUE);
    g_string_free (sv_relative_counter_position_string, TRUE);
    g_string_free (sv_absolute_counter_position_string, TRUE);
}

//////////////////////////////////////////////////////////
// Actions
//////////////////////////////////////////////////////////

// Helper to check for valid instance ids
gboolean
check_instance_id (GUPnPServiceAction *action)
{
    guint instance_id;
    gupnp_service_action_get (action,
                              "InstanceID", G_TYPE_UINT, &instance_id, NULL);
    if (instance_id != 0) 
    {
        gupnp_service_action_return_error (action, 718, "Invalid InstanceID");
        return FALSE;
    }
    return TRUE;
}


/*
 Optional: 
 <name>SetNextAVTransportURI</name>
 <name>Record</name>
 <name>SetPlayMode</name>
 <name>SetRecordQualityMode</name>
 <name>GetCurrentTransportActions</name>
 <name>GetDRMState</name>
 <name>GetStateVariables</name>
 <name>SetStateVariables</name>
 */

//			<name>SetAVTransportURI</name> OK: rygel
void
set_avtransport_uri_cb (GUPnPService *service,
                        GUPnPServiceAction *action,
                        gpointer user_data)
{
    g_debug ("set_avtransport_uri_cb");
    if (!check_instance_id (action)) 
    {
        return;
    }

    guint instance_id;
    gchar *current_track_uri = NULL;
    gchar *current_track_meta_data = NULL;
    avtransport_set_av_transport_uri_action_get (action, 
                                                 &instance_id, 
                                                 &current_track_uri, 
                                                 &current_track_meta_data);

    set_current_track_uri (NULL, current_track_uri);
    set_current_track_meta_data (NULL, current_track_meta_data);
    if (sv_transport_state == PLAYING)
    {
        // TODO: TRANSITIONING -> play new URI
    }
    else
    {
        set_transport_state (NULL, STOPPED);
    }

    g_debug ("\tcurrent_track_uri: %s\n\tcurrent_track_meta_data: %s\n", sv_current_track_uri->str, sv_current_track_meta_data->str);

    gstreamer_set_uri (sv_current_track_uri->str);

    gupnp_service_action_return (action);    
}

//			<name>GetMediaInfo</name>: OK: rygel
void
get_media_info_cb (GUPnPService *service,
                   GUPnPServiceAction *action,
                   gpointer user_data)
{
    g_debug ("get_media_info_cb");
    if (!check_instance_id (action)) 
    {
        return;
    }

    avtransport_get_media_info_action_set (action,
                                           sv_number_of_tracks,
                                           sv_current_track_duration_string->str,
                                           sv_current_track_uri->str,
                                           sv_current_track_meta_data->str,
                                           NOT_IMPLEMENTED,
                                           NOT_IMPLEMENTED,
                                           NOT_IMPLEMENTED,
                                           NOT_IMPLEMENTED,
                                           NOT_IMPLEMENTED);

    gupnp_service_action_return (action);    
}

//			<name>GetMediaInfo_Ext</name> ???
void
get_media_info_ext_cb (GUPnPService *service,
                       GUPnPServiceAction *action,
                       gpointer user_data)
{
    g_debug ("get_media_info_ext_cb");
    gupnp_service_action_return (action);    
}

//			<name>GetTransportInfo</name> OK: rygel
void
get_transport_info_cb (GUPnPService *service,
                       GUPnPServiceAction *action,
                       gpointer user_data)
{
    g_debug ("get_transport_info_cb");
    if (!check_instance_id(action))
    {
        return;
    }

    const gchar* tstate = transport_state_to_string (sv_transport_state);
    const gchar* tstatus = transport_status_to_string (sv_transport_status);

    g_debug ("\ttransport_state=%s\n\ttransport_status=%s\n\tcurrent_play_speed=%d\n", 
            tstate, tstatus, sv_current_play_speed);

    avtransport_get_transport_info_action_set (action,
                                               tstate,
                                               tstatus,
                                               sv_current_play_speed_string);
    gupnp_service_action_return (action);    
}

//			<name>GetPositionInfo</name> ???
void
get_position_info_cb(GUPnPService *service,
                     GUPnPServiceAction *action,
                     gpointer user_data)
{
    g_debug ("get_position_info_cb");
    if (!check_instance_id(action))
    {
        return;
    }

    gint64 relative_time_position = 0;
    gint64 current_track_duration = 0;
    if (gstreamer_get_element_info (&relative_time_position, &current_track_duration))
    {
        // 2.3.1 Event Model
        // The following variables are therefore not evented via LastChange:
        //    RelativeTimePosition, AbsoluteTimePosition
        //    RelativeCounterPosition, AbsoluteCounterPosition
        set_relative_time_position (NULL, relative_time_position);
        set_absolute_time_position (NULL, relative_time_position);
        set_current_track_duration (NULL, current_track_duration);
    }
    
    g_debug ("\trelative_time_position=%s\n\ttrack_duration=%s\n\ttrack=%d\n\ttrack_uri=%s\n\ttrack_meta_data=%s\n",
             sv_relative_time_position_string->str, 
             sv_current_track_duration_string->str, 
             sv_current_track, 
             sv_current_track_uri->str, 
             sv_current_track_meta_data->str);

    avtransport_get_position_info_action_set (action, 
                                              sv_current_track, 
                                              sv_current_track_duration_string->str,
                                              sv_current_track_meta_data->str, 
                                              sv_current_track_uri->str, 
                                              sv_relative_time_position_string->str, 
                                              sv_absolute_time_position_string->str, 
                                              INT_MAX, 
                                              INT_MAX);
    gupnp_service_action_return (action);    
}

// <name>GetDeviceCapabilities</name> OK: rygel
void
get_device_capabilities_cb (GUPnPService *service,
                            GUPnPServiceAction *action,
                            gpointer user_data)
{
    g_debug ("get_device_capabilities_cb");
    if (!check_instance_id(action))
    {
        return;
    }

    avtransport_get_device_capabilities_action_set (action,
                                                    NOT_IMPLEMENTED,
                                                    NOT_IMPLEMENTED,
                                                    NOT_IMPLEMENTED);

    gupnp_service_action_return (action);    
}

//			<name>GetTransportSettings</name> OK: rygel
void
get_transport_settings_cb (GUPnPService *service,
                           GUPnPServiceAction *action,
                           gpointer user_data)
{
    g_debug ("get_transport_settings_cb");
    if (!check_instance_id(action))
    {
        return;
    }

    avtransport_get_transport_settings_action_set (action,
                                                   play_mode_to_string (sv_current_play_mode),
                                                   NOT_IMPLEMENTED);
    gupnp_service_action_return (action);    
}

//			<name>Stop</name>
void
stop_cb (GUPnPService *service,
         GUPnPServiceAction *action,
         gpointer user_data)
{
    g_debug ("stop_cb");
    if (!check_instance_id(action))
    {
        return;
    }

    gstreamer_stop ();

    GString *last_change = last_change_new ();
    set_transport_state (last_change, STOPPED);
    avtransport_notify_last_change (service, last_change);

    gupnp_service_action_return (action);    
}

//			<name>Play</name>
void
play_cb (GUPnPService *service,
         GUPnPServiceAction *action,
         gpointer user_data)
{
    g_debug ("play_cb");
    if (!check_instance_id(action))
    {
        return;
    }

    gstreamer_play();

    GString *last_change = last_change_new ();
    set_transport_state (last_change, PLAYING);
    avtransport_notify_last_change(service, last_change);

    gupnp_service_action_return (action);    
}

//			<name>Pause</name>  (Optional)
void
pause_cb (GUPnPService *service,
          GUPnPServiceAction *action,
          gpointer user_data)
{
    g_debug ("pause_cb");
    if (!check_instance_id(action))
    {
        return;
    }


    gstreamer_pause ();

    GString *last_change = last_change_new ();
    set_transport_state (last_change, PAUSED_PLAYBACK);
    avtransport_notify_last_change(service, last_change);

    gupnp_service_action_return (action);    
}

//			<name>Seek</name>
void
seek_cb (GUPnPService *service,
         GUPnPServiceAction *action,
         gpointer user_data)
{
    g_debug ("seek_cb");
    if (!check_instance_id(action))
    {
        return;
    }

    // TODO:
    
    gupnp_service_action_return (action);    
}

//			<name>Next</name>
void
next_cb (GUPnPService *service,
         GUPnPServiceAction *action,
         gpointer user_data)
{
    g_debug ("next_cb");
    gupnp_service_action_return_error (action, 701, "Transition not available");
}

//			<name>Previous</name>
void
previous_cb (GUPnPService *service,
             GUPnPServiceAction *action,
             gpointer user_data)
{
    g_debug ("previous_cb");
    gupnp_service_action_return_error (action, 701, "Transition not available");
}

//////////////////////////////////////////////////////////
// Misc
//////////////////////////////////////////////////////////

gint
avtransport_init (GMainLoop *main_loop)
{
    g_debug ("Initializing state variables...");
    avtransport_state_variables_init ();

    g_debug ("Initializing gstreamer sub-system...");
    gstreamer_init (main_loop);

    return 0;
}

void 
avtransport_cleanup (GMainLoop *main_loop)
{
    gstreamer_cleanup ();

    avtransport_state_variables_cleanup ();
}
