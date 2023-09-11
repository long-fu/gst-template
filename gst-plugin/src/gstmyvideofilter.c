/* GStreamer
 * Copyright (C) 2023 FIXME <fixme@example.com>
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
 * Free Software Foundation, Inc., 51 Franklin Street, Suite 500,
 * Boston, MA 02110-1335, USA.
 */
/**
 * SECTION:element-gstmyvideofilter
 *
 * The myvideofilter element does FIXME stuff.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch-1.0 -v fakesrc ! myvideofilter ! FIXME ! fakesink
 * ]|
 * FIXME Describe what the pipeline does.
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>
#include <gst/video/video.h>
#include <gst/video/gstvideofilter.h>
#include "gstmyvideofilter.h"

#include <gst/video/video.h>
#include <gst/video/gstvideometa.h>
#include <gst/video/gstvideopool.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

GST_DEBUG_CATEGORY_STATIC(gst_myvideofilter_debug_category);
#define GST_CAT_DEFAULT gst_myvideofilter_debug_category

/* prototypes */

static void gst_myvideofilter_set_property(GObject *object,
                                           guint property_id, const GValue *value, GParamSpec *pspec);
static void gst_myvideofilter_get_property(GObject *object,
                                           guint property_id, GValue *value, GParamSpec *pspec);
static void gst_myvideofilter_dispose(GObject *object);
static void gst_myvideofilter_finalize(GObject *object);

static gboolean gst_myvideofilter_start(GstBaseTransform *trans);
static gboolean gst_myvideofilter_stop(GstBaseTransform *trans);
static gboolean gst_myvideofilter_set_info(GstVideoFilter *filter, GstCaps *incaps,
                                           GstVideoInfo *in_info, GstCaps *outcaps, GstVideoInfo *out_info);
static GstFlowReturn gst_myvideofilter_transform_frame(GstVideoFilter *filter,
                                                       GstVideoFrame *inframe, GstVideoFrame *outframe);
static GstFlowReturn gst_myvideofilter_transform_frame_ip(GstVideoFilter *filter,
                                                          GstVideoFrame *frame);

enum
{
  PROP_0
};

/* pad templates */

/* FIXME: add/remove formats you can handle */
#define VIDEO_SRC_CAPS \
  GST_VIDEO_CAPS_MAKE("{ YV12 }")

/* FIXME: add/remove formats you can handle */
#define VIDEO_SINK_CAPS \
  GST_VIDEO_CAPS_MAKE("{ YV12 }")

/* class initialization */

G_DEFINE_TYPE_WITH_CODE(GstMyVideoFilter, gst_myvideofilter, GST_TYPE_VIDEO_FILTER,
                        GST_DEBUG_CATEGORY_INIT(gst_myvideofilter_debug_category, "myvideofilter", 0,
                                                "debug category for myvideofilter element"));

static void
gst_myvideofilter_class_init(GstMyVideoFilterClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GstBaseTransformClass *base_transform_class = GST_BASE_TRANSFORM_CLASS(klass);
  GstVideoFilterClass *video_filter_class = GST_VIDEO_FILTER_CLASS(klass);

  /* Setting up pads and setting metadata should be moved to
     base_class_init if you intend to subclass this class. */
  gst_element_class_add_pad_template(GST_ELEMENT_CLASS(klass),
                                     gst_pad_template_new("src", GST_PAD_SRC, GST_PAD_ALWAYS,
                                                          gst_caps_from_string(VIDEO_SRC_CAPS)));
  gst_element_class_add_pad_template(GST_ELEMENT_CLASS(klass),
                                     gst_pad_template_new("sink", GST_PAD_SINK, GST_PAD_ALWAYS,
                                                          gst_caps_from_string(VIDEO_SINK_CAPS)));

  gst_element_class_set_static_metadata(GST_ELEMENT_CLASS(klass),
                                        "FIXME Long name", "Generic", "FIXME Description",
                                        "FIXME <fixme@example.com>");

  gobject_class->set_property = gst_myvideofilter_set_property;
  gobject_class->get_property = gst_myvideofilter_get_property;
  gobject_class->dispose = gst_myvideofilter_dispose;
  gobject_class->finalize = gst_myvideofilter_finalize;
  base_transform_class->start = GST_DEBUG_FUNCPTR(gst_myvideofilter_start);
  base_transform_class->stop = GST_DEBUG_FUNCPTR(gst_myvideofilter_stop);
  video_filter_class->set_info = GST_DEBUG_FUNCPTR(gst_myvideofilter_set_info);
  video_filter_class->transform_frame = GST_DEBUG_FUNCPTR(gst_myvideofilter_transform_frame);
  video_filter_class->transform_frame_ip = GST_DEBUG_FUNCPTR(gst_myvideofilter_transform_frame_ip);
}

