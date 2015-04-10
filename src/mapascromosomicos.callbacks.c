#include <gtk/gtk.h>
#include "Algoritmo.h"

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
void btmap_clicked(GtkButton *sender) {
	GtkSpinButton* spin_button = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spinbutton"));
	gdouble number_of_genes = gtk_spin_button_get_value(spin_button);

	// TODO: Simulate call to algorithm. Replace with the proper code.
		numMaps = 2;
	    double map1[3] = {0, 30, 40};
		double map2[3] = {30, 0, 40};
		double* maps[2] = {map1, map2};
		mapList = maps;
	//
	
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
