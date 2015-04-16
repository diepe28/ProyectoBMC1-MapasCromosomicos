#include <stdlib.h>
#include <gtk/gtk.h>

#ifndef GRIDVIEW
#define GRIDVIEW

#define CELL_MAX_SIZE 8
/* ---------------------------------------------------------------- */
gboolean gridrow_save(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer args) {
	char *filepath = args;
	char *pathstring = gtk_tree_path_to_string(path);
	char *value;
	int i;
	int n = gtk_tree_model_get_n_columns(model);

	FILE *file;
  if (!(file = fopen(filepath, "a"))) {
		return TRUE;
	}

	char *data[n + 1];
	for (i=0; i<n; i++) {
		gtk_tree_model_get(model, iter, i, &value, -1);

		data[i] = value;
	}

	data[n] = NULL;

	char *row = g_strjoinv(";", data);
	fprintf(file, "%s\n", row);
	
	g_free(row);
	g_free(pathstring);
	fclose(file);

	return FALSE;
}
/* ---------------------------------------------------------------- */
void gridview_save(GtkWidget *gridview, char* filepath) {
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(gridview));
	int n = gtk_tree_view_get_n_columns(GTK_TREE_VIEW(gridview));
	
	FILE *file;
  if (file = fopen(filepath, "r")) {
		fclose(file);
		unlink(filepath);
	}

  if (file = fopen(filepath, "a")) {
		fprintf(file, "%d\n", n);
		fclose(file);
	}
	
	gtk_tree_model_foreach(model, gridrow_save, filepath);
}
/* ---------------------------------------------------------------- */
void gridview_model_set_value(GtkTreeModel *model, gint row, gint column, gchar *text) {
	GtkTreeIter iter;
	
	GtkTreePath *path = gtk_tree_path_new_from_indices(row, column, -1);
	
	gtk_tree_model_get_iter(model, &iter, path);

	gtk_list_store_set(GTK_LIST_STORE(model), &iter, column, text, -1);

	gtk_tree_path_free(path);
	
	g_critical("cell (%d, %d) edited!", row, column);
}
/* ---------------------------------------------------------------- */
void col0_edited(GtkCellRendererText *cellrenderer, gchar *pathstring, gchar *newtext, gpointer args) {
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(args));
	
	gint x = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(cellrenderer), "column"));
	gint y = atoi(pathstring);

	newtext = g_strjoinv("_", g_strsplit(newtext, " ", CELL_MAX_SIZE));

	GtkTreeViewColumn *column = gtk_tree_view_get_column(GTK_TREE_VIEW(args), y + 1);
	gtk_tree_view_column_set_title(column, newtext);
	
	gridview_model_set_value(model, y, x, newtext);
}
/* ---------------------------------------------------------------- */
void cell_edited(GtkCellRendererText *cellrenderer, gchar *pathstring, gchar *newtext, gpointer args) {

	if (newtext[0] != '\0') { 
		float value = g_ascii_strtod(newtext, NULL);

		gboolean isnan = value == 0 && strcmp("0", newtext) != 0;

		if (isnan || value < 0 || value > 0.5) {
			return;
		}
	}
	
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(args));
	
	gint x = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(cellrenderer), "column"));
	gint y = atoi(pathstring);

	if (y + 1 < x) {
		gridview_model_set_value(model, y, x, newtext);
		gridview_model_set_value(model, x - 1, y + 1, newtext);
	}
}
/* ---------------------------------------------------------------- */
void gridview_init(GtkWidget *gridview, int dimensions) {
	int i;
	char data[CELL_MAX_SIZE];
	GType types[dimensions];
	GtkTreeViewColumn *column;
	GtkCellRenderer *cellrenderer;
	
	for (i=0; i<=dimensions; i++) {
		types[i] = G_TYPE_STRING;
		
		cellrenderer = gtk_cell_renderer_text_new();
		g_object_set_data(G_OBJECT(cellrenderer), "column", GINT_TO_POINTER(i));
		g_object_set(cellrenderer, "editable", TRUE, NULL);

		if (i == 0) {
			sprintf(data, "");

			g_signal_connect(cellrenderer, "edited", (GCallback) col0_edited, gridview);
		} else {
			sprintf(data, "G%d", i);
			
			g_signal_connect(cellrenderer, "edited", (GCallback) cell_edited, gridview);
		}
		
		column = gtk_tree_view_column_new_with_attributes(data, cellrenderer, "text", i, NULL);
		
		gtk_tree_view_append_column(GTK_TREE_VIEW(gridview), column);
	}

	int j;
	GtkTreeIter iter;
	GtkListStore *row = gtk_list_store_newv(dimensions + 1, types);
 
	for (i=0; i<dimensions; i++) {
		GValue *values = g_slice_alloc0(sizeof(GValue)*(dimensions + 1));
		gint *colids = g_slice_alloc0(sizeof(gint)*(dimensions + 1));
	
		// memset(values, 0x0, sizeof(values));
		
		gtk_list_store_append(row, &iter);

		for (j=0; j<=dimensions; j++) {
			if (j == 0) {
				column = gtk_tree_view_get_column(GTK_TREE_VIEW(gridview), i + 1);
				sprintf(data, "%s", gtk_tree_view_column_get_title(column));
			} else if (j == i + 1) {
				sprintf(data, "0");
			} else {
				sprintf(data, "");
			}
			
			// g_assert(!G_VALUE_HOLDS_STRING(&values[j]));
			g_value_init(&values[j], G_TYPE_STRING);
			g_value_set_string(&values[j], data);

			colids[j] = j;
		}

		gtk_list_store_set_valuesv(row, &iter, colids, values, dimensions + 1);
		
		g_slice_free1(sizeof(GValue)*(dimensions + 1), values);
		g_slice_free1(sizeof(gint)*(dimensions + 1), colids);
	}

	gtk_tree_view_set_model(GTK_TREE_VIEW(gridview), GTK_TREE_MODEL(row));
}
/* ---------------------------------------------------------------- */
void gridview_set_dimensions(GtkWidget *gridview, int dimensions) {
	int n = gtk_tree_view_get_n_columns(GTK_TREE_VIEW(gridview));
	int i;
	char data[CELL_MAX_SIZE];
	GType types[dimensions];
	GtkTreeViewColumn *column;
	GtkCellRenderer *cellrenderer;

	for (i=0; i<=dimensions; i++) {
		types[i] = G_TYPE_STRING;
	}

	if (n <= dimensions) {
		for (i=n; i<=dimensions; i++) {
			cellrenderer = gtk_cell_renderer_text_new();
			g_object_set_data(G_OBJECT(cellrenderer), "column", GINT_TO_POINTER(i));
			g_object_set(cellrenderer, "editable", TRUE, NULL);
			g_signal_connect(cellrenderer, "edited", (GCallback) cell_edited, gridview);
			
			sprintf(data, "G%d", i);
		
			column = gtk_tree_view_column_new_with_attributes(data, cellrenderer, "text", i, NULL);
			gtk_tree_view_append_column(GTK_TREE_VIEW(gridview), column);
		}
	} else {
		for (i=n-1; i>dimensions; i--) {
			column = gtk_tree_view_get_column(GTK_TREE_VIEW(gridview), i);
			gtk_tree_view_remove_column(GTK_TREE_VIEW(gridview), column);
		}
	}
	
	int j;
	GtkTreeIter iter;
	GtkListStore *row = gtk_list_store_newv(dimensions + 1, types);
 
	for (i=0; i<dimensions; i++) {
		GValue *values = g_slice_alloc0(sizeof(GValue)*(dimensions + 1));
		gint *colids = g_slice_alloc0(sizeof(gint)*(dimensions + 1));
	
		// memset(values, 0x0, sizeof(values));
		
		gtk_list_store_append(row, &iter);

		for (j=0; j<=dimensions; j++) {
			if (j == 0) {
				column = gtk_tree_view_get_column(GTK_TREE_VIEW(gridview), i + 1);
				g_snprintf(data, CELL_MAX_SIZE, "%s", gtk_tree_view_column_get_title(column));
			} else if (j == i + 1) {
				g_snprintf(data, CELL_MAX_SIZE, "0");
			} else {
				g_snprintf(data, CELL_MAX_SIZE, "");
			}
			
			// g_assert(!G_VALUE_HOLDS_STRING(&values[j]));
			g_value_init(&values[j], G_TYPE_STRING);
			g_value_set_string(&values[j], data);

			colids[j] = j;
		}

		gtk_list_store_set_valuesv(row, &iter, colids, values, dimensions + 1);
		
		g_slice_free1(sizeof(GValue)*(dimensions + 1), values);
		g_slice_free1(sizeof(gint)*(dimensions + 1), colids);
	}

	gtk_tree_view_set_model(GTK_TREE_VIEW(gridview), GTK_TREE_MODEL(row));
}
/* ---------------------------------------------------------------- */
void gridview_load(GtkWidget *gridview, char* filepath, GtkSpinButton* spinButton) {
	FILE *file;
	if (!(file = fopen(filepath, "r"))) {
		return;
	}

	int n;
	fscanf(file, "%d", &n);

	gridview_set_dimensions(gridview, n - 1);
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(gridview));
	GtkTreeViewColumn *column;

	char *data = g_slice_alloc0(sizeof(char*)*(CELL_MAX_SIZE + 1)*n);
	char **values;

	int i = 0;
	int j;
	do {
		fscanf(file, "%s", data);

		values = g_strsplit(data, ";", n);
		j = 0;
		while (values[j] != NULL) {
			if (j == 0) {
				column = gtk_tree_view_get_column(GTK_TREE_VIEW(gridview), i + 1);
				gtk_tree_view_column_set_title(column, values[j]);
			}
			gridview_model_set_value(model, i, j, values[j]);
			j++;
		}
		i++;
	} while (!feof(file));

	fclose(file);
	gtk_spin_button_set_value(spinButton, n-1);
}
/* ---------------------------------------------------------------- */
#endif
