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
#include <glib.h>
 
/*总线消息处理回调函数*/
static gboolean
bus_call (GstBus     *bus,
          GstMessage *msg,
          gpointer    data)
{
  GMainLoop *loop = (GMainLoop *) data;
 
  switch (GST_MESSAGE_TYPE (msg)) {
 
    case GST_MESSAGE_EOS:
      g_print ("End of stream\n");
      g_main_loop_quit (loop);
      break;
 
    case GST_MESSAGE_ERROR: {
      gchar  *debug;
      GError *error;
 
      gst_message_parse_error (msg, &error, &debug);
      g_free (debug);
 
      g_printerr ("Error: %s\n", error->message);
      g_error_free (error);
 
      g_main_loop_quit (loop);
      break;
    }
    default:
      break;
  }
  return TRUE;
}
 
int
main (int   argc,
      char *argv[])
{
  GMainLoop *loop;
 
  GstElement *pipeline, *source, *parser, *decoder, *conv, *sink,*filter;
  GstBus *bus;
  guint bus_watch_id;
 
  /* Initialisation */
  gst_init (&argc, &argv);
 
  loop = g_main_loop_new (NULL, FALSE);
 
 
  /* Check input arguments */
  if (argc != 2) {
    g_printerr ("Usage: %s <Ogg/Vorbis filename>\n", argv[0]);
    return -1;
  }
 
 
  /* Create gstreamer elements */
  pipeline = gst_pipeline_new ("audio-player");
  source   = gst_element_factory_make ("filesrc",       "file-source");
  parser  = gst_element_factory_make ("mpegaudioparse", "mp3-parser");
  decoder  = gst_element_factory_make ("mpg123audiodec", "mp3-decoder");
  conv     = gst_element_factory_make ("audioconvert",  "converter");
   /* use "identity" here for a filter that does nothing */
  filter   = gst_element_factory_make ("my_filter", "my_filter");

  sink     = gst_element_factory_make ("pulsesink", "audio-output");
  // sink     = gst_element_factory_make ("autoaudiosink", "audio-output");
 
  if (!pipeline || !source || !parser || !decoder || !conv || !filter || !sink ) {
    g_printerr ("One element could not be created. Exiting.\n");
    return -1;
  }
 
  /* Set up the pipeline */
 
  /* we set the input filename to the source element */
  g_object_set (G_OBJECT (source), "location", argv[1], NULL);
 
  /* we add a message handler */
  bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
  bus_watch_id = gst_bus_add_watch (bus, bus_call, loop);
  gst_object_unref (bus);
 
  /* we add all elements into the pipeline */
  /* file-source | mp3-parser | mp3-decoder | converter | alsa-output */
  gst_bin_add_many (GST_BIN (pipeline),
                    source, parser, decoder, conv,filter, sink, NULL);
 
  /* we link the elements together */
  /* file-source -> mp3-parser ~> mp3-decoder -> converter -> alsa-output */
  //gst_element_link (source, parser);
  gst_element_link_many (source, parser, decoder, conv, filter, sink, NULL);
 
  /* note that the parser will be linked to the decoder dynamically.
     The reason is that Ogg may contain various streams (for example
     audio and video). The source pad(s) will be created at run time,
     by the parser when it detects the amount and nature of streams.
     Therefore we connect a callback function which will be executed
     when the "pad-added" is emitted.*/
 
 
  /* Set the pipeline to "playing" state*/
  g_print ("Now playing: %s\n", argv[1]);
  gst_element_set_state (pipeline, GST_STATE_PLAYING);
 
 
  /* Iterate */
  g_print ("Running...\n");
  g_main_loop_run (loop);
 
 
  /* Out of the main loop, clean up nicely */
  g_print ("Returned, stopping playback\n");
  gst_element_set_state (pipeline, GST_STATE_NULL);
 
  g_print ("Deleting pipeline\n");
  gst_object_unref (GST_OBJECT (pipeline));
  g_source_remove (bus_watch_id);
  g_main_loop_unref (loop);
 
  return 0;
}