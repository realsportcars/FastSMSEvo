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

#include "conn.h"
#include <hildon-input-method/hildon-im-plugin.h>

void LogToFile (const gchar *, GLogLevelFlags, const gchar *, gpointer );

typedef struct{
    GtkWindow parent;
}him_plugin;

typedef struct{
    GtkWindowClass parent;
}him_plugin_class;

typedef struct{
    HildonIMUI* ui;
    connection* conn;
}him_plugin_private;

#define HIM_PLUGIN_TYPE (him_plugin_get_type())
#define HIM_PLUGIN( obj ) GTK_CHECK_CAST( obj, HIM_PLUGIN_TYPE, him_plugin )
#define HIM_PLUGIN_CLASS( klass ) GTK_CHECK_CLASS_CAST( klass, HIM_PLUGIN_TYPE, him_plugin_class )
#define HIM_PLUGIN_PRIVATE( obj ) G_TYPE_INSTANCE_GET_PRIVATE( obj, HIM_PLUGIN_TYPE, him_plugin_private )

static GType him_plugin_type = 0;
static GtkWidgetClass* parent_class = NULL;

static connection* conn;

static gint cursor_offset;
static gint surround_length;
static gboolean bDirectCommit;

GType him_plugin_get_type(void){ return him_plugin_type; }


gboolean him_plugin_requestCommit( GObject* plugin, GString gstr )
{
g_debug("him_plugin_requestCommit");

	GString *newStr = g_string_new(gstr.str);
	//g_debug("Received: \"%s\"", newStr->str );	

    	him_plugin_private* priv = HIM_PLUGIN_PRIVATE( plugin );

    	if ( bDirectCommit )
    	{
		//Delete previous content if needed
		hildon_im_ui_send_surrounding_offset( priv->ui, TRUE, surround_length - cursor_offset );
		gint i;
		for( i = 0; i < surround_length; i++ )
		{
		    hildon_im_ui_send_communication_message( priv->ui, HILDON_IM_CONTEXT_HANDLE_BACKSPACE );
		}
		gsize len = newStr->len;
		gboolean newline = FALSE;

		//Process last \n (if exists) like a system return
		if(len > 0)
		{
			if(newStr->str[len-1] == '\n')
			{
				newline = TRUE;
				newStr = g_string_truncate(newStr, len -1);
			}
		}
	
		//g_debug("Writing: \"%s\"", gstr.str );
		hildon_im_ui_send_utf8( priv->ui, newStr->str);	
		
		if(newline)
			hildon_im_ui_send_communication_message( priv->ui, HILDON_IM_CONTEXT_HANDLE_ENTER );
	}
	else
        	hildon_im_ui_send_surrounding_content( priv->ui, newStr->str );

	return TRUE;
}

/* called whenever the plug-in becomes available to the user */
static void enable( HildonIMPlugin* plugin, gboolean init )
{
g_debug("enable");

	/*  Send  g_debug() to the file log */
  	g_log_set_handler (NULL, G_LOG_LEVEL_DEBUG, LogToFile, NULL); 

    	him_plugin_private* priv = HIM_PLUGIN_PRIVATE( plugin );
    	priv->conn = conn;
    	connection_set( priv->conn, (GObject*)plugin, him_plugin_requestCommit );

	Window win = hildon_im_ui_get_input_window( priv->ui );
	bDirectCommit = isWindow(win);
        hildon_im_ui_send_communication_message( priv->ui, HILDON_IM_CONTEXT_REQUEST_SURROUNDING_FULL );
        //hildon_im_ui_send_communication_message( priv->ui, HILDON_IM_CONTEXT_SURROUNDING_MODE );
}

static void surrounding_received( HildonIMPlugin *plugin, const gchar *surrounding, gint offset )
{
g_debug("surrounding_received");
//g_debug("%s",surrounding);

    him_plugin_private* priv = HIM_PLUGIN_PRIVATE( plugin );
    
    cursor_offset = offset;
    surround_length = g_utf8_strlen( surrounding, -1 );
    
    showApplication( surrounding );    
}

static void get_property( GObject* object, guint prop_id, GValue* value, GParamSpec* pspec )
{
//g_debug("get_property");

    him_plugin_private* priv;

    g_return_if_fail( HIM_PLUGIN(object) );
    priv = HIM_PLUGIN_PRIVATE(object);

    switch ( prop_id )
    {
        case HILDON_IM_PROP_UI:
            g_value_set_object( value, priv->ui );
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID( object, prop_id, pspec );
            break;
    }
}

