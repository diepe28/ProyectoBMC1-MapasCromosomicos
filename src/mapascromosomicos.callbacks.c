#include <gtk/gtk.h>
#include "Algoritmo.h"
#include "GenomeToolsSupport.h"

// Extern variables from Algoritmo.h
gint numMaps;
gdouble** mapList;

// Global variables
GtkBuilder* global_builder = NULL;
gchar** global_currentGeneNames = NULL;
gint global_currentNumberOfGenes = 0;
gint global_currentMap = 0;
gfloat global_currentImageScale = 1.0;

static void append_to_log(gchar *text)
{
	GtkTextBuffer *tbuffer;
	GtkTextIter ei;
	GtkTextView *textview = GTK_TEXT_VIEW(gtk_builder_get_object(global_builder, "trace"));

	tbuffer = gtk_text_view_get_buffer(textview);
	gtk_text_buffer_get_end_iter(tbuffer, &ei);
	gtk_text_buffer_insert(tbuffer, &ei, text, -1);
}

// Private helper functions
static void update_map_nav() 
{
	GtkWidget* prevButton = GTK_WIDGET(gtk_builder_get_object(global_builder, "tbprev"));
	GtkWidget* nextButton = GTK_WIDGET(gtk_builder_get_object(global_builder, "tbnext"));
	
	GtkLabel* map_navigation_label = GTK_LABEL(gtk_builder_get_object(global_builder, "mapnav"));
	gchar label_text[8];
	g_snprintf (label_text, sizeof(label_text), "%d/%d", global_currentMap, numMaps);
	gtk_label_set_text (map_navigation_label, label_text);

	if (global_currentMap == 0 || global_currentMap == 1)
		gtk_widget_set_sensitive (prevButton, FALSE);
	else
		gtk_widget_set_sensitive (prevButton, TRUE);

	if (global_currentMap == numMaps)
		gtk_widget_set_sensitive (nextButton, FALSE);
	else
		gtk_widget_set_sensitive (nextButton, TRUE);
}

static void change_zoom_controls(gboolean enable) 
{
	GtkWidget* zoom_in_button = GTK_WIDGET(gtk_builder_get_object(global_builder, "tbzoomin"));
	GtkWidget* zoom_out_button = GTK_WIDGET(gtk_builder_get_object(global_builder, "tbzoomout"));
	GtkWidget* fit_button = GTK_WIDGET(gtk_builder_get_object(global_builder, "btfit"));
	gtk_widget_set_sensitive (zoom_in_button, enable);
	gtk_widget_set_sensitive (zoom_out_button, FALSE);
	gtk_widget_set_sensitive (fit_button, enable);
}

static void render_current_map() 
{
	GtkImage* image = GTK_IMAGE(gtk_builder_get_object (global_builder, "image"));
	GtkWidget* viewport = GTK_WIDGET(gtk_builder_get_object (global_builder, "viewport"));
	gulong allocated_width_for_viewport = gtk_widget_get_allocated_width (viewport);
	cairo_surface_t* cairo_surface = 
		create_cairo_surface_from_data (mapList[global_currentMap-1], 
		                                global_currentGeneNames, 
		                                global_currentNumberOfGenes, 
		                                allocated_width_for_viewport, 
		                                global_currentImageScale);
	gtk_image_set_from_surface (image, cairo_surface);
	cairo_surface_destroy (cairo_surface);
}

