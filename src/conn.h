/****************************************************************************
**
** Copyright (C) 2010  Aimone Davide realsportcars@hotmail.com
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef CONNECTION_H
#define CONNECTION_H

#include <glib-object.h>
#include <glib.h>
#include <dbus/dbus-glib.h>
#include <X11/Xlib.h>


#define CONNECTION_TYPE (connection_get_type())
#define CONNECTION(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), CONNECTION, connection))

typedef gboolean (*requestCommit_func)( GObject* obj, GString gstr );

typedef struct{
    GObject parent;
    requestCommit_func requestCommit;
    GObject* plugin;
}connection;

typedef struct{
    GObjectClass parent_class;
}connection_class;

GType connection_get_type(void);
connection* connection_new(void);
void connection_set( connection* conn, GObject* plugin, requestCommit_func func );
int showApplication( const gchar* str );
gboolean check_x11win_classhint( Window win );

#endif
