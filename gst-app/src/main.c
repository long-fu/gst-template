/* Copyright (C) 2006 Tim-Philipp Müller <tim centricular net>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Alternatively, the contents of this file may be used under the
 * GNU Lesser General Public License Version 2.1 (the "LGPL"), in
 * which case the following provisions apply instead of the ones
 * mentioned above:
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gst-app.h"

// static void
// handle_file_or_directory (const gchar * filename)
// {
//   GError *err = NULL;
//   GDir *dir;
//   gchar *uri;

//   if ((dir = g_dir_open (filename, 0, NULL))) {
//     const gchar *entry;

//     while ((entry = g_dir_read_name (dir))) {
//       gchar *path;

//       path = g_strconcat (filename, G_DIR_SEPARATOR_S, entry, NULL);
//       handle_file_or_directory (path);
//       g_free (path);
//     }

//     g_dir_close (dir);
//     return;
//   }

//   if (g_path_is_absolute (filename)) {
//     uri = g_filename_to_uri (filename, NULL, &err);
//   } else {
//     gchar *curdir, *absolute_path;

//     curdir = g_get_current_dir ();
//     absolute_path = g_strconcat ( curdir, G_DIR_SEPARATOR_S, filename, NULL);
//     uri = g_filename_to_uri (absolute_path, NULL, &err);
//     g_free (absolute_path);
//     g_free (curdir);
//   }

//   if (uri) {
//     /* great, we have a proper file:// URI, let's play it! */
//     play_uri (uri);
//   } else {
//     g_warning ("Failed to convert filename '%s' to URI: %s", filename,
//         err->message);
//     g_error_free (err);
//   }

//   g_free (uri);
// }

// int
// main (int argc, char *argv[])
// {
//   gchar *filenames[] = {"/home/haoshuai/video.mp4"};
//   const GOptionEntry entries[] = {
//     /* you can add your won command line options here */
//     { G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_FILENAME_ARRAY, &filenames,
//       "Special option that collects any remaining arguments for us" },
//     { NULL, }
//   };
//   GOptionContext *ctx;
//   GError *err = NULL;
//   gint i, num;

//   ctx = g_option_context_new ("[FILE1] [FILE2] ...");
//   g_option_context_add_group (ctx, gst_init_get_option_group ());
//   g_option_context_add_main_entries (ctx, entries, NULL);

//   if (!g_option_context_parse (ctx, &argc, &argv, &err)) {
//     g_print ("Error initializing: %s\n", GST_STR_NULL (err->message));
//     return -1;
//   }
//   g_option_context_free (ctx);

//   if (filenames == NULL || *filenames == NULL) {
//     g_print ("Please specify a file to play\n\n");
//     return -1;
//   }



//   num = g_strv_length (filenames);

//   for (i = 0; i < num; ++i) {
//     handle_file_or_directory (filenames[i]);
//   }

//   g_strfreev (filenames);

//   return 0;
// }


#include <gst/gst.h>

static gboolean
bus_call (GstBus     *bus,
      GstMessage *msg,
      gpointer    data)
{
  GMainLoop *loop = data;

  switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_EOS:
      g_print ("End-of-stream\n");
      g_main_loop_quit (loop);
      break;
    case GST_MESSAGE_ERROR: {
      gchar *debug = NULL;
      GError *err = NULL;

      gst_message_parse_error (msg, &err, &debug);

      g_print ("Error: %s\n", err->message);
      g_error_free (err);

      if (debug) {
        g_print ("Debug details: %s\n", debug);
        g_free (debug);
      }

      g_main_loop_quit (loop);
      break;
    }
    default:
      break;
  }

  return TRUE;
}

gint
main (gint   argc,
      gchar *argv[])
{
  GstStateChangeReturn ret;
  GstElement *pipeline, *filesrc, *decoder, *filter, *sink;
  GstElement *convert1, *convert2, *resample;
  GMainLoop *loop;
  GstBus *bus;
  guint watch_id;

  /* initialization */
  gst_init (&argc, &argv);
  loop = g_main_loop_new (NULL, FALSE);
  if (argc != 2) {
    g_print ("Usage: %s <mp3 filename>\n", argv[0]);
    return 01;
  }

  /* create elements */
  pipeline = gst_pipeline_new ("my_pipeline");

  /* watch for messages on the pipeline's bus (note that this will only
   * work like this when a GLib main loop is running) */
  bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
  watch_id = gst_bus_add_watch (bus, bus_call, loop);
  gst_object_unref (bus);

  filesrc  = gst_element_factory_make ("filesrc", "my_filesource");
  decoder  = gst_element_factory_make ("mad", "my_decoder");

  /* putting an audioconvert element here to convert the output of the
   * decoder into a format that my_filter can handle (we are assuming it
   * will handle any sample rate here though) */
  convert1 = gst_element_factory_make ("audioconvert", "audioconvert1");

  /* use "identity" here for a filter that does nothing */
  filter   = gst_element_factory_make ("my_filter", "my_filter");

  /* there should always be audioconvert and audioresample elements before
   * the audio sink, since the capabilities of the audio sink usually vary
   * depending on the environment (output used, sound card, driver etc.) */
  convert2 = gst_element_factory_make ("audioconvert", "audioconvert2");
  resample = gst_element_factory_make ("audioresample", "audioresample");
  sink     = gst_element_factory_make ("pulsesink", "audiosink");

  if (!sink || !decoder) {
    g_print ("Decoder or output could not be found - check your install\n");
    return -1;
  } else if (!convert1 || !convert2 || !resample) {
    g_print ("Could not create audioconvert or audioresample element, "
             "check your installation\n");
    return -1;
  } else if (!filter) {
    g_print ("Your self-written filter could not be found. Make sure it "
             "is installed correctly in $(libdir)/gstreamer-1.0/ or "
             "~/.gstreamer-1.0/plugins/ and that gst-inspect-1.0 lists it. "
             "If it doesn't, check with 'GST_DEBUG=*:2 gst-inspect-1.0' for "
             "the reason why it is not being loaded.");
    return -1;
  }

  g_object_set (G_OBJECT (filesrc), "location", argv[1], NULL);

  gst_bin_add_many (GST_BIN (pipeline), filesrc, decoder, convert1, filter,
                    convert2, resample, sink, NULL);

  /* link everything together */
  if (!gst_element_link_many (filesrc, decoder, convert1, filter, convert2,
                              resample, sink, NULL)) {
    g_print ("Failed to link one or more elements!\n");
    return -1;
  }

  /* run */
  ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
  if (ret == GST_STATE_CHANGE_FAILURE) {
    GstMessage *msg;

    g_print ("Failed to start up pipeline!\n");

    /* check if there is an error message with details on the bus */
    msg = gst_bus_poll (bus, GST_MESSAGE_ERROR, 0);
    if (msg) {
      GError *err = NULL;

      gst_message_parse_error (msg, &err, NULL);
      g_print ("ERROR: %s\n", err->message);
      g_error_free (err);
      gst_message_unref (msg);
    }
    return -1;
  }

  g_main_loop_run (loop);

  /* clean up */
  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (pipeline);
  g_source_remove (watch_id);
  g_main_loop_unref (loop);

  return 0;
}
