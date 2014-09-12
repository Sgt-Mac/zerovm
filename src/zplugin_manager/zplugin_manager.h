
///////////////////////////////////////////////////////////////////////////////
//-
#ifndef _plugin_manager_h_
#define _plugin_manager_h_

///////////////////////////////////////////////////////////////////////////////
#include <glib.h>
#include "src/zplugin_manager/zplugin_context.h"

///////////////////////////////////////////////////////////////////////////////
//-

///////////////////////////////////////////////////////////////////////////////
//-
typedef struct 
{
  //////-
  GPtrArray *plugin_list;
  int        plugin_list_size;

  //////-
  //- Directory list of where to search for plugins
  GPtrArray *plugin_dir_list ;

  //////-
  void (*add_plugin_dir   )( char *path);
  void (*remove_plugin_dir)( char *path);

  //////-
  int (*load_zplugin  )( char             *name_or_fullpath );
  int (*unload_zplugin)( plugin_context_t *plugin           );

  //////-
  //- Internal 'helper'/'utility' functions
  plugin_context_t * (*load_zplugin_by_fullpath    )( char *plugin_path          );
  plugin_context_t * (*load_zplugin_by_path_by_name)( char *path, char *name     );
  plugin_context_t * (*load_zplugin_by_name        )( char *name                 );
  plugin_context_t * (*get_plugin_by_channel       )( struct ChannelDesc *channel);



} zplugin_manager_t;

////////////////////////////////////////////////////////////////////////////////
extern zplugin_manager_t *zPluginManagerCtor();
extern void               zPluginManagerDtor();

////////////////////////////////////////////////////////////////////////////////
extern zplugin_manager_t *plugin_manager    ;
extern const char        *default_plugin_dir;


#endif /* _plugin_manager_h_ */