static void
gst_myvideofilter_init(GstMyVideoFilter *myvideofilter)
{
}

void gst_myvideofilter_set_property(GObject *object, guint property_id,
                                    const GValue *value, GParamSpec *pspec)
{
  GstMyVideoFilter *myvideofilter = GST_MYVIDEOFILTER(object);

  GST_DEBUG_OBJECT(myvideofilter, "set_property");

  switch (property_id)
  {
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    break;
  }
}

void gst_myvideofilter_get_property(GObject *object, guint property_id,
                                    GValue *value, GParamSpec *pspec)
{
  GstMyVideoFilter *myvideofilter = GST_MYVIDEOFILTER(object);

  GST_DEBUG_OBJECT(myvideofilter, "get_property");

  switch (property_id)
  {
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    break;
  }
}

void gst_myvideofilter_dispose(GObject *object)
{
  GstMyVideoFilter *myvideofilter = GST_MYVIDEOFILTER(object);

  GST_DEBUG_OBJECT(myvideofilter, "dispose");

  /* clean up as possible.  may be called multiple times */

  G_OBJECT_CLASS(gst_myvideofilter_parent_class)->dispose(object);
}

void gst_myvideofilter_finalize(GObject *object)
{
  GstMyVideoFilter *myvideofilter = GST_MYVIDEOFILTER(object);

  GST_DEBUG_OBJECT(myvideofilter, "finalize");

  /* clean up object here */

  G_OBJECT_CLASS(gst_myvideofilter_parent_class)->finalize(object);
}

static gboolean
gst_myvideofilter_start(GstBaseTransform *trans)
{
  GstMyVideoFilter *myvideofilter = GST_MYVIDEOFILTER(trans);

  GST_DEBUG_OBJECT(myvideofilter, "start");

  return TRUE;
}

static gboolean
gst_myvideofilter_stop(GstBaseTransform *trans)
{
  GstMyVideoFilter *myvideofilter = GST_MYVIDEOFILTER(trans);

  GST_DEBUG_OBJECT(myvideofilter, "stop");

  return TRUE;
}

