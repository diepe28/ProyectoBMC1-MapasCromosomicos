
#include "GenomeToolsSupport.h"

static gdouble get_upper_bound(gdouble* mapData, gint numberOfGenes, gint* index) 
{
	gint i = 0;
	gdouble max = 0.0;
	for (i = 0; i < numberOfGenes; i++)
		if (mapData[i] > max) {
			*index = i;
			max = mapData[i];
		}
	return max;
}

static void handle_error(GtError *err)
{
  fprintf(stderr, "error writing canvas %s\n", gt_error_get(err));
  exit(EXIT_FAILURE);
}

static GtArray* create_array_of_features(gdouble* mapData, gchar** geneNames, gint numberOfGenes, gdouble upperBound, gint geneLength)
{
	// TODO Quitar cromosoma, e ignorar genes con banderas
	GtArray *features;
	GtGenomeNode *gene, *chromosome;
	GtStr *seqid;
	gint i = 0;

	features = gt_array_new(sizeof (GtGenomeNode*));
	seqid = gt_str_new_cstr("linkage_map");
	
	chromosome = gt_feature_node_new(seqid, "chromosomes", 0, upperBound + geneLength, GT_STRAND_FORWARD);
	gt_feature_node_set_attribute((GtFeatureNode*)chromosome, "Name", "Chr1");

	for (i = 0; i < numberOfGenes; i++) {
		gene = gt_feature_node_new(seqid, "genes", mapData[i], mapData[i] + geneLength, GT_STRAND_FORWARD);
		gt_feature_node_set_attribute((GtFeatureNode*)gene, "Name", geneNames[i]);
		gt_array_add(features, gene);
	}
	
	gt_array_add(features, chromosome);
	gt_str_delete(seqid);
	return features;
}

static void delete_example_features(GtArray *features)
{
  gint i;
  for (i = 0; i < gt_array_size(features); i++)
    gt_genome_node_delete(*(GtGenomeNode**) gt_array_get(features, i));
  gt_array_delete(features);
}

static gint getBestWidth(gdouble upperBound, gfloat mapScale) 
{
	gint geneWidth = (upperBound * 2) / 40;
	if (geneWidth < 1)
		geneWidth = 1;
	if (mapScale > 1.3)
		if (mapScale < 1.6)
			geneWidth *= 0.8;
		else
			geneWidth *= 0.4;
	return geneWidth;
}

cairo_surface_t* create_cairo_surface_from_data(gdouble* mapData, gchar** geneNames, gint numberOfGenes, gint mapWidth, gfloat mapScale)
{
	// Code for adjusting gene widths and optimal diagram configuration.
	gint gene_index_with_upper = 0;
	gdouble upperBound = get_upper_bound(mapData, numberOfGenes, &gene_index_with_upper);
	gint geneWidth = 0;

	if (upperBound <= 10)
		geneWidth = 0;
	else
		geneWidth = getBestWidth(upperBound, mapScale);

	GtStr* gtString = gt_str_new_cstr(geneNames[gene_index_with_upper]);
	gulong stringLength = gt_str_length(gtString);
	gint extension = stringLength / 3;
	if (extension < 1)
		extension = 1;
	gt_str_delete(gtString);
	
	GtRange range = { 0, upperBound + (geneWidth * extension)};
	GtStyle *style;
	GtDiagram *diagram;
	GtLayout *layout;
	GtError *err = gt_error_new();
	GtCanvas *canvas = NULL;

	gulong height;
	gint rval;

	char* style_file = "src/default.style";

	/* create style */
	if (!(style = gt_style_new(err)))
		handle_error(err);

	/* load style file */
	if (gt_style_load_file(style, style_file, err))
		handle_error(err);

	/* create diagram */
	GtArray* features = create_array_of_features (mapData, geneNames, numberOfGenes, upperBound, geneWidth);
	diagram = gt_diagram_new_from_array(features, &range, style);

	if (!diagram || mapWidth <= 30) return FALSE;

	/* render image */
	layout = gt_layout_new(diagram, mapWidth * mapScale, style, err);
	if (!layout) return FALSE;
	rval = gt_layout_get_height(layout, &height, err);
	gt_assert(rval == 0);
	mapWidth *= mapScale; 
	height *= mapScale;
	
	/* cairo code */
	cairo_surface_t* surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, mapWidth * mapScale, height * mapScale);
	cairo_t* cairo_context = cairo_create (surface);
	cairo_scale (cairo_context, mapScale, mapScale);
	cairo_rectangle(cairo_context, 0, 0, mapWidth, height);
	cairo_clip(cairo_context);

	canvas = gt_canvas_cairo_context_new(style, cairo_context, 0, mapWidth,
	                                     height, NULL, err);
	
	gt_assert(canvas);
	gt_layout_sketch(layout, canvas, err);

	cairo_destroy (cairo_context);
	gt_canvas_delete(canvas);
	gt_layout_delete(layout);
	gt_diagram_delete(diagram);
	gt_style_delete(style);
	gt_error_delete(err);
	delete_example_features(features);
	
	return surface;
}