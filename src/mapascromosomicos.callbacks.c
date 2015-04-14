#include <gtk/gtk.h>

#ifndef CALLBACKS
#define CALLBACKS
/* ---------------------------------------------------------------- */
void window_init(GtkBuilder *sender) {
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
void btmap_clicked(GtkButton *sender) {
	g_critical("btmap clicked!");
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
void imisave_activate(GtkMenuItem *sender, gpointer args) {
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;

	GtkWidget *dialog = gtk_file_chooser_dialog_new(
		"Save",
		NULL,
		action,
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
#endif
