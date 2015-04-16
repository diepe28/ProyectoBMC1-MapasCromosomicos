#include <gtk/gtk.h>
#include "mapascromosomicos.h"
#include "Algoritmo.h"
#include "GenomeToolsSupport.h"


#ifndef CALLBACKS
#define CALLBACKS

// New variables
gdouble*** groupsData;
gint* numberOfMapsPerGroup;
gint numberOfGroups;
gint global_currentGroup = 0;

// Extern variables from Algoritmo.h
gint numMaps;
gdouble** mapList;

// Global variables
GtkBuilder* global_builder = NULL;
gchar** global_currentGeneNames = NULL;
gint global_currentNumberOfGenes = 0;
gint global_currentMap = 0;
gfloat global_currentImageScale = 1.0;

gint global_current_log_entry = 0;

static void createDummyDataFor3Genes();
/* ---------------------------------------------------------------- */
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
/* ---------------------------------------------------------------- */
// Private helper functions
static void update_map_nav() 
{
	GtkWidget* prevButton = GTK_WIDGET(gtk_builder_get_object(global_builder, "tbprev"));
	GtkWidget* nextButton = GTK_WIDGET(gtk_builder_get_object(global_builder, "tbnext"));
	
	GtkLabel* map_navigation_label = GTK_LABEL(gtk_builder_get_object(global_builder, "mapnav"));
	gchar label_text[15];
	g_snprintf (label_text, sizeof(label_text), "Mapa %d/%d", global_currentMap, numMaps);
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

static void update_group_nav() 
{
	GtkWidget* prevButton = GTK_WIDGET(gtk_builder_get_object(global_builder, "tbprevGrupo"));
	GtkWidget* nextButton = GTK_WIDGET(gtk_builder_get_object(global_builder, "tbnextGrupo"));
	
	GtkLabel* group_navigation_label = GTK_LABEL(gtk_builder_get_object(global_builder, "gruposnav"));
	gchar label_text[30];
	g_snprintf (label_text, sizeof(label_text), "Grupo de Ligamiento %d/%d", global_currentGroup, numberOfGroups);
	gtk_label_set_text (group_navigation_label, label_text);

	if (global_currentGroup == 0 || global_currentGroup == 1)
		gtk_widget_set_sensitive (prevButton, FALSE);
	else
		gtk_widget_set_sensitive (prevButton, TRUE);

	if (global_currentGroup == numberOfGroups)
		gtk_widget_set_sensitive (nextButton, FALSE);
	else
		gtk_widget_set_sensitive (nextButton, TRUE);
}

/* ---------------------------------------------------------------- */
static void change_zoom_controls(gboolean enable) 
{
	GtkWidget* zoom_in_button = GTK_WIDGET(gtk_builder_get_object(global_builder, "tbzoomin"));
	GtkWidget* zoom_out_button = GTK_WIDGET(gtk_builder_get_object(global_builder, "tbzoomout"));
	GtkWidget* fit_button = GTK_WIDGET(gtk_builder_get_object(global_builder, "btfit"));
	gtk_widget_set_sensitive (zoom_in_button, enable);
	gtk_widget_set_sensitive (zoom_out_button, FALSE);
	gtk_widget_set_sensitive (fit_button, enable);
}
/* ---------------------------------------------------------------- */
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
/* ---------------------------------------------------------------- */
// Function for retrieving data from grid.
void static getDataFromGrid(gdouble** data, gchar** geneNames, gint numberOfGenes) 
{
	GtkTreeIter iter;
	gboolean valid = FALSE;
	gint row_count = 0;
	gint i, j = 0;
	GtkTreeView* grid = GTK_TREE_VIEW(gtk_builder_get_object(global_builder, "gridview"));
	GtkTreeModel* list_store = gtk_tree_view_get_model (grid);
	gchar *str_data;
	
	valid = gtk_tree_model_get_iter_first (list_store,
	                                       &iter);
	if (valid)
	{
		for (i = 0; i < numberOfGenes && valid; i++) {

			gtk_tree_model_get (list_store, &iter,
			                    0, &geneNames[i],
			                    -1);

			for (j = 0; j < numberOfGenes && valid; j++) {
				gtk_tree_model_get (list_store, &iter,
			                    j+1, &str_data,
			                    -1);

				if (str_data[0] == '\0')
					data[i][j] = -1;
				else {
					gdouble result = g_ascii_strtod(str_data, NULL);
					data[i][j] = result;
				}
				g_free(str_data);
			}

			valid = gtk_tree_model_iter_next (list_store,
			                                  &iter);
		}

	}
}

void static clean_and_reset_data() 
{
	gint i, j, k = 0;
	if (global_currentGeneNames != NULL && groupsData != NULL) {
		for (i = 0; i < global_currentNumberOfGenes; i++) 
			g_free(global_currentGeneNames[i]);
		g_free(global_currentGeneNames);

		for (i = 0; i < numberOfGroups; i++) {
			for (j = 0; j < numberOfMapsPerGroup[i]; j++)
					g_free(groupsData[i][j]);
			g_free(groupsData[i]);
		}
		g_free(groupsData);
		g_free(numberOfMapsPerGroup);
	}
	global_currentGeneNames = NULL;
	global_currentNumberOfGenes = 0;
	global_currentMap = 0;
	global_currentImageScale = 1.0;
	mapList = NULL;
	numMaps = 0;
	groupsData = NULL;
	numberOfMapsPerGroup = NULL;
	numberOfGroups = 0;
	global_currentGroup = 0;
}

// Callbacks
/* ---------------------------------------------------------------- */
void window_init(GtkBuilder *sender) {
	// Storing a reference to gtk_builder. So every callback function has access to it.
	global_builder = g_object_ref (sender);

	GtkWindow* mainWindow = GTK_WINDOW(gtk_builder_get_object (global_builder, "window"));
	gtk_window_set_icon_from_file (mainWindow, "src/icon_window.png", NULL);
	gtk_window_set_default_icon_from_file ("src/icon_window.png", NULL);
	
	GtkWidget *gridview = GTK_WIDGET(gtk_builder_get_object(sender, "gridview"));
	gridview_init(gridview, 3);
	update_map_nav();
	update_group_nav();
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

void btmap_clicked(GtkButton *sender) {
	// Freeing memory and reseting current data.
	gint i, j, k = 0;
	clean_and_reset_data();

	GtkSpinButton* spin_button = GTK_SPIN_BUTTON(gtk_builder_get_object(global_builder, "spinbutton"));
	gint numberOfGenes = gtk_spin_button_get_value_as_int (spin_button);

	// Retrieving data from grid.
	gdouble** data = g_malloc(sizeof(gdouble*) * numberOfGenes);
	gchar** geneNames = g_malloc(sizeof(gchar*) * numberOfGenes);
	for (i = 0; i < numberOfGenes; i++) {
		data[i] = g_malloc(sizeof(gdouble) * numberOfGenes);
		geneNames[i] = g_malloc(sizeof(gchar) * 50);
	}

	// Populate data y geneNames
	getDataFromGrid(data, geneNames, numberOfGenes);

	// Call algorithm. Populates groupsData, numberOfMapsPerGroup, numberOfGroups;
	
	createDummyDataFor3Genes ();
	//createMaps(data, numberOfGenes);

	// Convert percentages to cM
	for (i = 0; i < numberOfGroups; i++)
		for (j = 0; j < numberOfMapsPerGroup[i]; j++)
			for (k = 0; k < numberOfGenes; k++)
				groupsData[i][j][k] *= 100;

	// Clean input data memory
	for (i = 0; i < numberOfGenes; i++) 
		g_free(data[i]);
	g_free(data);
	
	if (numberOfGroups > 0) {
		numMaps = numberOfMapsPerGroup[0];
		mapList = groupsData[0];
		global_currentGroup = 1;
		global_currentGeneNames = geneNames;
		global_currentImageScale = 1.0;
		global_currentNumberOfGenes = numberOfGenes;
		global_currentMap = 1;
		render_current_map();
		change_zoom_controls(TRUE);
		append_to_log("Mapas generados correctamente.\n");
	}
	else {
		// Clear unstored geneNames
		for (i = 0; i < numberOfGenes; i++) 
			g_free(geneNames[i]);
		g_free(geneNames);
		
		global_currentGeneNames = NULL;
		append_to_log("Los datos introducidos son inconsistentes o están incompletos.\n");
		change_zoom_controls(FALSE);
	}
	update_map_nav();
	update_group_nav ();
}
/* ---------------------------------------------------------------- */
void btcalc_clicked(GtkButton *sender) {
	// Freeing memory and reseting current data.
	gint i, j = 0;

	GtkSpinButton* spin_button = GTK_SPIN_BUTTON(gtk_builder_get_object(global_builder, "spinbutton"));
	gint numberOfGenes = gtk_spin_button_get_value_as_int (spin_button);

	// Retrieving data from grid.
	gdouble** data = g_malloc(sizeof(gdouble*) * numberOfGenes);
	gchar** geneNames = g_malloc(sizeof(gchar*) * numberOfGenes);
	for (i = 0; i < numberOfGenes; i++) {
		data[i] = g_malloc(sizeof(gdouble) * numberOfGenes);
		geneNames[i] = g_malloc(sizeof(gchar) * 50);
	}

	// Populate data y geneNames
	getDataFromGrid(data, geneNames, numberOfGenes);

	gint probsPredicted = predict(data, numberOfGenes);
	append_to_log(resumeStr);
	
	if(probsPredicted > 0)
	{
		GtkTreeView* grid = GTK_TREE_VIEW(gtk_builder_get_object(global_builder, "gridview"));
		GtkTreeModel* model = gtk_tree_view_get_model (grid);
		char valueStr[10];
		
		//just upper right triangle of matriz
		for(i = 0; i < numberOfGenes-1; i++){
			for(j = i+1; j < numberOfGenes; j++){
				sprintf(valueStr, "%0.5f",data[i][j]);
				valueStr[1] = '.';
				gridview_model_set_value(model, i,j+1, valueStr);
				gridview_model_set_value(model, j,i+1, valueStr);
			}
		}
		clean_and_reset_data();
		change_zoom_controls(FALSE);
		update_map_nav ();
		update_group_nav ();
	}

	// Clean input data memory
	for (i = 0; i < numberOfGenes; i++) 
		g_free(data[i]);
	g_free(data);
	for (i = 0; i < numberOfGenes; i++) 
		g_free(geneNames[i]);
	g_free(geneNames);
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

void btprevGrupo_clicked(GtkButton *sender) {
	//
	global_currentGroup--;
	numMaps = numberOfMapsPerGroup[global_currentGroup-1];
	mapList = groupsData[global_currentGroup-1];
	global_currentMap = 1;
	render_current_map ();
	update_map_nav ();
	update_group_nav ();
}

void btnextGrupo_clicked(GtkButton *sender) {
	global_currentGroup++;
	numMaps = numberOfMapsPerGroup[global_currentGroup-1];
	mapList = groupsData[global_currentGroup-1];
	global_currentMap = 1;
	render_current_map ();
	update_map_nav ();
	update_group_nav ();
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
void iminew_activate(GtkMenuItem *sender, gpointer args) {
	clean_and_reset_data();
	change_zoom_controls(FALSE);
	update_map_nav ();
	update_group_nav ();
	gridview_set_dimensions(args, 3);
	
	g_critical("iminew clicked!");
}
/* ---------------------------------------------------------------- */
void imiopen_activate(GtkMenuItem *sender, gpointer args) {
	clean_and_reset_data();
	change_zoom_controls(FALSE);
	update_map_nav ();
	update_group_nav ();
	
	GtkWidget *dialog = gtk_file_chooser_dialog_new(
		"Open",
		NULL,
		GTK_FILE_CHOOSER_ACTION_OPEN,
		"Cancel",
		GTK_RESPONSE_CANCEL,
		"Open",
		GTK_RESPONSE_ACCEPT,
		NULL
	);

	gint res = gtk_dialog_run(GTK_DIALOG(dialog));
	if (res == GTK_RESPONSE_ACCEPT) {
		GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
		char *filename = gtk_file_chooser_get_filename(chooser);
		gridview_load(args, filename);
		g_free(filename);
	}

	gtk_widget_destroy(dialog);

	g_critical("imiopen clicked!");
}
/* ---------------------------------------------------------------- */
void imisave_activate(GtkMenuItem *sender, gpointer args) {
	GtkWidget *dialog = gtk_file_chooser_dialog_new(
		"Save",
		NULL,
		GTK_FILE_CHOOSER_ACTION_SAVE,
		"Cancel",
		GTK_RESPONSE_CANCEL,
		"Save",
		GTK_RESPONSE_ACCEPT,
		NULL
	);
	
	GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);

	gtk_file_chooser_set_do_overwrite_confirmation(chooser, TRUE);

	gtk_file_chooser_set_current_name(chooser, "Untitled.csv");

	gint res = gtk_dialog_run(GTK_DIALOG(dialog));
	
	if (res == GTK_RESPONSE_ACCEPT) {
		char *filename = gtk_file_chooser_get_filename(chooser);
		
		gridview_save(args, filename);
		g_free(filename);
	}

	gtk_widget_destroy(dialog);
	
	g_critical("imisave clicked!");
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
/* ---------------------------------------------------------------- */
void
on_menuitem_exit (GtkMenuItem *menuitem,
               gpointer     user_data) 
{
	clean_and_reset_data();
	GtkWindow* mainWindow = GTK_WINDOW(gtk_builder_get_object(global_builder, "window"));
	GtkApplication* application = gtk_window_get_application (mainWindow);
	g_application_quit (G_APPLICATION(application));
}
/* ---------------------------------------------------------------- */
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

static void createDummyDataFor3Genes() 
{
	numberOfGroups = 3;
	numberOfMapsPerGroup = g_malloc(sizeof(gint) * 3);
	numberOfMapsPerGroup[0] = 1;
	numberOfMapsPerGroup[1] = 2;
	numberOfMapsPerGroup[2] = 3;
	groupsData = g_malloc(sizeof(gdouble**) * 3); // Three linkage groups
	groupsData[0] = g_malloc(sizeof(gdouble*) * 1); // 1 map
	groupsData[1] = g_malloc(sizeof(gdouble*) * 2); // 2 maps
	groupsData[2] = g_malloc(sizeof(gdouble*) * 3); // 3 maps
	// 3 genes
	groupsData[0][0] = g_malloc(sizeof(gdouble) * 3);
	groupsData[0][0][0] = 0.2;
	groupsData[0][0][1] = -1;
	groupsData[0][0][2] = 0.33;
	groupsData[1][0] = g_malloc(sizeof(gdouble) * 3);
	groupsData[1][0][0] = -1;
	groupsData[1][0][1] = 0.22;
	groupsData[1][0][2] = 0.14;
	groupsData[1][1] = g_malloc(sizeof(gdouble) * 3);
	groupsData[1][1][0] = 0.1;
	groupsData[1][1][1] = 0.12;
	groupsData[1][1][2] = -1;
	groupsData[2][0] = g_malloc(sizeof(gdouble) * 3);
	groupsData[2][0][0] = 0.04;
	groupsData[2][0][1] = 0.1;
	groupsData[2][0][2] = 0.3;
	groupsData[2][1] = g_malloc(sizeof(gdouble) * 3);
	groupsData[2][1][0] = 0.2;
	groupsData[2][1][1] = 0.3;
	groupsData[2][1][2] = 0.23;
	groupsData[2][2] = g_malloc(sizeof(gdouble) * 3);
	groupsData[2][2][0] = 0.1;
	groupsData[2][2][1] = 0.25;
	groupsData[2][2][2] = 0.12;
}

/* ---------------------------------------------------------------- */
#endif
