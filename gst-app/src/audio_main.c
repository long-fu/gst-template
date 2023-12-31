
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