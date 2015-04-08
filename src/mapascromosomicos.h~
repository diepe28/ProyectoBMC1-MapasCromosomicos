/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * mapascromosomicos.h
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

#ifndef _MAPASCROMOSOMICOS_
#define _MAPASCROMOSOMICOS_

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define MAPASCROMOSOMICOS_TYPE_APPLICATION             (mapascromosomicos_get_type ())
#define MAPASCROMOSOMICOS_APPLICATION(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), MAPASCROMOSOMICOS_TYPE_APPLICATION, Mapascromosomicos))
#define MAPASCROMOSOMICOS_APPLICATION_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), MAPASCROMOSOMICOS_TYPE_APPLICATION, MapascromosomicosClass))
#define MAPASCROMOSOMICOS_IS_APPLICATION(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MAPASCROMOSOMICOS_TYPE_APPLICATION))
#define MAPASCROMOSOMICOS_IS_APPLICATION_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), MAPASCROMOSOMICOS_TYPE_APPLICATION))
#define MAPASCROMOSOMICOS_APPLICATION_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), MAPASCROMOSOMICOS_TYPE_APPLICATION, MapascromosomicosClass))

typedef struct _MapascromosomicosClass MapascromosomicosClass;
typedef struct _Mapascromosomicos Mapascromosomicos;
typedef struct _MapascromosomicosPrivate MapascromosomicosPrivate;

struct _MapascromosomicosClass
{
	GtkApplicationClass parent_class;
};

struct _Mapascromosomicos
{
	GtkApplication parent_instance;

	MapascromosomicosPrivate *priv;

};

GType mapascromosomicos_get_type (void) G_GNUC_CONST;
Mapascromosomicos *mapascromosomicos_new (void);

/* Callbacks */

G_END_DECLS

#endif /* _APPLICATION_H_ */

