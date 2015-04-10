
#include <gtk/gtk.h>
#include <genometools.h>

#ifndef GENOME_TOOLS_SUPPORT_H
#define GENOME_TOOLS_SUPPORT_H

cairo_t*  createCairoSurfaceFromData(gdouble** data, gint width, gint height, gfloat scale);

#endif