static gboolean
gst_myvideofilter_set_info(GstVideoFilter *filter, GstCaps *incaps,
                           GstVideoInfo *in_info, GstCaps *outcaps, GstVideoInfo *out_info)
{
  // GstVideoConvert *space;
  GstMyVideoFilter *myvideofilter = GST_MYVIDEOFILTER(filter);

  GST_DEBUG_OBJECT(myvideofilter, "set_info");

  g_print("in_info %d x %d fmt: %s fps: %d\n",
          GST_VIDEO_INFO_WIDTH(in_info),
          GST_VIDEO_INFO_HEIGHT(in_info),
          GST_VIDEO_INFO_NAME(in_info),
          GST_VIDEO_INFO_FPS_N(in_info));

  g_print("out_info %d x %d\n", out_info->width, out_info->height);

  myvideofilter->convert = gst_video_converter_new(in_info, out_info,
                                                   gst_structure_new("GstVideoMyFilterConfig",
                                                                     //  GST_VIDEO_CONVERTER_OPT_DITHER_METHOD, GST_TYPE_VIDEO_DITHER_METHOD,
                                                                     // space->dither,
                                                                     //  GST_VIDEO_CONVERTER_OPT_DITHER_QUANTIZATION, G_TYPE_UINT,
                                                                     // space->dither_quantization,
                                                                     //  GST_VIDEO_CONVERTER_OPT_CHROMA_RESAMPLER_METHOD,
                                                                     // GST_TYPE_VIDEO_RESAMPLER_METHOD, space->chroma_resampler,
                                                                     //  GST_VIDEO_CONVERTER_OPT_ALPHA_MODE,
                                                                     // GST_TYPE_VIDEO_ALPHA_MODE, space->alpha_mode,
                                                                     //  GST_VIDEO_CONVERTER_OPT_ALPHA_VALUE,
                                                                     // G_TYPE_DOUBLE, space->alpha_value,
                                                                     //  GST_VIDEO_CONVERTER_OPT_CHROMA_MODE,
                                                                     // GST_TYPE_VIDEO_CHROMA_MODE, space->chroma_mode,
                                                                     //  GST_VIDEO_CONVERTER_OPT_MATRIX_MODE,
                                                                     // GST_TYPE_VIDEO_MATRIX_MODE, space->matrix_mode,
                                                                     //  GST_VIDEO_CONVERTER_OPT_GAMMA_MODE,
                                                                     // GST_TYPE_VIDEO_GAMMA_MODE, space->gamma_mode,
                                                                     //  GST_VIDEO_CONVERTER_OPT_PRIMARIES_MODE,
                                                                     // GST_TYPE_VIDEO_PRIMARIES_MODE, space->primaries_mode,
                                                                     //  GST_VIDEO_CONVERTER_OPT_THREADS, G_TYPE_UINT,
                                                                     NULL));

  return TRUE;
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
  
  int ty = image_width * y + x;
  *(image_data + ty) = color.y;
  
  int u = image_width * image_height * 5 / 4 + (y / 2 * image_width / 2 + x / 2);
  *(image_data + u) = color.u;
  int v = image_width * image_height + (y / 2 * image_width / 2 + x / 2);
  *(image_data + v) = color.v;
  
  // *(image_data + y * image_width + x) = color.y;
  // uint8_t *uv_offset = image_data + image_width * image_height + (y / 2) * image_width + x / 2 * 2;
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
static int tindex = 0;

/* transform */
static GstFlowReturn
gst_myvideofilter_transform_frame(GstVideoFilter *filter, GstVideoFrame *src,
                                  GstVideoFrame *dest)
{

  GstMyVideoFilter *myvideofilter = GST_MYVIDEOFILTER(filter);

  GST_DEBUG_OBJECT(myvideofilter, "transform_frame");

  gst_video_frame_copy(dest, src);

  GstMapInfo dest_map;
  guint8 *data;
  gsize size;

  gst_buffer_map(dest->buffer, &dest_map, GST_MAP_WRITE);
// https://color.d777.com/yuv-60.996_-30.071_125.415
  YUVColor color;
  color.u = 125;
  color.v = -30;
  color.y = 61;
  draw_rect(dest_map.data, 1280, 720, 20, 20, 1260, 700, color, 5);
  gst_buffer_unmap(dest->buffer, &dest_map);
  // void *file_data;
  
  // file_data = malloc(size);
  
  // memcpy(file_data, dest_map.data, size);
  
  // char file_name[64] = {0x0};
  
  // sprintf(file_name, "/home/haoshuai/code/gst/gst-template/data/bin_%d.yuv", tindex);
  
  // save_file(file_name, file_data, size);

  // free(file_data);
  // tindex++;

  return GST_FLOW_OK;
}

static GstFlowReturn
gst_myvideofilter_transform_frame_ip(GstVideoFilter *filter, GstVideoFrame *frame)
{
  GstMyVideoFilter *myvideofilter = GST_MYVIDEOFILTER(filter);

  GST_DEBUG_OBJECT(myvideofilter, "transform_frame_ip");

  return GST_FLOW_OK;
}

static gboolean
plugin_init(GstPlugin *plugin)
{

  /* FIXME Remember to set the rank if it's an element that is meant
     to be autoplugged by decodebin. */
  return gst_element_register(plugin, "myvideofilter", GST_RANK_NONE,
                              GST_TYPE_MYVIDEOFILTER);
}

/* FIXME: these are normally defined by the GStreamer build system.
   If you are creating an element to be included in gst-plugins-*,
   remove these, as they're always defined.  Otherwise, edit as
   appropriate for your external plugin package. */
#ifndef VERSION
#define VERSION "0.0.FIXME"
#endif
#ifndef PACKAGE
#define PACKAGE "FIXME_package"
#endif
#ifndef PACKAGE_NAME
#define PACKAGE_NAME "FIXME_package_name"
#endif
#ifndef GST_PACKAGE_ORIGIN
#define GST_PACKAGE_ORIGIN "http://FIXME.org/"
#endif

GST_PLUGIN_DEFINE(GST_VERSION_MAJOR,
                  GST_VERSION_MINOR,
                  myvideofilter,
                  "FIXME plugin description",
                  plugin_init, VERSION, "LGPL", PACKAGE_NAME, GST_PACKAGE_ORIGIN)