// Callbacks
void window_init(GtkBuilder *sender) {
	// Storing a reference to gtk_builder. So every callback function has access to it.
	global_builder = g_object_ref (sender);
	
	GtkWidget *gridview = GTK_WIDGET(gtk_builder_get_object(sender, "gridview"));
	gridview_init(gridview, 3);
	update_map_nav();
	change_zoom_controls(FALSE);

	append_to_log("¡Bienvenidos al graficador de mapas cromosómicos!\n");
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

void btmap_clicked(GtkButton *sender) {
	
	// Freeing memory and reseting current data.
	gint i = 0;
	if (global_currentGeneNames != NULL && mapList != NULL) {
		for (i = 0; i < global_currentNumberOfGenes; i++) 
			g_free(global_currentGeneNames[i]);
		g_free(global_currentGeneNames);

		for (i = 0; i < numMaps; i++)
			g_free(mapList[i]);
		g_free(mapList);
	}
	global_currentGeneNames = NULL;
	global_currentNumberOfGenes = 0;
	global_currentMap = 0;
	global_currentImageScale = 1.0;
	mapList = NULL;
	numMaps = 0;
	
	
	// TODO: Extracting genes names and simulating call to algorithm. Replace with the proper code.
		gchar** geneNames;
		geneNames = g_malloc(sizeof(gchar*) * 3);
	    geneNames[0] = g_malloc(sizeof(gchar) * 10);
		geneNames[1] = g_malloc(sizeof(gchar) * 10);
		geneNames[2] = g_malloc(sizeof(gchar) * 10);
		g_stpcpy(geneNames[0], "Gen 1");
		g_stpcpy(geneNames[1], "Gen 2");
		g_stpcpy(geneNames[2], "Gen 3");

		gdouble** maps;
		maps = g_malloc(sizeof(gdouble*) * 2);
		maps[0] = g_malloc(sizeof(gdouble) * 3);
		maps[1] = g_malloc(sizeof(gdouble) * 3);
		maps[0][0] = 0.0;
		maps[0][1] = 5.0;
		maps[0][2] = 50.0;
		maps[1][0] = 10.0;
		maps[1][1] = 0.0;
		maps[1][2] = 20.0;

		// These variables are setted by algorithm
		mapList = maps;
		numMaps = 2;
	//		
	
	if (numMaps > 0) {
		GtkSpinButton* spin_button = GTK_SPIN_BUTTON(gtk_builder_get_object(global_builder, "spinbutton"));
		global_currentGeneNames = geneNames;
		global_currentImageScale = 1.0;
		global_currentNumberOfGenes = gtk_spin_button_get_value(spin_button);
		global_currentMap = 1;
		render_current_map();
		change_zoom_controls(TRUE);
		append_to_log("Mapas graficados correctamente.\n");
	}
	else {
		change_zoom_controls(FALSE); 
	}
	update_map_nav();
}

/* ---------------------------------------------------------------- */
void btcalc_clicked(GtkButton *sender) {
	g_critical("btcalc clicked!");
}
/* ---------------------------------------------------------------- */
void btprev_clicked(GtkButton *sender) {
	global_currentMap--;
	render_current_map();
	update_map_nav ();
}
/* ---------------------------------------------------------------- */
void btnext_clicked(GtkButton *sender) {
	global_currentMap++;
	render_current_map();
	update_map_nav ();
}
/* ---------------------------------------------------------------- */
void btzoomin_clicked(GtkButton *sender) {
	global_currentImageScale += 0.1;
	render_current_map ();
	GtkWidget* zoom_in_button = GTK_WIDGET(gtk_builder_get_object(global_builder, "tbzoomin"));
	GtkWidget* zoom_out_button = GTK_WIDGET(gtk_builder_get_object(global_builder, "tbzoomout"));
	if (global_currentImageScale >= 2.0) 
		gtk_widget_set_sensitive (zoom_in_button, FALSE);
	gtk_widget_set_sensitive (zoom_out_button, TRUE);
}
/* ---------------------------------------------------------------- */
void btfit_clicked(GtkButton *sender) {
	GtkWidget* zoom_in_button = GTK_WIDGET(gtk_builder_get_object(global_builder, "tbzoomin"));
	GtkWidget* zoom_out_button = GTK_WIDGET(gtk_builder_get_object(global_builder, "tbzoomout"));
	global_currentImageScale = 1.0;
	render_current_map ();
	gtk_widget_set_sensitive (zoom_out_button, FALSE);
	gtk_widget_set_sensitive (zoom_in_button, TRUE);
}
/* ---------------------------------------------------------------- */
void btzoomout_clicked(GtkButton *sender) {
	global_currentImageScale -= 0.1;
	render_current_map ();
	GtkWidget* zoom_in_button = GTK_WIDGET(gtk_builder_get_object(global_builder, "tbzoomin"));
	GtkWidget* zoom_out_button = GTK_WIDGET(gtk_builder_get_object(global_builder, "tbzoomout"));
	if (global_currentImageScale == 1.0) 
		gtk_widget_set_sensitive (zoom_out_button, FALSE);
	gtk_widget_set_sensitive (zoom_in_button, TRUE);
}
/* ---------------------------------------------------------------- */

void on_trace_size_allocate (GtkWidget    *widget,
               GdkRectangle *allocation,
               gpointer      user_data) 
{
	GtkScrolledWindow *scrolledWindow = GTK_SCROLLED_WINDOW(gtk_builder_get_object(global_builder, "scrolls"));
	GtkAdjustment* adjustment = gtk_scrolled_window_get_vadjustment (scrolledWindow);
	gtk_adjustment_set_value (adjustment, gtk_adjustment_get_upper (adjustment));
}

