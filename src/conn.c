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

#include <X11/Xatom.h>
#include <gdk/gdkx.h>
#include "conn.h"

gboolean connection_requestCommit( connection* obj, GString gstr )
{
    return obj->requestCommit( obj->plugin, gstr );
}

#include "interface.h"

static void connection_class_init( connection_class* klass );
static void connection_init( connection* obj );

GType connection_get_type( void )
{
    static GType connection_type = 0;
    if ( !connection_type ) {
        static const GTypeInfo connection_info = {
            sizeof( connection_class ),
            NULL,
            NULL,
            (GClassInitFunc)connection_class_init,
            NULL,
            NULL,
            sizeof( connection ),
            0,
            (GInstanceInitFunc)connection_init
        };
        connection_type = g_type_register_static( G_TYPE_OBJECT, "connection", &connection_info, 0 );
    }
    return connection_type;
}

static void connection_class_init( connection_class* klass )
{
    dbus_g_object_type_install_info( CONNECTION_TYPE, &dbus_glib_connection_object_info );
}
static void connection_init( connection* obj )
{
    DBusGConnection* conn;
    DBusGProxy* proxy;
    GError *error = NULL;
    guint request_name_result;

    conn = dbus_g_bus_get (DBUS_BUS_SESSION, &error);

    proxy = dbus_g_proxy_new_for_name( conn, "org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus" );

    dbus_g_proxy_call ( proxy, "RequestName", &error, G_TYPE_STRING, "it.rsc.fastsms.plugin.connection", G_TYPE_UINT, 0, G_TYPE_INVALID, G_TYPE_UINT, &request_name_result, G_TYPE_INVALID );

    dbus_g_connection_register_g_object ( conn, "/", G_OBJECT(obj) );
}

connection* connection_new(void)
{
    connection* obj;
    obj = g_object_new( CONNECTION_TYPE, NULL );
   
    return obj;
}
void connection_set( connection* conn, GObject* plugin, requestCommit_func func )
{
    conn->plugin = plugin;
    conn->requestCommit = func;
}

int showApplication( const gchar* str )
{
    GError* error = NULL;
    static DBusGConnection* conn = NULL;
    static DBusGProxy* proxy = NULL;

    if (!conn)
        conn = dbus_g_bus_get (DBUS_BUS_SESSION, &error);

    //open fastsms
    if (!proxy )
	proxy = dbus_g_proxy_new_for_name( conn, "it.rsc.fastsms", "/", "it.rsc.fastsms" );

    if ( !dbus_g_proxy_call( proxy, "surroundRequest", &error, G_TYPE_STRING, str, G_TYPE_INVALID, G_TYPE_INVALID ) ) {
        g_debug( "failed to call: %s", error->message );
        g_error_free(error);
        return 1;
    }
    return 0;
}

/* 
 * Check if client is a window or not.
 */
gboolean isWindow(Window win)
{
    gboolean bReturn = TRUE;
    GdkDisplay* gdkdisplay;
    Display *display;
    Atom* propertyList;
    int nPropCount = 0;
    int nChildCount;
    Window root;
    Window parent;
    Window* childList;

    gdkdisplay = gdk_display_get_default();
    display = GDK_DISPLAY_XDISPLAY(gdkdisplay);
    XQueryTree( display, win, &root, &parent, &childList, &nChildCount );
    XFree(childList);
    propertyList = XListProperties( display, win, &nPropCount );
    XFree(propertyList); //Only need number of...

    //If win is a windows nPropCount must be greater than 0 at this point
    if (( root == parent ) && ( nPropCount <= 0 )) 
    {   
	g_debug("Client is not a win");     
	bReturn = FALSE;
    }
    else
    {
        //Retrive first parent with nPropCount != 0
	while( nPropCount == 0 && root != parent )
	{		
		win = parent;
		XQueryTree( display, win, &root, &parent, &childList, &nChildCount );
		XFree(childList);
		propertyList = XListProperties( display, win, &nPropCount );
		XFree(propertyList); //Only need number of...
	}

	//Retrive classhint
	XClassHint classhint;
	XGetClassHint( display, win, &classhint );
	if ( classhint.res_name == NULL && classhint.res_class == NULL )
        {
		g_debug("Undefined win");
		bReturn = FALSE;
	}
	//TODO: When context-menu was open vkb not works. Check why
	else
		g_debug( "Client name = %s, class = %s", classhint.res_name, classhint.res_class );

	XFree( classhint.res_name );
	XFree( classhint.res_class );
    }

    return bReturn;
}
