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

/*

   ...

 */

static const gchar *CONFIG_FILE_DEFAULT = "/etc/m3player/m3player.ini";
static const gchar *configFile = NULL;
static const gchar *PID_FILE_DEFAULT = "/var/run/m3player.pid";
static const gchar *pidFile = NULL;
static const gchar *XML_FOLDER_DEFAULT = "/usr/share/m3player";
static const gchar *xmlFolder = NULL;

static GOptionEntry entries[] =
{
  { "config", 'c', 0, G_OPTION_ARG_STRING, &configFile, "Path to the config file", NULL },
  { "pidfile", 'p', 0, G_OPTION_ARG_STRING, &pidFile, "Path to the pid file", NULL },
  { "xmlfolder", 'x', 0, G_OPTION_ARG_STRING, &xmlFolder, "Path to the XML folder file", NULL },
  { "version", 'v', 0, G_OPTION_ARG_STRING, &xmlFolder, "Path to the XML folder file", NULL },
  { NULL }
};

static GKeyFile *iniFile = NULL;


void
presets_signal_handler (int sig) 
{
    gchar *url = presets_next();
    g_debug("Next station: '%s'", url);
    gstreamer_set_uri (url);
    gstreamer_play ();

    
    set_transport_state (NULL, PLAYING);
    set_transport_status (NULL, OK);
    set_current_media_category (NULL, TRACK_UNAWARE);
    set_current_play_mode (NULL, NORMAL);
    set_current_play_speed (NULL, 1);
    set_number_of_tracks (NULL, 1);
    set_current_track (NULL, 1);
    //set_current_track_duration (NULL, );
    //set_current_media_duration (NULL, );
    //set_current_track_meta_data (NULL, );
    set_current_track_uri (NULL, url);
    //set_relative_time_position (NULL, );
    //set_absolute_time_position (NULL, );
    //set_relative_counter_position (NULL, );
    //set_absolute_counter_position (NULL, );
}

void
write_pid_file ()
{
    FILE *file;
    pid_t pid;
    pid = getpid ();
    file = fopen (pidFile, "w");
    if (file)
    {
        fprintf (file,"%d", pid);
        fclose (file); 
    }
}


gint
gupnp_init (const gchar *xmlFolder)
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
    dev = gupnp_root_device_new (context, "MediaRendererV2.xml", xmlFolder);

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
    if (error)
    {
        g_printerr ("Could not autoconnect signals: %s", error->message);
        g_error_free (error);

        return EXIT_FAILURE;
    }

    g_debug ("Get the rendering control service");
    renderingControlService = GUPNP_SERVICE(gupnp_device_info_get_service
        (GUPNP_DEVICE_INFO (dev), "urn:schemas-upnp-org:service:RenderingControl:1"));
    if (!renderingControlService) {
        g_printerr ("Cannot get RenderingControl service");

        return EXIT_FAILURE;
    }
    gupnp_service_signals_autoconnect (GUPNP_SERVICE (renderingControlService), NULL, &error);
    if (error)
    {
        g_printerr ("Could not autoconnect signals: %s", error->message);
        g_error_free (error);

        return EXIT_FAILURE;
    }
    
    g_debug ("Get the av transport service");
    avTransportService = GUPNP_SERVICE(gupnp_device_info_get_service
        (GUPNP_DEVICE_INFO (dev), "urn:schemas-upnp-org:service:AVTransport:1"));
    if (!avTransportService) {
        g_printerr ("Cannot get AVTransport service");

        return EXIT_FAILURE;
    }
    gupnp_service_signals_autoconnect (GUPNP_SERVICE (avTransportService), NULL, &error);
    if (error)
    {
        g_printerr ("Could not autoconnect signals: %s", error->message);
        g_error_free (error);

        return EXIT_FAILURE;
    }
    // additionally connect avtransport's version of last_change
//    avtransport_last_change_query_connect (GUPNP_SERVICE (avTransportService), 
//                                           query_avtransport_last_change_cb, NULL);
        
    return EXIT_SUCCESS;
}

int
main (int argc, char **argv)
{
    // Parse command line
    GError *optionError = NULL;
    GOptionContext *context;
    context = g_option_context_new ("m3player");
    g_option_context_add_main_entries (context, entries, NULL);
    if (!g_option_context_parse (context, &argc, &argv, &optionError))
    {
        g_printerr ("option parsing failed: %s\n", optionError->message);
        g_error_free (optionError);
        exit (1);
    }

    // Set defaults
    if (configFile == NULL)
    {
        g_debug ("Setting configFile to default: %s", CONFIG_FILE_DEFAULT);
        configFile = CONFIG_FILE_DEFAULT;
    }
    if (xmlFolder == NULL)
    {
        g_debug ("Setting XML folder to default: %s", XML_FOLDER_DEFAULT);
        xmlFolder = XML_FOLDER_DEFAULT;
    }
    if (pidFile == NULL)
    {
        g_debug ("Setting PID file to default: %s", PID_FILE_DEFAULT);
        pidFile = PID_FILE_DEFAULT;
    }

    // Load ini file
    if (g_file_test (configFile, G_FILE_TEST_EXISTS)) 
    {
        GError *iniError = NULL;
        g_debug("Reading config file: %s", configFile);
        iniFile = g_key_file_new ();
        if (!g_key_file_load_from_file (iniFile, configFile, G_KEY_FILE_NONE, &iniError))
        {
            g_printerr ("reading ini file failed! %s\n", iniError->message);
            g_error_free (iniError);
            exit (2);
        }
    }
    else
    {
        g_printerr ("Specified config file does not exist: %s\n", configFile);
        exit (3);
    }

    // Test whether the folder exists
    if (!g_file_test (xmlFolder, G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR)) 
    {
        g_printerr ("Specified XML folder does not exist: %s\n", xmlFolder);
        exit (4);
    }
    
    // Initialize sub-systems
    g_thread_init (NULL);
    g_type_init ();
    gst_init (NULL, NULL);
    int rc = gupnp_init (xmlFolder);
    if (rc != 0)
    {
        g_printerr ("GUPnP initialization failed!");
        return rc;
    }

    GMainLoop *main_loop;
    g_debug ("Create new main loop...");
    main_loop = g_main_loop_new (NULL, FALSE);

    g_debug ("Initializing AVTransport instance...");
    avtransport_init(main_loop);

    g_debug ("Initializing presets...");
    presets_init (presets_signal_handler, iniFile);
    g_debug ("Setting next preset...");
    gchar *url = presets_next ();
    if (url)
    {
        g_debug ("Setting url %s", url);
        gstreamer_set_uri (url);
        g_debug ("Playing...");
        gstreamer_play ();
    }
    else
    {
        g_debug ("No preset!");
    }
    
    g_debug ("Running main-loop...");
    g_main_loop_run (main_loop);

    g_debug ("Cleaning up presets...");
    presets_cleanup();

    g_debug ("Cleaning up avtransport...");
    avtransport_cleanup(main_loop);

    g_debug ("Unreferencing...");
    g_main_loop_unref (main_loop);
    g_object_unref (connectionManagerService);
    g_object_unref (renderingControlService);
    g_object_unref (avTransportService);
    
    g_object_unref (dev);
    g_object_unref (context);

    g_debug ("Exiting...");
    return EXIT_SUCCESS;
}
