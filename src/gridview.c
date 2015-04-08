#include <gtk/gtk.h>
/* ---------------------------------------------------------------- */
void cell_edited(GtkCellRendererText *cellrenderer, gchar *pathstring, gchar *newtext, gpointer args) {
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(args));
	
	GtkTreePath *path = gtk_tree_path_new_from_string(pathstring);
	gint column = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(cellrenderer), "column"));
	GtkTreeIter iter;
	gtk_tree_model_get_iter(model, &iter, path);
	
	gtk_list_store_set(GTK_LIST_STORE(model), &iter, column, newtext, -1);

	gtk_tree_path_free(path);
	
	g_critical("cell (%s, %d) edited!", pathstring, column);
}
/* ---------------------------------------------------------------- */
void gridview_init(GtkWidget *gridview, int dimensions) {
	int i;
	char data[8];
	GType types[dimensions];
	GtkTreeViewColumn *column;
	GtkCellRenderer *cellrenderer;
	
	for (i=0; i<=dimensions; i++) {
		types[i] = G_TYPE_STRING;
		
		cellrenderer = gtk_cell_renderer_text_new();
		g_object_set_data(G_OBJECT(cellrenderer), "column", GINT_TO_POINTER(i));

		if (i == 0) {
			sprintf(data, "");
		} else {
			sprintf(data, "Gen %d", i);
			
			g_object_set(cellrenderer, "editable", TRUE, NULL);
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
				sprintf(data, "Gen %d", i + 1);
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
	char data[8];
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
			
			sprintf(data, "Gen %d", i);
		
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
				sprintf(data, "Gen %d", i + 1);
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
