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

#include "gstreamer.h"

#include <gst/gst.h>

const static gint ONE = 1;
GstElement *pipeline;
GstElement *source, *filter, *sink;
GString *mime_types = NULL;

GString*
gstreamer_format_time (gint64 time)
{
    gchar value[20];
    sprintf(value, "%" GST_TIME_FORMAT, GST_TIME_ARGS (time));
    return g_string_new(value);
}

void
gstreamer_print_uri (const char *prefix)
{
    gchar *uri;
    g_object_get (G_OBJECT (source), "location", &uri, NULL);
    g_debug("%s: %s", prefix, uri);
    g_free(uri);
}

void 
gstreamer_set_uri (gchar *uri)
{
    gst_element_set_state (pipeline, GST_STATE_NULL);
    g_object_set (G_OBJECT (source), "location", uri, NULL);

    gstreamer_print_uri ("location");
}

void
gstreamer_play ()
{
    gstreamer_print_uri ("play");

    gst_element_set_state (pipeline, GST_STATE_PLAYING);
}

void
gstreamer_pause ()
{
    gstreamer_print_uri ("pause");

    gst_element_set_state (pipeline, GST_STATE_PAUSED);
}

void
gstreamer_stop ()
{
    gstreamer_print_uri ("stop");

    gst_element_set_state (pipeline, GST_STATE_PAUSED);
}

gboolean
gstreamer_get_element_info (gint64 *position, gint64 *duration)
{
    GstFormat fmt = GST_FORMAT_TIME;
    gboolean result = FALSE;

    result = (gst_element_query_position (pipeline, &fmt, position)
              && gst_element_query_duration (pipeline, &fmt, duration));

    return result;
}

const char*
gstreamer_get_mime_types ()
{
    return mime_types->str;
}

/////////////////////////////////////////////////////////////////////

int
iterate_plugins_elements (GstElementFactory *factory, GHashTable *hashtable)
{
    GstElement *element;
    const GList *pads;
    GstStaticPadTemplate *padtemplate;
    const GstCaps * caps;
    int i;

    factory =
        GST_ELEMENT_FACTORY (gst_plugin_feature_load (GST_PLUGIN_FEATURE
                                                      (factory)));

    if (!factory) 
    {
        g_error ("element plugin couldn't be loaded");
        return -1;
    }

    element = gst_element_factory_create (factory, NULL);
    if (!element) 
    {
        g_error ("couldn't construct element for some reason");
        return -1;
    }

    if (factory->numpadtemplates) 
    {
        pads = factory->staticpadtemplates;
        while (pads) 
        {
            padtemplate = (GstStaticPadTemplate *) (pads->data);
            pads = g_list_next (pads);

            if (padtemplate->direction == GST_PAD_SINK 
                && padtemplate->presence == GST_PAD_ALWAYS
                && padtemplate->static_caps.string) {

                    caps = gst_static_caps_get (&padtemplate->static_caps);
                    if (caps != NULL && !gst_caps_is_any (caps) && !gst_caps_is_empty (caps))
                    {
                        for (i = 0; i < gst_caps_get_size (caps); i++) 
                        {
                            GstStructure *structure = gst_caps_get_structure (caps, i);
                            const gchar *mime = gst_structure_get_name (structure);
                            if (g_str_has_prefix (mime, "audio"))
                            {
                                g_hash_table_insert(hashtable, (gpointer)mime, (gpointer)&ONE);
                            }
                        }
                    }
                }
        }
    }

    gst_object_unref (element);
    gst_object_unref (factory);

    return 0;    
}

