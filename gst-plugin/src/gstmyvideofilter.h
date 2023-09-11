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
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef _GST_MYVIDEOFILTER_H_
#define _GST_MYVIDEOFILTER_H_

#include <gst/video/video.h>
#include <gst/video/gstvideofilter.h>

// GObject
//  +----GInitiallyUnowned
//        +----GstObject
//              +----GstElement
//                    +----GstBaseTransform
//                          +----GstVideoFilter
//                                +----GstMyVideoFilter

// GObject
//  +----GInitiallyUnowned
//        +----GstObject
//              +----GstElement
//                    +----GstBaseTransform
//                          +----GstVideoFilter
//                                +----GstVideoConvert

G_BEGIN_DECLS

#define GST_TYPE_MYVIDEOFILTER   (gst_myvideofilter_get_type())
#define GST_MYVIDEOFILTER(obj)   (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_MYVIDEOFILTER,GstMyVideoFilter))
#define GST_MYVIDEOFILTER_CLASS(klass)   (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_MYVIDEOFILTER,GstMyVideoFilterClass))
#define GST_IS_MYVIDEOFILTER(obj)   (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_MYVIDEOFILTER))
#define GST_IS_MYVIDEOFILTER_CLASS(obj)   (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_MYVIDEOFILTER))

typedef struct _GstMyVideoFilter GstMyVideoFilter;
typedef struct _GstMyVideoFilterClass GstMyVideoFilterClass;

struct _GstMyVideoFilter
{
  GstVideoFilter base_myvideofilter;
  GstVideoConverter *convert;
};

struct _GstMyVideoFilterClass
{
  GstVideoFilterClass base_myvideofilter_class;
};

GType gst_myvideofilter_get_type (void);

G_END_DECLS

#endif
