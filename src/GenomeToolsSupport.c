
#include "GenomeToolsSupport.h"

const gint GENE_LENGTH = 3;

static gdouble get_upper_bound(gdouble* mapData, gint numberOfGenes) 
{
	gint i = 0;
	gdouble max = 0.0;
	for (i = 0; i < numberOfGenes; i++)
		if (mapData[i] > max)
			max = mapData[i];
	return max;
}

static void handle_error(GtError *err)
{
  fprintf(stderr, "error writing canvas %s\n", gt_error_get(err));
  exit(EXIT_FAILURE);
}

static GtArray* create_array_of_features(gdouble* mapData, gint numberOfGenes, gdouble upperBound)
{
	GtArray *features;
	GtGenomeNode *gene, *chromosome;
	GtStr *seqid;
	gint i = 0;

	features = gt_array_new(sizeof (GtGenomeNode*));
	seqid = gt_str_new_cstr("linkage_map");
	
	chromosome = gt_feature_node_new(seqid, "chromosome", 0, upperBound, GT_STRAND_FORWARD);
	gt_feature_node_set_attribute((GtFeatureNode*)chromosome, "ID", "Chr1");

	for (i = 0; i < numberOfGenes; i++) {
		gene = gt_feature_node_new(seqid, "gene", mapData[i], mapData[i] + GENE_LENGTH, GT_STRAND_FORWARD);
		//TODO Use gene name gt_feature_node_set_attribute((GtFeatureNode*)gene, "ID", "Gene 1");
		gt_feature_node_add_child((GtFeatureNode*) chromosome, (GtFeatureNode*) gene);
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

cairo_surface_t* create_cairo_surface_from_data(gdouble* mapData, gint numberOfGenes, gint mapWidth, gfloat mapScale)
{
	gdouble upperBound = get_upper_bound(mapData, numberOfGenes); 
	GtRange range = { 0, upperBound};
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
	GtArray* features = create_array_of_features (mapData, numberOfGenes, upperBound);
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

	canvas = gt_canvas_cairo_context_new(style, cairo_context, 0, mapWidth,
	                                     height, NULL, err);
	
	gt_assert(canvas);
	gt_layout_sketch(layout, canvas, err);

	cairo_destroy (cairo_context);
	// cairo_surface_destroy (surface);
	gt_canvas_delete(canvas);
	gt_layout_delete(layout);
	gt_diagram_delete(diagram);
	gt_style_delete(style);
	gt_error_delete(err);
	delete_example_features(features);
	
	return surface;
}