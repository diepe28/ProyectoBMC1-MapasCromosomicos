/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * mapascromosomicos.c
 * Copyright (C) 2015 Ubuntu <diego@ubuntu>
 * 
 * MapasCromosomicos is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * MapasCromosomicos is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "mapascromosomicos.h"

#include <glib/gi18n.h>



/* For testing propose use the local (not installed) ui file */
/* #define UI_FILE PACKAGE_DATA_DIR"/ui/mapascromosomicos.ui" */
#define UI_FILE "src/mapascromosomicos.ui"
#define TOP_WINDOW "window"


G_DEFINE_TYPE(Mapascromosomicos, mapascromosomicos, GTK_TYPE_APPLICATION);

/* ANJUTA: Macro MAPASCROMOSOMICOS_APPLICATION gets Mapascromosomicos - DO NOT REMOVE */
struct _MapascromosomicosPrivate {
	/* ANJUTA: Widgets declaration for mapascromosomicos.ui - DO NOT REMOVE */
};

/* Create a new window loading a file */
static void mapascromosomicos_new_window(GApplication *app, GFile *file) {
	GtkWidget *window;

	GtkBuilder *builder;
	GError* error = NULL;

	MapascromosomicosPrivate *priv = MAPASCROMOSOMICOS_APPLICATION(app)->priv;

	/* Load UI from file */
	builder = gtk_builder_new();
	if (!gtk_builder_add_from_file(builder, UI_FILE, &error)) {
		g_critical("Couldn't load builder file: %s", error->message);
		g_error_free(error);
	}

	/* Auto-connect signal handlers */
	gtk_builder_connect_signals(builder, app);

	/* Get the window object from the ui file */
	window = GTK_WIDGET(gtk_builder_get_object(builder, TOP_WINDOW));
    if (!window) {
		g_critical(
			"Widget \"%s\" is missing in file %s.",
			TOP_WINDOW,
			UI_FILE
		);
    }

	
	/* ANJUTA: Widgets initialization for mapascromosomicos.ui - DO NOT REMOVE */

	window_init(builder);
	
	g_object_unref(builder);
	
	
	gtk_window_set_application(GTK_WINDOW(window), GTK_APPLICATION(app));
	if (file != NULL) {
		/* TODO: Add code here to open the file in the new window */
	}

	gtk_widget_show_all(GTK_WIDGET(window));
}


/* GApplication implementation */
static void mapascromosomicos_activate(GApplication *application) {
	mapascromosomicos_new_window(application, NULL);
}

static void mapascromosomicos_open(GApplication *application, GFile **files, gint n_files, const gchar *hint) {
	gint i;

	for (i = 0; i < n_files; i++) {
		mapascromosomicos_new_window(application, files[i]);
	}
}

static void mapascromosomicos_init(Mapascromosomicos *object) {
	object->priv = G_TYPE_INSTANCE_GET_PRIVATE(object, MAPASCROMOSOMICOS_TYPE_APPLICATION, MapascromosomicosPrivate);
}

static void mapascromosomicos_finalize(GObject *object) {
	G_OBJECT_CLASS(mapascromosomicos_parent_class)->finalize(object);
}

static void mapascromosomicos_class_init(MapascromosomicosClass *klass) {
	G_APPLICATION_CLASS(klass)->activate = mapascromosomicos_activate;
	G_APPLICATION_CLASS(klass)->open = mapascromosomicos_open;

	g_type_class_add_private(klass, sizeof(MapascromosomicosPrivate));

	G_OBJECT_CLASS(klass)->finalize = mapascromosomicos_finalize;
}

Mapascromosomicos *mapascromosomicos_new(void) {
	g_type_init();

	return g_object_new(
		mapascromosomicos_get_type(),
		"application-id",
		"org.gnome.mapascromosomicos",
		"flags",
		G_APPLICATION_HANDLES_OPEN,
		NULL
	);
}