void
iterate_plugins (GHashTable *hashtable)
{
    GList *plugins, *orig_plugins;

    orig_plugins = plugins = gst_default_registry_get_plugin_list ();
    while (plugins) 
    {
        GList *features, *orig_features;
        GstPlugin *plugin;

        plugin = (GstPlugin *) (plugins->data);
        plugins = g_list_next (plugins);

        if (plugin->flags & GST_PLUGIN_FLAG_BLACKLISTED) 
        {
            continue;
        }

        orig_features = features =
            gst_registry_get_feature_list_by_plugin (gst_registry_get_default (),
                                                     plugin->desc.name);
        while (features) 
        {
            GstPluginFeature *feature;

            if (G_UNLIKELY (features->data == NULL))
                goto next;

            feature = GST_PLUGIN_FEATURE (features->data);
            if (GST_IS_ELEMENT_FACTORY (feature)) 
            {
                GstElementFactory *factory;

                factory = GST_ELEMENT_FACTORY (feature);
                iterate_plugins_elements (factory, hashtable);
            }
            next:
                features = g_list_next (features);
        }

        gst_plugin_feature_list_free (orig_features);
    }

    gst_plugin_list_free (orig_plugins);
}



void
iterator(gpointer key, gpointer value, gpointer user_data) 
{
    GString *mt = (GString *) user_data;
    g_string_append(mt, "http-get:*:");
    g_string_append(mt, (gchar*) key);
    g_string_append(mt, ":*,");
}

GString*
gstreamer_find_mime_types ()
{
    GString *mt_str = g_string_new ("");
    GHashTable *mt_hash = g_hash_table_new (g_str_hash, g_str_equal);
    iterate_plugins (mt_hash);
    g_hash_table_foreach (mt_hash, (GHFunc) iterator, mt_str);
    g_string_erase(mt_str, mt_str->len - 1, 1);
    g_hash_table_destroy (mt_hash);

    g_debug ("mime-types: %s", mt_str->str);

    return mt_str;
}


gboolean
bus_call (GstBus *bus, GstMessage *msg, gpointer data)
{
    switch (GST_MESSAGE_TYPE (msg)) {

        case GST_MESSAGE_EOS:
            g_debug ("End of stream");

            //set_transport_state (NULL, STOPPED);
            break;

        case GST_MESSAGE_ERROR: 
        {
            gchar  *debug;
            GError *error;

            gst_message_parse_error (msg, &error, &debug);

            g_printerr ("Error: %s (%s)", error->message, debug);
            g_free (debug);
            g_error_free (error);

            break;
        }
        default:
            break;
    }

    return TRUE;
}

/*
 gboolean
 cb_print_position (GstElement *pipeline)
 {
     GstFormat fmt = GST_FORMAT_TIME;

     if (!gst_element_query_position (pipeline, &fmt, &relative_time_position))
     {
         // TODO: Error
         }

         if (!gst_element_query_duration (pipeline, &fmt, &sv_current_track_duration)) 
         {
             // TODO: Error
             }

             // call me again
             return TRUE;
             }
*/

             

void
gstreamer_init (GMainLoop *main_loop)
{
    GstBus *bus;

    gst_init (NULL, NULL);

    pipeline = gst_pipeline_new ("m3player-pipeline");

    source = gst_element_factory_make ("souphttpsrc", "source");
    filter = gst_element_factory_make ("mad", "filter");
    sink = gst_element_factory_make ("alsasink", "sink");

    gst_bin_add_many (GST_BIN (pipeline), source, filter, sink, NULL);

    if (!gst_element_link_many (source, filter, sink, NULL)) {
        g_warning ("Failed to link elements!");
    }

    // set up
    //pipeline = gst_element_factory_make ("playbin2", "play");
    //g_debug("pipeline=%ld", (long int) pipeline):


    bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
    g_debug("bus=%ld", (long int) bus);
    gst_bus_add_watch (bus, bus_call, main_loop);
    gst_object_unref (bus);

    //DISABLED: g_timeout_add (500, (GSourceFunc) cb_print_position, pipeline);

    mime_types = gstreamer_find_mime_types ();
}

void
gstreamer_cleanup ()
{
    g_string_free (mime_types, TRUE);

    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (GST_OBJECT (source));
    gst_object_unref (GST_OBJECT (filter));
    gst_object_unref (GST_OBJECT (sink));
    gst_object_unref (GST_OBJECT (pipeline));
}
