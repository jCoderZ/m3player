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
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "renderingcontrol.h"
#include "connectionmanager.h"
#include "avtransport.h"
#include "presets.h"
#include "gstreamer.h"

#define die(_fmt, ...)  printf(_fmt, __VA_ARGS__)

#define CONFIG_FILE_DEFAULT "/etc/m3player/m3player.ini"
#define PID_FILE_DEFAULT "/var/run/m3player.pid"
#define XML_FOLDER_DEFAULT "/usr/share/m3player"
#define ROOT_FILE_DEFAULT "MediaRendererV2.xml"
#define LOG_FILE_DEFAULT "/var/log/m3player.log"

static GUPnPContext *context;
static GUPnPRootDevice *dev;

static GUPnPService *connectionManagerService;
static GUPnPService *renderingControlService;
static GUPnPService *avTransportService;

static const gchar *configFile = NULL;
static const gchar *pidFile = NULL;
static const gchar *xmlFolder = NULL;
static const gchar *rootFile = NULL;
static const gchar *logFile = NULL;
static const gchar *interface = NULL;
static gchar *hostName = NULL;
static gint makeDaemon = 0;

static GOptionEntry entries[] = {
  { "config", 'c', 0, G_OPTION_ARG_STRING, &configFile, "Path to the config file (Default: " CONFIG_FILE_DEFAULT ")", NULL },
  { "pid", 'p', 0, G_OPTION_ARG_STRING, &pidFile, "Path to the pid file (Default: " PID_FILE_DEFAULT ")", NULL },
  { "xml", 'x', 0, G_OPTION_ARG_STRING, &xmlFolder, "Path to the XML folder file (Default: " XML_FOLDER_DEFAULT ")", NULL },
  { "name", 'n', 0, G_OPTION_ARG_STRING, &hostName, "The name of the player instance (Default: HOSTNAME)", NULL },
  { "root", 'r', 0, G_OPTION_ARG_STRING, &rootFile, "The name of the root device file (Default: " ROOT_FILE_DEFAULT ")", NULL },
  { "log", 'l', 0, G_OPTION_ARG_STRING, &logFile, "The name of the log file (Default: " LOG_FILE_DEFAULT ")", NULL },
  { "interface", 'i', 0, G_OPTION_ARG_STRING, &interface, "The name of the interface the process is bound to (e.g. eth0)", NULL },
  { "daemonise", 'd', 0, G_OPTION_ARG_NONE, &makeDaemon, "Fork the player as daemon", NULL },
  { NULL }
};

static GKeyFile *iniFile = NULL;


