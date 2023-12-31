/*
 * GStreamer
 * Copyright (C) 2005 Thomas Vander Stichele <thomas@apestaart.org>
 * Copyright (C) 2005 Ronald S. Bultje <rbultje@ronald.bitfreak.net>
 * Copyright (C) 2023 haoshuai <<user@hostname.org>>
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

/**
 * SECTION:element-myfilter
 *
 * FIXME:Describe myfilter here.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m fakesrc ! myfilter ! fakesink silent=TRUE
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gst/gst.h>

#include "gstmyfilter.h"

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
GST_DEBUG_CATEGORY_STATIC(gst_my_filter_debug);
#define GST_CAT_DEFAULT gst_my_filter_debug

/* Filter signals and args */
enum
{
  /* FILL ME */
  LAST_SIGNAL
};

enum
{
  PROP_0,
  PROP_SILENT
};

/* FIXME: add/remove formats you can handle */
#define VIDEO_SRC_CAPS \
  GST_VIDEO_CAPS_MAKE("{ I420, Y444, Y42B, UYVY, RGBA }")

/* FIXME: add/remove formats you can handle */
#define VIDEO_SINK_CAPS \
  GST_VIDEO_CAPS_MAKE("{ I420, Y444, Y42B, UYVY, RGBA }")

/* the capabilities of the inputs and outputs.
 *
 * describe the real formats here.
 */
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE("sink",
                                                                   GST_PAD_SINK,
                                                                   GST_PAD_ALWAYS,
                                                                   GST_STATIC_CAPS("ANY"));

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE("src",
                                                                  GST_PAD_SRC,
                                                                  GST_PAD_ALWAYS,
                                                                  GST_STATIC_CAPS("ANY"));

#define gst_my_filter_parent_class parent_class
G_DEFINE_TYPE(GstMyFilter, gst_my_filter, GST_TYPE_ELEMENT);

GST_ELEMENT_REGISTER_DEFINE(my_filter, "my_filter", GST_RANK_NONE,
                            GST_TYPE_MYFILTER);

static void gst_my_filter_set_property(GObject *object,
                                       guint prop_id, const GValue *value, GParamSpec *pspec);
static void gst_my_filter_get_property(GObject *object,
                                       guint prop_id, GValue *value, GParamSpec *pspec);

static gboolean gst_my_filter_sink_event(GstPad *pad,
                                         GstObject *parent, GstEvent *event);
static GstFlowReturn gst_my_filter_chain(GstPad *pad,
                                         GstObject *parent, GstBuffer *buf);

/* GObject vmethod implementations */