static void set_property( GObject* object, guint prop_id, const GValue* value, GParamSpec* pspec )
{
//g_debug("set_property");

    him_plugin_private* priv;

    g_return_if_fail( HIM_PLUGIN(object) );
    priv = HIM_PLUGIN_PRIVATE(object);

    switch ( prop_id ){
        case HILDON_IM_PROP_UI:
            priv->ui = g_value_get_object( value );
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID( object, prop_id, pspec );
            break;
    }
}
static void finalize( GObject *object )
{
//g_debug("finalize");

    if ( G_OBJECT_CLASS( parent_class )->finalize )
        G_OBJECT_CLASS( parent_class )->finalize( object );
}
static void realize ( GtkWidget *widget )
{
//g_debug("realize");

    g_return_if_fail( HIM_PLUGIN(widget) );
    GTK_WIDGET_SET_FLAGS( widget, GTK_REALIZED );
}

static gboolean destroy( GtkWidget* widget, GdkEventAny* event )
{
//g_debug("destroy");
    return FALSE;
}
static gboolean expose( GtkWidget* widget, GdkEventExpose* event )
{
g_debug("expose");
    return TRUE;
}

static void him_plugin_class_init( him_plugin_class* klass )
{
//g_debug("him_plugin_class_init");

    GObjectClass* object_class;
    GtkWidgetClass* widget_class;

    parent_class = g_type_class_peek_parent( klass );
    g_type_class_add_private( klass, sizeof(him_plugin_private) );

    object_class = G_OBJECT_CLASS( klass );
    widget_class = GTK_WIDGET_CLASS( klass );

    object_class->set_property = set_property;
    object_class->get_property = get_property;
    object_class->finalize = finalize;

    widget_class->realize = realize;
    widget_class->destroy_event = destroy;
    widget_class->expose_event = expose;

    g_object_class_install_property(object_class, HILDON_IM_PROP_UI,
            g_param_spec_object(HILDON_IM_PROP_UI_DESCRIPTION, 
                HILDON_IM_PROP_UI_DESCRIPTION,
                "fastsms-input-pad-plugin",
                HILDON_IM_TYPE_UI,
                G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY
                )
            );
}

GtkWidget* him_plugin_widget_new( HildonIMUI* widget )
{    
//g_debug("him_plugin_widget_new");

    return g_object_new( HIM_PLUGIN_TYPE, HILDON_IM_PROP_UI_DESCRIPTION, widget, NULL );
}

HildonIMPlugin* module_create( HildonIMUI* widget )
{
//g_debug("module_create");

    return HILDON_IM_PLUGIN( him_plugin_widget_new( widget ) );
}

const HildonIMPluginInfo* hildon_im_plugin_get_info( void )
{
//g_debug("hildon_im_plugin_get_info");

    static const HildonIMPluginInfo info = 
    {
        "FASTSMS-INPUT-PAD-PLUGIN",
        "fastsms-input-pad-plugin",
        NULL,
        NULL,
        TRUE,
        FALSE,
        HILDON_IM_TYPE_SPECIAL_STANDALONE,
        HILDON_IM_GROUP_LATIN,
        HILDON_IM_DEFAULT_PLUGIN_PRIORITY,
        NULL,
        NULL,
        FALSE,
        HILDON_IM_DEFAULT_HEIGHT,
        HILDON_IM_TRIGGER_NONE
    };
    return &info;
}

gchar** hildon_im_plugin_get_available_languages( gboolean* free )
{
    static gchar* list[] = { "bg_BG", "cs_CZ", "da_DK", "de_DE", "en_GB", "en_US", "es_ES", "fi_FI", "fr_FR", "hr_HR", "it_IT", "nl_NL", "nn_NO", "no_NO", "pl_PL", "pt_PT", "sk_SK", "sl_SI", "sq_AL", "sr_YU", "sv_FI", "sv_SE", NULL };
    *free = FALSE;
    return list;


}

void LogToFile (const gchar *log_domain, GLogLevelFlags log_level,
    const gchar *message, gpointer user_data)
{
  FILE *logfile = fopen ("/opt/fastsms/data/plugin_fse.log", "a");
  if (logfile == NULL)
  {
    /*  Fall  back  to  console  output  if  unable  to  open  file  */
    printf ("Rerouted to console: %s\n", message);
    return;
  }

  fprintf (logfile, "%s\n", message);
  fclose (logfile);
} 

