#include <gtk/gtk.h>
#include "mapascromosomicos.h"
#include "Algoritmo.h"
#include "GenomeToolsSupport.h"

// Extern variables from Algoritmo.h
gint numMaps;
gdouble** mapList;

// Extern variables from mapascromosomicos.h
GApplication* application;

// Global variables
GtkBuilder* global_builder = NULL;
gchar** global_currentGeneNames = NULL;
gint global_currentNumberOfGenes = 0;
gint global_currentMap = 0;
gfloat global_currentImageScale = 1.0;

gint global_current_log_entry = 0;

static void append_to_log(gchar *text)
{
	GtkTextBuffer *tbuffer;
	GtkTextIter ei;
	GtkTextView *textview = GTK_TEXT_VIEW(gtk_builder_get_object(global_builder, "trace"));
	gchar text_to_append[150];

	global_current_log_entry++;
	g_snprintf (text_to_append, sizeof(text_to_append), "%d --- %s", global_current_log_entry, text);

	tbuffer = gtk_text_view_get_buffer(textview);
	gtk_text_buffer_get_end_iter(tbuffer, &ei);
	gtk_text_buffer_insert(tbuffer, &ei, text_to_append, -1);
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

	GtkWindow* mainWindow = GTK_WINDOW(gtk_builder_get_object (global_builder, "window"));
	gtk_window_set_icon_from_file (mainWindow, "src/icon_window.png", NULL);
	gtk_window_set_default_icon_from_file ("src/icon_window.png", NULL);
	
	GtkWidget *gridview = GTK_WIDGET(gtk_builder_get_object(sender, "gridview"));
	gridview_init(gridview, 3);
	update_map_nav();
	change_zoom_controls(FALSE);

	append_to_log("¡Bienvenidos al graficador de mapas cromosómicos!\n");
	append_to_log("Introduzca las probabilidades de recombinación entre un grupo de genes y presione el botón 'Mapear'.\n");
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
	gint i, j = 0;
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

	GtkSpinButton* spin_button = GTK_SPIN_BUTTON(gtk_builder_get_object(global_builder, "spinbutton"));
	gint numberOfGenes = gtk_spin_button_get_value_as_int (spin_button);

	// TODO: Simulating gene names and grid data
	gchar** geneNames;
	geneNames = g_malloc(sizeof(gchar*) * 3);
	geneNames[0] = g_malloc(sizeof(gchar) * 10);
	geneNames[1] = g_malloc(sizeof(gchar) * 10);
	geneNames[2] = g_malloc(sizeof(gchar) * 10);
	g_stpcpy(geneNames[0], "Gen 1");
	g_stpcpy(geneNames[1], "Gen 2");
	g_stpcpy(geneNames[2], "Gen 3");

	gdouble** data = g_malloc(sizeof(gdouble*) * 3);
	data[0] = g_malloc(sizeof(gdouble) * 3);
	data[1] = g_malloc(sizeof(gdouble) * 3);
	data[2] = g_malloc(sizeof(gdouble) * 3);
	data[0][0] = 0.0; data[0][1] = 0.4; data[0][2] = -1;
	data[1][0] = 0.4; data[1][1] = 0.0; data[1][2] = 0.2;
	data[2][0] = -1; data[2][1] = 0.2; data[2][2] = 0.0;
	// End simulation

	// Call algorithm to populate numMaps and mapList extern variables
	createMaps(data, numberOfGenes);

	// Convert percentages to cM
	for (i = 0; i < numMaps; i++)
		for (j = 0; j < numberOfGenes; j++)
			mapList[i][j] *= 100;

	// Clean input data memory
	for (i = 0; i < numberOfGenes; i++) 
		g_free(data[i]);
	g_free(data);
	
	if (numMaps > 0) {
		global_currentGeneNames = geneNames;
		global_currentImageScale = 1.0;
		global_currentNumberOfGenes = numberOfGenes;
		global_currentMap = 1;
		render_current_map();
		change_zoom_controls(TRUE);
		append_to_log("Mapas generados correctamente.\n");
	}
	else {
		append_to_log("Los datos introducidos son inconsistentes o están incompletos.\n");
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

void
on_menuitem_exit (GtkMenuItem *menuitem,
               gpointer     user_data) 
{
	GtkWindow* mainWindow = GTK_WINDOW(gtk_builder_get_object(global_builder, "window"));
	GtkApplication* application = gtk_window_get_application (mainWindow);
	g_application_quit (G_APPLICATION(application));
}

void
on_helpmenuitem_activate (GtkMenuItem *menuitem,
               gpointer     user_data) 
{
	GtkAboutDialog *dialog = GTK_ABOUT_DIALOG(gtk_about_dialog_new());
	gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), "Graficador de Mapas Cromosómicos");
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), 
	                              "Proyecto #1 del curso: Introducción a la Biología Molecular Computacional.");

	GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file("src/icon.png", NULL);
	gtk_about_dialog_set_logo(dialog, pixbuf);
    g_object_unref(pixbuf);
	pixbuf = NULL; 
	
	gchar **authors = g_malloc(sizeof(gchar*) * 4);
	authors[0] = g_malloc(sizeof(gchar) * 30);
	authors[1] = g_malloc(sizeof(gchar) * 30);
	authors[2] = g_malloc(sizeof(gchar) * 30);
	authors[3] = NULL;
	g_stpcpy(authors[0], "Olger Calderón Achío");
	g_stpcpy(authors[1], "Wilberth Castro Fuentes");
	g_stpcpy(authors[2], "Diego Pérez Arroyo");
	gtk_about_dialog_set_authors(dialog, authors);

	gtk_dialog_run(GTK_DIALOG (dialog));
	g_free(authors[0]);
	g_free(authors[1]);
	g_free(authors[2]);
	g_free(authors[3]);
	g_free(authors);
	gtk_widget_destroy(GTK_WIDGET(dialog));
}

