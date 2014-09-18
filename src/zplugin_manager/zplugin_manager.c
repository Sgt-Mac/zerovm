
///////////////////////////////////////////////////////////////////////////////
//-
#include <assert.h>
#include <glib.h>

//-
#include "src/channels/channel.h"
#include "src/zplugin_manager/zplugin_manager.h"


////////////////////////////////////////////////////////////////////////////////
zplugin_manager_t *plugin_manager     = 0;
const char        *default_plugin_dir = "./plugins";

////////////////////////////////////////////////////////////////////////////////
//-
void add_plugin_dir   ( char *path);
void remove_plugin_dir( char *path);

int  load_zplugin     ( char             *name_or_fullpath );
int  unload_zplugin   ( plugin_context_t *plugin           );

////////////////////////////////////////////////////////////////////////////////
//-
plugin_context_t * load_zplugin_by_fullpath    ( char *plugin_path          );
plugin_context_t * load_zplugin_by_path_by_name( char *path, char *name     );
plugin_context_t * load_zplugin_by_name        ( char *name                 );
plugin_context_t * get_plugin_by_channel       ( struct ChannelDesc *channel);

////////////////////////////////////////////////////////////////////////////////
//-
zplugin_manager_t *
zPluginManagerCtor()
{
  //////-
  assert( plugin_manager == 0);

  if( !plugin_manager)
  {
    plugin_manager = (zplugin_manager_t *)g_malloc0( sizeof( zplugin_manager_t));
  }

  if( plugin_manager)
  {
    //////-
    plugin_manager->plugin_dir_list       = g_ptr_array_new();
    plugin_manager->plugin_list           = g_ptr_array_new();

    //////-
    plugin_manager->add_plugin_dir        = add_plugin_dir;
    plugin_manager->remove_plugin_dir     = remove_plugin_dir;

    //////-
    plugin_manager->load_zplugin          = load_zplugin;
    plugin_manager->unload_zplugin        = unload_zplugin; 
    plugin_manager->get_plugin_by_channel = get_plugin_by_channel;

    //////-
    plugin_manager->load_zplugin_by_fullpath     = load_zplugin_by_fullpath    ;
    plugin_manager->load_zplugin_by_path_by_name = load_zplugin_by_path_by_name;
    plugin_manager->load_zplugin_by_name         = load_zplugin_by_name        ;
  }

  return( plugin_manager);
}

////////////////////////////////////////////////////////////////////////////////
//-
void
zPluginManagerDtor()
{
  //////-
  plugin_context_t *plugin = 0;

  //////-
  assert( plugin_manager != 0);

  //////-
  //- Iterate plugin list and unload any plugins that may remain
  while( plugin_manager->plugin_list->len > 0)
  {
    plugin = (plugin_context_t *)(g_ptr_array_remove_index( plugin_manager->plugin_list,
                                                    (plugin_manager->plugin_list->len-1)));

    //- force plugin unload
    plugin->ref_count = 0;

    //- unload plugin
    plugin_manager->unload_zplugin( plugin);

  }

  //////-
  g_ptr_array_free( plugin_manager->plugin_dir_list, TRUE);

  //////-
  g_free( plugin_manager);
  plugin_manager = 0;
}

////////////////////////////////////////////////////////////////////////////////
//-
void
add_plugin_dir( char *path)
{
  //////-
  assert( plugin_manager != 0);

  if( plugin_manager->plugin_dir_list && path)
  {
    GString *_path = g_string_new( g_strstrip( path));
    g_ptr_array_add( plugin_manager->plugin_dir_list, _path->str);
  }
}

////////////////////////////////////////////////////////////////////////////////
//-
void
remove_plugin_dir( char *path)
{
  //////-
  assert( plugin_manager != 0);

  if( plugin_manager->plugin_dir_list && path)
  {
    g_ptr_array_remove( plugin_manager->plugin_dir_list, path);
  }
}

////////////////////////////////////////////////////////////////////////////////
//-
plugin_context_t *
load_zplugin_by_fullpath( char *plugin_path)
{
  plugin_context_t *retval = 0;

  //////-
  assert( plugin_manager != 0);

  if( plugin_path)
  {
    if( !access( plugin_path, F_OK|R_OK))
    {
      retval = zplugin_load( plugin_path);
    }
  }

  return( retval);
}


