
#include <gtk/gtk.h>
#include <genometools.h>

#ifndef GENOME_TOOLS_SUPPORT_H
#define GENOME_TOOLS_SUPPORT_H

cairo_surface_t*  create_cairo_surface_from_data(gdouble* mapData, gchar** geneNames, gint numberOfGenes, gint mapWidth, gfloat mapScale);

#endif