void
presets_signal_handler (int sig) {
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

gint
gupnp_init (const gchar* fileName, const gchar *xmlFolder) {
    GError *error = NULL;
    g_debug ("Create the UPnP context (interface: %s)", interface);
    context = gupnp_context_new (NULL, interface, 0, &error);
    if (error) {
        g_printerr ("Error creating the GUPnP context: %s", error->message);
        g_error_free (error);

        return EXIT_FAILURE;
    }
    g_debug ("Running on port %d", gupnp_context_get_port (context));

    g_debug ("Create root device");
    dev = gupnp_root_device_new (context, fileName, xmlFolder);

    char devInfo[256];
    const char *ver1 = strstr(fileName, "V1");

    g_debug ("Announce root device");
    gupnp_root_device_set_available (dev, TRUE);

    g_debug ("Get the connection manager service V%d", (ver1 ? 1 : 2));
    sprintf(devInfo, "urn:schemas-upnp-org:service:ConnectionManager:%d", (ver1 ? 1 : 2));
    connectionManagerService = GUPNP_SERVICE(gupnp_device_info_get_service
        (GUPNP_DEVICE_INFO (dev), devInfo));
    if (!connectionManagerService) {
        g_printerr ("Cannot get ConnectionManager service");

        return EXIT_FAILURE;
    }
    gupnp_service_signals_autoconnect (GUPNP_SERVICE (connectionManagerService), NULL, &error);
    if (error) {
        g_printerr ("Could not autoconnect signals: %s", error->message);
        g_error_free (error);

        return EXIT_FAILURE;
    }

    g_debug ("Get the rendering control service V%d", (ver1 ? 1 : 2));
    sprintf(devInfo, "urn:schemas-upnp-org:service:RenderingControl:%d", (ver1 ? 1 : 2));
    renderingControlService = GUPNP_SERVICE(gupnp_device_info_get_service
        (GUPNP_DEVICE_INFO (dev), devInfo));
    if (!renderingControlService) {
        g_printerr ("Cannot get RenderingControl service");

        return EXIT_FAILURE;
    }
    gupnp_service_signals_autoconnect (GUPNP_SERVICE (renderingControlService), NULL, &error);
    if (error) {
        g_printerr ("Could not autoconnect signals: %s", error->message);
        g_error_free (error);

        return EXIT_FAILURE;
    }
    
    g_debug ("Get the av transport service V%d", (ver1 ? 1 : 2));
    sprintf(devInfo, "urn:schemas-upnp-org:service:AVTransport:%d", (ver1 ? 1 : 2));
    avTransportService = GUPNP_SERVICE(gupnp_device_info_get_service
        (GUPNP_DEVICE_INFO (dev), devInfo));
    if (!avTransportService) {
        g_printerr ("Cannot get AVTransport service");

        return EXIT_FAILURE;
    }
    gupnp_service_signals_autoconnect (GUPNP_SERVICE (avTransportService), NULL, &error);
    if (error) {
        g_printerr ("Could not autoconnect signals: %s", error->message);
        g_error_free (error);

        return EXIT_FAILURE;
    }
    // additionally connect avtransport's version of last_change
//    avtransport_last_change_query_connect (GUPNP_SERVICE (avTransportService), 
//                                           query_avtransport_last_change_cb, NULL);
        
    return EXIT_SUCCESS;
}

/*
   Cause a process to become a daemon
   http://www.microhowto.info/howto/cause_a_process_to_become_a_daemon.html
 */
void
daemonise() {
    // already a daemon
    int ppid = getppid();
	if(ppid == 1) {
        die("we are already a daemon (%d)", ppid);
    }

    // Fork, allowing the parent process to terminate.
    pid_t pid = fork();
    if (pid == -1) {
        die("failed to fork while daemonising (errno=%d)",errno);
    } else if (pid != 0) {
        _exit(0);
    }

    // Start a new session for the daemon.
    if (setsid()==-1) {
        die("failed to become a session leader while daemonising(errno=%d)",errno);
    }

    // Fork again, allowing the parent process to terminate.
    signal(SIGHUP,SIG_IGN);
    pid=fork();
    if (pid == -1) {
        die("failed to fork while daemonising (errno=%d)",errno);
    } else if (pid != 0) {
        _exit(0);
    }

    // Set the current working directory to the root directory.
    if (chdir("/") == -1) {
        die("failed to change working directory while daemonising (errno=%d)",errno);
    }

    // Set the user file creation mask to zero.
    umask(0);

    // Close then reopen standard file descriptors.
    close(STDIN_FILENO);
    if (open("/dev/null",O_RDONLY) == -1) {
        die("failed to reopen stdin while daemonising (errno=%d)",errno);
    }

    int logfile_fileno = open(logFile,O_RDWR|O_CREAT|O_APPEND,S_IRUSR|S_IWUSR|S_IRGRP);
    if (logfile_fileno == -1) {
        die("failed to open logfile (errno=%d)",errno);
    }
    dup2(logfile_fileno,STDOUT_FILENO);
    dup2(logfile_fileno,STDERR_FILENO);
    close(logfile_fileno);

    // ignore child
    signal(SIGCHLD,SIG_IGN);
    // ignore tty signals
	signal(SIGTSTP,SIG_IGN);
	signal(SIGTTOU,SIG_IGN);
	signal(SIGTTIN,SIG_IGN);
}

void
handleParameters(int argc, char **argv) {
    GError *optionError = NULL;
    GOptionContext *context;
    context = g_option_context_new ("m3player");
    g_option_context_add_main_entries (context, entries, NULL);
    if (!g_option_context_parse (context, &argc, &argv, &optionError)) {
        g_printerr ("option parsing failed: %s\n", optionError->message);
        g_error_free (optionError);
        exit (1);
    }
    
    // Set defaults in case no command-line parameter has been given
    if (configFile == NULL) {
        configFile = CONFIG_FILE_DEFAULT;
    }

    if (xmlFolder == NULL) {
        xmlFolder = XML_FOLDER_DEFAULT;
    }

    if (rootFile == NULL) {
        rootFile = ROOT_FILE_DEFAULT;
    }

    if (pidFile == NULL) {
        pidFile = PID_FILE_DEFAULT;
    }

    if (logFile == NULL) {
        logFile = LOG_FILE_DEFAULT;
    }

    if (!hostName) {
        gchar host[HOST_NAME_MAX];
        if (gethostname(host, HOST_NAME_MAX) == 0) {
            hostName = g_strdup (host);
        }
        else {
            hostName = g_strdup ("m3player");
        }
    }

    // Load ini file
    if (g_file_test (configFile, G_FILE_TEST_EXISTS)) {
        GError *iniError = NULL;
        iniFile = g_key_file_new ();
        if (!g_key_file_load_from_file (iniFile, configFile, G_KEY_FILE_NONE, &iniError)) {
            g_printerr ("reading ini file failed! %s\n", iniError->message);
            g_error_free (iniError);
            exit (2);
        }
    }
    else {
        g_printerr ("Specified config file does not exist: %s\n", configFile);
        exit (3);
    }

    // Test whether the xml folder exists
    if (!g_file_test (xmlFolder, G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR)) {
        g_printerr ("Specified XML folder does not exist: %s\n", xmlFolder);
        exit (4);
    }
}

int
main (int argc, char **argv) {
    handleParameters(argc, argv);

    if (makeDaemon) {
        daemonise();
    }

    g_debug ("Configuration file: %s", configFile);
    g_debug ("XML folder: %s", xmlFolder);
    g_debug ("Root device file: %s", rootFile);
    g_debug ("PID file: %s", pidFile);
    g_debug ("Log file: %s", logFile);
    g_debug ("hostname: %s", hostName);
    g_debug ("Daemonise: %d", makeDaemon);
        
    g_debug ("Create new main loop...");
    GMainLoop *main_loop = g_main_loop_new (NULL, FALSE);

    // Initialize sub-systems
    g_thread_init (NULL);
    g_type_init ();

    g_debug ("Initializing gstreamer sub-system...");
    gstreamer_init (main_loop);

    int rc = gupnp_init (rootFile, xmlFolder);
    if (rc != 0) {
        g_printerr ("GUPnP initialization failed!");
        return rc;
    }

    g_debug ("Initializing AVTransport instance...");
    avtransport_init(main_loop);

    g_debug ("Initializing presets...");
    presets_init (presets_signal_handler, iniFile);
    g_debug ("Setting next preset...");
    gchar *url = presets_next ();
    if (url) {
        g_debug ("Setting url %s", url);
        gstreamer_set_uri (url);
        g_debug ("Playing...");
        gstreamer_play ();
    }
    else {
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

    g_free (hostName);
    
    g_debug ("Exiting...");
    return EXIT_SUCCESS;
}
