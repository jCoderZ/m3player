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

#include "connectionmanager.h"
#include "connectionmanager.gen.c"

#include "gstreamer.h"

const gchar *SOURCE = "";
const gchar *connectionIds = "0";
const gchar *CONNECTION_STATUS = "Unknown";
const gchar *DIRECTION = "Input";

const gint DEFAULT_CONNECTION_ID = 0;

const gint RCS_ID = 0;
const gint AVTRANSPORT_ID = 0;

//////////////////////////////////////////////////////////
// State Variables
//////////////////////////////////////////////////////////

//  <name>SourceProtocolInfo</name>
const gchar*
query_source_protocol_info_cb (GUPnPService *service, 
                               gpointer user_data)
{
    g_debug("query_source_protocol_info_cb: %s", SOURCE);

    return SOURCE;
}

//  <name>SinkProtocolInfo</name>
const gchar*
query_sink_protocol_info_cb (GUPnPService *service, 
                             gpointer user_data)
{
    const char* result = gstreamer_get_mime_types ();

    g_debug("query_sink_protocol_info_cb: %s", result);

    return result;
}

//  <name>CurrentConnectionIDs</name>
const gchar*
query_current_connection_ids_cb (GUPnPService *service, 
                                 gpointer user_data)
{
    g_debug("query_current_connection_ids_cb: %s", connectionIds);

    return connectionIds;
}

//////////////////////////////////////////////////////////
// Actions
//////////////////////////////////////////////////////////

/*
Optional Actions:

	<name>PrepareForConnection</name>
	<name>ConnectionComplete</name>

    Both actions are not implemented as we do not support
    multiple instances of the AVTransport or Rendering services.
*/ 

//  <name>GetProtocolInfo</name>
void
get_protocol_info_cb(GUPnPService *service,
                     GUPnPServiceAction *action,
                     gpointer user_data)
{
    const char *SINK = gstreamer_get_mime_types ();
    connectionmanager_get_protocol_info_action_set (action, SOURCE, SINK);

    g_debug("get_protocol_info_cb: SOURCE=%s, SINK=%s", SOURCE, SINK);

    gupnp_service_action_return (action);
}

// <name>GetCurrentConnectionIDs</name>
void
get_current_connection_ids_cb(GUPnPService *service,
                              GUPnPServiceAction *action,
                              gpointer user_data)
{
    connectionmanager_get_current_connection_i_ds_action_set (action, connectionIds);

    g_debug("get_current_connection_ids_cb: %s", connectionIds);

    gupnp_service_action_return (action);
}

// <name>GetCurrentConnectionInfo</name>
void
get_current_connection_info_cb(GUPnPService *service,
                               GUPnPServiceAction *action,
                               gpointer user_data)
{
    gint connectionId;
    connectionmanager_get_current_connection_info_action_get (action, &connectionId);
    if (connectionId != DEFAULT_CONNECTION_ID) 
    {
        // only one connection is allowed
        gupnp_service_action_return_error (action, 706, "invalid connection reference");
        return;
    }

    connectionmanager_get_current_connection_info_action_set (action, 
            RCS_ID,           /* RcsId=0 => RenderingControl implemented */
            AVTRANSPORT_ID,   /* AVTransportID=0 => AVTransport implemented */
            "",               /* ?? */
            "",               /* MUST be the empty string */
            -1,               /* MUST be -1 */ 
            "Input",          /* ?? */
            CONNECTION_STATUS /* ?? */);

    g_debug("get_current_connection_info_cb: %d, %d, '', '', -1, Input, %s", 
            RCS_ID, AVTRANSPORT_ID, CONNECTION_STATUS);

    gupnp_service_action_return (action);
}

//////////////////////////////////////////////////////////
// Misc
//////////////////////////////////////////////////////////