/* initialize the myfilter's class */
static void
gst_my_filter_class_init(GstMyFilterClass *klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;

  gobject_class = (GObjectClass *)klass;
  gstelement_class = (GstElementClass *)klass;

  gobject_class->set_property = gst_my_filter_set_property;
  gobject_class->get_property = gst_my_filter_get_property;

  g_object_class_install_property(gobject_class, PROP_SILENT,
                                  g_param_spec_boolean("silent", "Silent", "Produce verbose output ?",
                                                       FALSE, G_PARAM_READWRITE));

  gst_element_class_set_details_simple(gstelement_class,
                                       "MyFilter",
                                       "FIXME:Generic",
                                       "FIXME:Generic Template Element", "haoshuai <<user@hostname.org>>");

  gst_element_class_add_pad_template(gstelement_class,
                                     gst_static_pad_template_get(&src_factory));
  gst_element_class_add_pad_template(gstelement_class,
                                     gst_static_pad_template_get(&sink_factory));
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad callback functions
 * initialize instance structure
 */
static void
gst_my_filter_init(GstMyFilter *filter)
{
  filter->sinkpad = gst_pad_new_from_static_template(&sink_factory, "sink");

  gst_pad_set_event_function(filter->sinkpad,
                             GST_DEBUG_FUNCPTR(gst_my_filter_sink_event));
  gst_pad_set_chain_function(filter->sinkpad,
                             GST_DEBUG_FUNCPTR(gst_my_filter_chain));
  GST_PAD_SET_PROXY_CAPS(filter->sinkpad);

  gst_element_add_pad(GST_ELEMENT(filter), filter->sinkpad);

  filter->srcpad = gst_pad_new_from_static_template(&src_factory, "src");

  GST_PAD_SET_PROXY_CAPS(filter->srcpad);

  gst_element_add_pad(GST_ELEMENT(filter), filter->srcpad);

  filter->silent = FALSE;
}

static void
gst_my_filter_set_property(GObject *object, guint prop_id,
                           const GValue *value, GParamSpec *pspec)
{
  GstMyFilter *filter = GST_MYFILTER(object);

  switch (prop_id)
  {
  case PROP_SILENT:
    filter->silent = g_value_get_boolean(value);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
    break;
  }
}

static void
gst_my_filter_get_property(GObject *object, guint prop_id,
                           GValue *value, GParamSpec *pspec)
{
  GstMyFilter *filter = GST_MYFILTER(object);

  switch (prop_id)
  {
  case PROP_SILENT:
    g_value_set_boolean(value, filter->silent);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
    break;
  }
}

/* GstElement vmethod implementations */

/* this function handles sink events */
static gboolean
gst_my_filter_sink_event(GstPad *pad, GstObject *parent,
                         GstEvent *event)
{
  GstMyFilter *filter;
  gboolean ret;

  filter = GST_MYFILTER(parent);

  GST_LOG_OBJECT(filter, "Received %s event: %" GST_PTR_FORMAT,
                 GST_EVENT_TYPE_NAME(event), event);
  // pad->
  g_print("Received %d event \n", GST_EVENT_TYPE(event));

  switch (GST_EVENT_TYPE(event))
  {
  case GST_EVENT_CAPS:
  {
    GstCaps *caps;

    gst_event_parse_caps(event, &caps);
    /* do something with the caps */
    // 保存没一帧数据

    /* and forward */
    ret = gst_pad_event_default(pad, parent, event);
    break;
  }
  default:
    ret = gst_pad_event_default(pad, parent, event);
    break;
  }
  return ret;
}

typedef __uint8_t uint8_t;
typedef __uint16_t uint16_t;
typedef __uint32_t uint32_t;
typedef __uint64_t uint64_t;

typedef struct _YUVColor
{
  uint8_t y;
  int8_t u;
  int8_t v;
} YUVColor;

void SetPixel(uint8_t *image_data, int image_width, int image_height, int x, int y, const YUVColor color)
{
  // *(image_data + y * image_width + x) = color.y;
  int ty = image_width * y + x;
  *(image_data + ty) = color.y;
  // uint8_t *uv_offset = image_data + image_width * image_height + (y / 2) * image_width + x / 2 * 2;
  int u = image_width * image_height * 5 / 4 + (y / 2 * image_width / 2 + x / 2);
  *(image_data + u) = color.u;
  int v = image_width * image_height + (y / 2 * image_width / 2 + x / 2);
  *(image_data + v) = color.v;
  // uv_offset[0] = color.u;
  // uv_offset[1] = color.v;
}

void draw_rect(uint8_t *image_data, int image_width, int image_height, int x1, int y1, int x2, int y2, const YUVColor color,
               int lineWidth)
{
  if (x1 > x2)
  {
    // swap(x1, x2);
    int tmp = x1;
    x1 = x2;
    x2 = tmp;
  }

  if (y1 > y2)
  {
    int tmp = y1;
    y1 = y2;
    y2 = tmp;
    // swap(y1, y2);
  }

  int i, j;
  int iBound, jBound;
  int iStart, jStart;
  int width = (int)image_width;
  int height = (int)image_height;

  jBound = MIN(height, y1 + lineWidth);
  iBound = MIN(width - 1, x2);

  for (j = y1; j < jBound; ++j)
  {
    for (i = x1; i <= iBound; ++i)
    {
      SetPixel(image_data, image_width, image_height, i, j, color);
    }
  }

  jStart = MAX(0, y2 - lineWidth + 1);
  jBound = MIN(height - 1, y2);

  iStart = MAX(0, x1);
  iBound = MIN(width - 1, x2);

  for (j = jStart; j <= jBound; ++j)
  {
    for (i = iStart; i <= iBound; ++i)
    {
      SetPixel(image_data, image_width, image_height, i, j, color);
    }
  }

  iBound = MIN(width, x1 + lineWidth);
  jBound = MIN(height - 1, y2);

  for (i = x1; i < iBound; ++i)
  {
    for (j = y1; j <= jBound; ++j)
    {
      SetPixel(image_data, image_width, image_height, i, j, color);
    }
  }

  iStart = MAX(0, (x2 - lineWidth + 1));
  iBound = MIN(width - 1, x2);

  jStart = MAX(0, y1);
  jBound = MIN(height - 1, y2);

  for (i = iStart; i <= iBound; ++i)
  {
    for (j = jStart; j <= jBound; ++j)
    {
      SetPixel(image_data, image_width, image_height, i, j, color);
    }
  }
}

/* chain function
 * this function does the actual processing
 */
static GstFlowReturn
gst_my_filter_chain(GstPad *pad, GstObject *parent, GstBuffer *inbuf)
{
  GstFlowReturn ret;
  GstMyFilter *filter;
  GstBuffer *outbuf;

  GstMapInfo map;
  guint8 *data;
  gsize size;
  void *file_data;
  static int index = 0;
  filter = GST_MYFILTER(parent);

  if (filter->silent == FALSE)
  {
    gst_buffer_map(inbuf, &map, GST_MAP_READ);
    data = map.data;
    size = map.size;

    gst_buffer_unmap(inbuf, &map);

    // gsize temp_size = gst_buffer_get_size(inbuf); temp_size == size
    // g_print ("Have data of size %" G_GSIZE_FORMAT" / %" G_GSIZE_FORMAT" bytes!\n", size, temp_size);
    outbuf = gst_buffer_new_and_alloc(size);
    gst_buffer_copy_into(outbuf, inbuf, GST_BUFFER_COPY_TIMESTAMPS, 0, size);
    gst_buffer_map(outbuf, &map, GST_MAP_WRITE);
    // map.data
    memcpy(map.data, data, size);
    YUVColor color;
    // 41.92,-4.891,75.492
    // 推理

    // 推理结果

    // 绘制结果

    color.u = -5;
    color.v = 76;
    color.y = 42;
    draw_rect(map.data, 1280, 720, 20, 20, 1260, 700, color, 5);
    // data = map.data;
    // size = map.size;
    gst_buffer_unmap(outbuf, &map);
    //   g_print ("Have data of size %" G_GSIZE_FORMAT" bytes!\n", size);
    //   if(!gst_buffer_map (buf, &info, GST_MAP_READ)) {
    //     // GST_ELEMENT_ERROR (decoder, STREAM, DECODE,
    //     //       ("Failed to map codec data"), (NULL));

    //     g_print("read mem Failed\n");
    //     return ;
    //   }
    //   // data是视频帧的每一帧数据

    file_data = malloc(size);
    memcpy(file_data,map.data,size);
    char file_name[64] = {0x0};
    sprintf(file_name,"/home/haoshuai/code/gst/gst-template/data/bin_%d.yuv",index);
    save_file(file_name,file_data,size);
    free(file_data);
      index++;
  }

  /* just push out the incoming buffer without touching it */
  return gst_pad_push(filter->srcpad, outbuf);
}

void save_file(char *file, void *file_data, gsize size)
{
  FILE *outFileFp = fopen(file, "wb+");
  if (outFileFp == NULL)
  {
    // ERROR_LOG("Save file %s failed for open error", file);
    g_print("Save file %s failed for open error\n", file);
    return;
  }
  fwrite(file_data, 1, size, outFileFp);

  fflush(outFileFp);
  fclose(outFileFp);
}

/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
static gboolean
myfilter_init(GstPlugin *myfilter)
{
  /* debug category for filtering log messages
   *
   * exchange the string 'Template myfilter' with your description
   */
  GST_DEBUG_CATEGORY_INIT(gst_my_filter_debug, "myfilter",
                          0, "Template myfilter");

  return GST_ELEMENT_REGISTER(my_filter, myfilter);
}

/* PACKAGE: this is usually set by meson depending on some _INIT macro
 * in meson.build and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use meson to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "myfirstmyfilter"
#endif

/* gstreamer looks for this structure to register myfilters
 *
 * exchange the string 'Template myfilter' with your myfilter description
 */
GST_PLUGIN_DEFINE(GST_VERSION_MAJOR,
                  GST_VERSION_MINOR,
                  myfilter,
                  "my_filter",
                  myfilter_init,
                  PACKAGE_VERSION, GST_LICENSE, GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)