static void him_plugin_init( him_plugin* plugin )
{
    //g_debug("him_plugin_init");  
}
void module_exit( void )
{
   //g_debug("module_exit");  
}
static void disable( HildonIMPlugin* plugin )
{
/*    g_debug("Plugin disabling");

    gchar* msg = NULL;
    GError* error = NULL;
    static DBusGConnection* conn = NULL;
    static DBusGProxy* proxy = NULL;

g_debug("conn");
    if (!conn)
        conn = dbus_g_bus_get (DBUS_BUS_SESSION, &error);

g_debug("proxy");
    //open fastsms
    if (!proxy )
	proxy = dbus_g_proxy_new_for_name( conn, "it.rsc.fastsms", "/", "it.rsc.fastsms" );

g_debug("call");
    if ( !dbus_g_proxy_call( proxy, "commitRequest", &error, G_TYPE_INVALID, G_TYPE_STRING, msg, G_TYPE_INVALID ) ) 
    {
        g_debug( "failed to call: %s", error->message );
        g_error_free(error);
        return;
    }

g_debug("inizio");
    GString *newStr = g_string_new(msg);
    him_plugin_requestCommit((GObject*)plugin, *newStr);    
g_debug("fine");*/

    g_debug("Plugin disabled");
}
static void settings_changed( HildonIMPlugin* plugin, const gchar* key, const GConfValue* value )
{
    //g_debug("settings_changed");
}
static void language_settings_changed( HildonIMPlugin* plugin, const gint index )
{
    /* Not implemented */
}
static void input_mode_changed( HildonIMPlugin* plugin ){
    g_debug("Input mode changed");
}
static void keyboard_state_changed( HildonIMPlugin* plugin ){
    g_debug("keyboard_state_changed");
}
static void client_widget_changed( HildonIMPlugin* plugin )
{
    /*him_plugin_private* priv = HIM_PLUGIN_PRIVATE( plugin );
    Window win = hildon_im_ui_get_input_window( priv->ui );
    g_debug("Client widget changed!!! %s",gtk_window_get_title(&win));*/
    g_debug("Client widget changed!!!");
}
static void character_autocase( HildonIMPlugin* plugin ){
    /* Not implemented */

//g_debug("character_autocase");
}
static void clear( HildonIMPlugin* plugin ){
//g_debug("clear");
    /* Not implemented */
}
static void save_data( HildonIMPlugin* plugin ){
    //g_debug("save_data");
}
static void mode_a( HildonIMPlugin* plugin ){
    /* Not implemented */
}
static void mode_b( HildonIMPlugin* plugin ){
    /* Not implemented */
}
static void language( HildonIMPlugin* plugin ){
    /* Not implemented */
}
static void backspace( HildonIMPlugin* plugin ){
    /* Not implemented */
}
static void enter( HildonIMPlugin* plugin ){
    /* Not implemented */
}
static void tab( HildonIMPlugin* plugin ){
    /* Not implemented */
}
static void fullscreen( HildonIMPlugin* plugin, gboolean fullscreen ){
    //g_debug("fullscreen");
}
static void select_region( HildonIMPlugin *plugin, gint start, gint end ){
    /* Not implemented */
}
static void key_event( HildonIMPlugin *plugin, GdkEventType type, guint state, guint keyval, guint hardware_keycode ){
    //g_debug("key_event");
}
static void transition( HildonIMPlugin *plugin, gboolean from ){
    //g_debug("transition");
}
static void button_activated( HildonIMPlugin *plugin, HildonIMButton button, gboolean long_press ){
    /* Not implemented */
}
static void preedit_committed( HildonIMPlugin *plugin, const gchar *committed_preedit ){
    //g_debug("preedit_committed");
}

static void him_plugin_interface_init( HildonIMPluginIface* iface )
{
//g_debug("him_plugin_interface_init");

    iface->enable = enable;
    iface->disable = disable;
    iface->settings_changed = settings_changed;
    iface->language_settings_changed = language_settings_changed;
    iface->input_mode_changed = input_mode_changed;
    iface->keyboard_state_changed = keyboard_state_changed;
    iface->client_widget_changed = client_widget_changed;
    iface->character_autocase = character_autocase;
    iface->clear = clear;
    iface->save_data = save_data;
    iface->mode_a = mode_a;
    iface->mode_b = mode_b;
    iface->language = language;
    iface->backspace = backspace;
    iface->enter = enter;
    iface->tab = tab;
    iface->fullscreen = fullscreen;
    iface->select_region = select_region;
    iface->key_event = key_event;
    iface->transition = transition;
    iface->surrounding_received = surrounding_received;
    iface->button_activated = button_activated;
    iface->preedit_committed = preedit_committed;
}

void module_init( GTypeModule* module )
{
//g_debug("module_init");

    static const GTypeInfo type_info = {
        sizeof( him_plugin_class ),
        NULL, /* base_init */
        NULL, /* base_finalize */
        (GClassInitFunc) him_plugin_class_init,
        NULL, /* class_finalize */
        NULL, /* class_data */
        sizeof( him_plugin ),
        0, /* n_preallocs */
        (GInstanceInitFunc) him_plugin_init,
    };
    static const GInterfaceInfo him_plugin_info = {
        (GInterfaceInitFunc) him_plugin_interface_init,
        NULL, /* interface_finalize */
        NULL, /* interface_data */
    };
    him_plugin_type = g_type_module_register_type( module, GTK_TYPE_WIDGET, "him_plugin", &type_info, 0 );
    g_type_module_add_interface( module, HIM_PLUGIN_TYPE, HILDON_IM_TYPE_PLUGIN, &him_plugin_info );

    conn = connection_new();
    
    cursor_offset = 0;
    surround_length = 0;
}