////////////////////////////////////////////////////////////////////////////////
//-
plugin_context_t *
load_zplugin_by_path_by_name( char *path, char *name)
{
  plugin_context_t *retval = 0;

  //////-
  assert( plugin_manager != 0);

  if( path && name)
  {
    GString *plugin_path = g_string_new( path);

    if( path[strlen( path)-1] != '/')
    {
      g_string_append( plugin_path, "/");
    }
    g_string_append( plugin_path, name);

    retval = plugin_manager->load_zplugin_by_fullpath( plugin_path->str);
    g_string_free( plugin_path, TRUE);
  }
 
  return( retval);
}

////////////////////////////////////////////////////////////////////////////////
//-
plugin_context_t *
load_zplugin_by_name( char *name)
{
  plugin_context_t *retval = 0;

  //////-
  assert( plugin_manager != 0);

  //////-
  if( name)
  {
    //////-
    if( plugin_manager->plugin_dir_list->len > 0)
    {
      int index = 0;
      for( index = 0; index < plugin_manager->plugin_dir_list->len; index++)
      {
        char *path = (char *)(g_ptr_array_index( plugin_manager->plugin_dir_list, index));

        retval = plugin_manager->load_zplugin_by_path_by_name( path, name);
        if( retval)
        {
          break;
        }
      }
    }
    else
    { //- No search directories, so search default location
      retval = plugin_manager->load_zplugin_by_path_by_name( (char *)default_plugin_dir, name);
    }
  } // end if name

  return( retval);
}

////////////////////////////////////////////////////////////////////////////////
//-
//- Load a plugin into internal plugin list by either plugin name (eg plugin.so)
//-   or fullpath ( eg ./plugin_dir/plugin.so)
//- 
//- Returns: a plugin context or null on fail.
//-
int
load_zplugin( char *name_or_fullpath)
{
  int               retval = -1;
  plugin_context_t *plugin = 0;

  //////-
  assert( plugin_manager != 0);

  if( name_or_fullpath)
  {
    GString *plugin_name = g_string_new( g_strstrip( name_or_fullpath));

    if( g_strrstr( plugin_name->str, "/"))
    { //- Load by fullpath
      plugin = plugin_manager->load_zplugin_by_fullpath( plugin_name->str);
    }
    else
    { //- Load by searching directory paths
      plugin = plugin_manager->load_zplugin_by_name( plugin_name->str);
    }

    if( plugin)
    {
      g_ptr_array_add( plugin_manager->plugin_list, plugin);
      retval = 0;
    }
  }

  return( retval);
}

////////////////////////////////////////////////////////////////////////////////
//-
//- Unloads a plugin and removes it from the plugin managers plugin list if
//-   the plugins reference count is 0.
//-
//- Returns: 0 on success, not zero on fail
//-
int
unload_zplugin( plugin_context_t *plugin)
{
  int retval = -1;

  //////-
  assert( plugin_manager != 0);

  if( plugin)
  {
    retval = zplugin_unload( plugin);
    if( retval == 0)
    {
      g_ptr_array_remove( plugin_manager->plugin_list, (gpointer )plugin);
    }
  }

  return( retval);
}

////////////////////////////////////////////////////////////////////////////////
//-
//- Retrieves the plugin 
plugin_context_t *
get_plugin_by_channel( struct ChannelDesc *channel)
{
  plugin_context_t *retval = 0;
  plugin_context_t *temp   = 0;
  int index = 0;

  //////-
  assert( plugin_manager != 0);
  assert( channel        != 0);

  //////-
  for( index = 0; index < plugin_manager->plugin_list->len; index++)
  {
    char **tokens;
    temp = (plugin_context_t *)(g_ptr_array_index( plugin_manager->plugin_list, index));
    tokens = g_strsplit( channel->name, ":", 2);

    //////-
    if( !strcmp( tokens[0], temp->protocol))
    {
      retval = temp;
      retval->ref_count++;

      //////-
      if( strlen( tokens[1]) > 0)
      {
        char **name_tokens = g_strsplit( tokens[1], ":", 2);
        channel->name = g_strdup( name_tokens[1]);
        g_strfreev( name_tokens);
      }

      break;
    }

    g_strfreev(tokens);
  }

  return( retval);
}
