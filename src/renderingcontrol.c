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

#include "renderingcontrol.h"
#include "renderingcontrol.gen.c"

#include "last_change.h"

//////////////////////////////////////////////////////////
// State Variables
//////////////////////////////////////////////////////////

/*
Optional:
<stateVariable>
			<name>Brightness</name>
			<name>Contrast</name>
			<name>Sharpness</name>
			<name>RedVideoGain</name>
			<name>GreenVideoGain</name>
			<name>BlueVideoGain</name>
			<name>RedVideoBlackLevel</name>
			<name>GreenVideoBlackLevel</name>
			<name>BlueVideoBlackLevel</name>
			<name>ColorTemperature</name>
			<name>HorizontalKeystone</name>
			<name>VerticalKeystone</name>
			<name>Mute</name>
			<name>Volume</name>
			<name>VolumeDB</name>
			<name>Loudness</name>
*/

//			<name>LastChange</name>

//			<name>PresetNameList</name>
const gchar*
query_preset_name_list_cb (GUPnPService *service, 
                           gpointer user_data)
{
    g_debug("query_preset_name_list_cb");

    return "";
}

const gchar*
query_last_change_cb (GUPnPService *service, 
                      gpointer user_data)
{
    g_debug ("query_last_change_cb");

    const gchar *result = NULL;

    GString *last_change = last_change_new();
/*
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
*/
    result = last_change_free (last_change);

    g_debug ("\t%s", result);

    return result;
}

void
renderingcontrol_notify_last_change (GUPnPService *service, GString *last_change)
{
    g_debug ("notify_last_change");
    g_debug ("%s", last_change->str);

    renderingcontrol_last_change_variable_notify (service, last_change->str);

    g_string_free (last_change, TRUE);
}

//////////////////////////////////////////////////////////
// Actions
//////////////////////////////////////////////////////////

/*
Missing:
<action>
			<name>GetBrightness</name>
			<name>SetBrightness</name>
			<name>GetContrast</name>
    		<name>SetContrast</name>
			<name>GetSharpness</name>
			<name>SetSharpness</name>
			<name>GetRedVideoGain</name>
			<name>SetRedVideoGain</name>
			<name>GetGreenVideoGain</name>
			<name>SetGreenVideoGain</name>
			<name>GetBlueVideoGain</name>
			<name>SetBlueVideoGain</name>
			<name>GetRedVideoBlackLevel</name>
			<name>SetRedVideoBlackLevel</name>
			<name>GetGreenVideoBlackLevel</name>
			<name>SetGreenVideoBlackLevel</name>
			<name>GetBlueVideoBlackLevel</name>
			<name>SetBlueVideoBlackLevel</name>
			<name>GetColorTemperature</name>
			<name>SetColorTemperature</name>
			<name>GetHorizontalKeystone</name>
			<name>SetHorizontalKeystone</name>
			<name>GetVerticalKeystone</name>
			<name>SetVerticalKeystone</name>
			<name>GetMute</name>
			<name>SetMute</name>
			<name>GetVolume</name>
			<name>SetVolume</name>
			<name>GetVolumeDB</name>
			<name>SetVolumeDB</name>
			<name>GetVolumeDBRange</name>
			<name>GetLoudness</name>
			<name>SetLoudness</name>
			<name>GetStateVariables</name>
			<name>SetStateVariables</name>

*/

//			<name>ListPresets</name>
//			<name>SelectPreset</name>


//////////////////////////////////////////////////////////
// Misc
//////////////////////////////////////////////////////////

