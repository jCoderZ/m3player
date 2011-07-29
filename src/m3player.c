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

#include <gst/gst.h>
#include <unistd.h>

#include "renderingcontrol.h"
#include "connectionmanager.h"
#include "avtransport.h"
#include "presets.h"
#include "gstreamer.h"

GUPnPContext *context;
GUPnPRootDevice *dev;

GUPnPService *connectionManagerService;
GUPnPService *renderingControlService;
GUPnPService *avTransportService;

void
presets_signal_handler (int sig) 
{
    GString *url = presets_next();
    g_debug("Next station: '%s'", url->str);
    gstreamer_set_uri (url->str);
    gstreamer_play ();
}

void
write_pid_file ()
{
    FILE *file;
    pid_t pid;
    pid = getpid ();
    file = fopen ("/var/run/m3ddity-player.pid","w");
    if (file)
    {
        fprintf (file,"%d", pid);
        fclose (file); 
    }
}

gint
gupnp_init ()
{
    GError *error = NULL;
    g_debug ("Create the UPnP context");
    context = gupnp_context_new (NULL, NULL, 0, &error);
    if (error) 
    {
        g_printerr ("Error creating the GUPnP context: %s", error->message);
        g_error_free (error);

        return EXIT_FAILURE;
    }
    g_debug ("Running on port %d", gupnp_context_get_port (context));

    g_debug ("Create root device");
    dev = gupnp_root_device_new (context, "description2.xml", ".");

    g_debug ("Announce root device");
    gupnp_root_device_set_available (dev, TRUE);

    g_debug ("Get the connection manager service");
    connectionManagerService = GUPNP_SERVICE(gupnp_device_info_get_service
        (GUPNP_DEVICE_INFO (dev), "urn:schemas-upnp-org:service:ConnectionManager:1"));
    if (!connectionManagerService) {
        g_printerr ("Cannot get ConnectionManager service");

        return EXIT_FAILURE;
    }
    gupnp_service_signals_autoconnect (GUPNP_SERVICE (connectionManagerService), NULL, &error);

    g_debug ("Get the rendering control service");
    renderingControlService = GUPNP_SERVICE(gupnp_device_info_get_service
        (GUPNP_DEVICE_INFO (dev), "urn:schemas-upnp-org:service:RenderingControl:1"));
    if (!renderingControlService) {
        g_printerr ("Cannot get RenderingControl service");

        return EXIT_FAILURE;
    }
    gupnp_service_signals_autoconnect (GUPNP_SERVICE (renderingControlService), NULL, &error);
    

    g_debug ("Get the av transport service");
    avTransportService = GUPNP_SERVICE(gupnp_device_info_get_service
        (GUPNP_DEVICE_INFO (dev), "urn:schemas-upnp-org:service:AVTransport:1"));
    if (!avTransportService) {
        g_printerr ("Cannot get AVTransport service");

        return EXIT_FAILURE;
    }
    gupnp_service_signals_autoconnect (GUPNP_SERVICE (avTransportService), NULL, &error);
    // additionally connect avtransport's version of last_change
//    avtransport_last_change_query_connect (GUPNP_SERVICE (avTransportService), 
//                                           query_avtransport_last_change_cb, NULL);
        
    return EXIT_SUCCESS;
}

int
main (int argc, char **argv)
{
    GMainLoop *main_loop;
    
    g_thread_init (NULL);
    g_type_init ();
    gst_init (NULL, NULL);

    int rc = gupnp_init ();
    if (rc != 0)
    {
        return rc;
    }

    g_debug ("Run the main loop");
    main_loop = g_main_loop_new (NULL, FALSE);

    avtransport_init(main_loop);

    presets_init (presets_signal_handler);
    GString *url = presets_next ();
    if (url)
    {
        gstreamer_set_uri (url->str);
        gstreamer_play ();
    }
    
    g_main_loop_run (main_loop);

    presets_cleanup();

    avtransport_cleanup(main_loop);

    g_main_loop_unref (main_loop);
    g_object_unref (connectionManagerService);
    g_object_unref (renderingControlService);
    g_object_unref (avTransportService);
    
    g_object_unref (dev);
    g_object_unref (context);

    return EXIT_SUCCESS;
}
