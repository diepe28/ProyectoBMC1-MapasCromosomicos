#include <gtk/gtk.h>
#include "Algoritmo.h"
#include "GenomeToolsSupport.h"

GtkBuilder *builder = NULL;

void window_init(GtkBuilder *sender) {
	// Storing a reference to gtk_builder. So every callback function has access to it.
	builder = g_object_ref (sender);
	GtkWidget *gridview = GTK_WIDGET(gtk_builder_get_object(sender, "gridview"));
	gridview_init(gridview, 3);
}
/* ---------------------------------------------------------------- */
void spinbutton_valuechanged(GtkSpinButton *sender, gpointer args) {
	GtkWidget *gridview = (GtkWidget*) args;

	int value = gtk_spin_button_get_value_as_int(sender);

	gridview_set_dimensions(gridview, value);

	g_critical("spinbutton value changed!");
}
/* ---------------------------------------------------------------- */
void gridview_row_activated(GtkTreeView *sender, GtkTreePath *path, GtkTreeViewColumn *column, gpointer args) {
	

	g_critical("row activated, path: %s", gtk_tree_path_to_string(path));
}
/* ---------------------------------------------------------------- */

gint totalNumberOfMaps = 0;
gint currentMap = 0;
gfloat currentImageScale = 1.0;

static void update_map_nav(gint currentMap, gint totalMaps) 
{
	GtkLabel* map_navigation_label = GTK_LABEL(gtk_builder_get_object(builder, "mapnav"));
	gchar label_text[8];
	g_snprintf (label_text, sizeof(label_text), "%d/%d", currentMap, totalMaps);
	gtk_label_set_text (map_navigation_label, label_text);
}

void btmap_clicked(GtkButton *sender) {

	// TODO: Simulating call to algorithm. Replace with the proper code.
		numMaps = 2;
	
		gchar* geneNames[3];
	    geneNames[0] = g_malloc(sizeof(gchar) * 10);
		geneNames[1] = g_malloc(sizeof(gchar) * 10);
		geneNames[2] = g_malloc(sizeof(gchar) * 10);
		g_stpcpy(geneNames[0], "Gen 1");
		g_stpcpy(geneNames[1], "Gen 2");
		g_stpcpy(geneNames[2], "Gen 3");

		gdouble* maps[2];
		maps[0] = g_malloc(sizeof(gdouble) * 3);
		maps[1] = g_malloc(sizeof(gdouble) * 3);
		maps[0][0] = 0.0;
		maps[0][1] = 30.0;
		maps[0][2] = 40.0;
		maps[1][0] = 30.0;
		maps[1][1] = 0.0;
		maps[1][2] = 40.0;
		
		/*gdouble maps[2][3] = {
			{0.0, 30.0, 40.0},
			{30.0, 0.0, 40.0}
		};*/
		mapList = maps;
	//
	
	GtkSpinButton* spin_button = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spinbutton"));
	gdouble number_of_genes = gtk_spin_button_get_value(spin_button);

	GtkWidget* prevButton = GTK_WIDGET(gtk_builder_get_object(builder, "tbprev"));
	GtkWidget* nextButton = GTK_WIDGET(gtk_builder_get_object(builder, "tbnext"));
	
	if (numMaps > 0) {
		GtkImage* image = GTK_IMAGE(gtk_builder_get_object (builder, "image"));
		GtkWidget* viewport = GTK_WIDGET(gtk_builder_get_object (builder, "viewport"));
		gulong allocated_width_for_viewport = gtk_widget_get_allocated_width (viewport);
		
		currentImageScale = 1.0;
		// cairo_surface_t* cairo_surface = create_cairo_surface_from_data (mapList[0], number_of_genes, allocated_width_for_viewport, currentImageScale);
		cairo_surface_t* cairo_surface = create_cairo_surface_from_data (mapList[0], geneNames, number_of_genes, allocated_width_for_viewport, currentImageScale);
		gtk_image_set_from_surface (image, cairo_surface);
		cairo_surface_destroy (cairo_surface);
		update_map_nav (1, numMaps);
	}
	else {
		update_map_nav(0, 0);
		gtk_widget_set_sensitive (nextButton, FALSE);
	}
	
	gtk_widget_set_sensitive (prevButton, FALSE);

	/* Freeing test memory */
	//TODO remove later
	g_free(geneNames[0]);
	g_free(geneNames[1]);
	g_free(geneNames[2]);
	g_free(maps[0]);
	g_free(maps[1]);
	
}
/* ---------------------------------------------------------------- */
void btcalc_clicked(GtkButton *sender) {
	g_critical("btcalc clicked!");
}
/* ---------------------------------------------------------------- */
void btprev_clicked(GtkButton *sender) {
	g_critical("btprev clicked!");
}
/* ---------------------------------------------------------------- */
void btnext_clicked(GtkButton *sender) {
	g_critical("btnext clicked!");
}
/* ---------------------------------------------------------------- */
void btzoomin_clicked(GtkButton *sender) {
	g_critical("btzoomin clicked!");
}
/* ---------------------------------------------------------------- */
void btfit_clicked(GtkButton *sender) {
	g_critical("btfit clicked!");
}
/* ---------------------------------------------------------------- */
void btzoomout_clicked(GtkButton *sender) {
	g_critical("btzoomout clicked!");
}
/* ---------------------------------------------------------------- */
