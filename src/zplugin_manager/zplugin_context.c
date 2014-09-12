
///////////////////////////////////////////////////////////////////////////////
//-
#include <sys/types.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <dlfcn.h>

//-
#include "src/zplugin_manager/zplugin_context.h"

////////////////////////////////////////////////////////////////////////////////
zvm_zplugin_context_api_version_t version = { .major=0, .minor=1 };


////////////////////////////////////////////////////////////////////////////////
//-
plugin_context_t *
zplugin_load( char *plugin_path)
{
  plugin_context_t *retval   = NULL;
  char             *error    = NULL;
  GString          *init_str = NULL;
  void             *handle   = NULL;
  void (*init_func)( plugin_context_t *);

  //////
  //-
  handle = dlopen( plugin_path, RTLD_LAZY);
  if( !(handle))
  {
    error = dlerror();
    goto plugin_load_error;
  }

  //////
  //-
  // init_str =  g_string_new( "init_zplugin");
  init_func = dlsym( handle, "init_zplugin"); //init_str->str);
  if( (error = dlerror()) != NULL)
  {
    goto plugin_load_error;
  }

  //////
  //-
  retval = (plugin_context_t *)g_malloc0( sizeof( plugin_context_t));
  if( !retval)
  {
    fprintf( stderr, "[!] Unable to allocate memory for plugin...\n");
    goto plugin_load_error;
  }

  //////
  //- 
  retval->handle    = handle;

  //////
  init_func( retval);

  //////
  //- rjm TODO verify plugin api version
  if( version.major != retval->version.major)
  {
    fprintf( stderr, "[!] plugin using different plugin api version...\n");

    zplugin_unload( retval);
    goto plugin_load_error;
  }
  
  
  //////
  return( retval);

  //////-
plugin_load_error:
  if( error)
  {
    fprintf( stderr, "[!] %s\n", error);
    fflush( stderr);
  }

  if( retval)
  {
    g_free( retval);
  }
  if( init_str)
  {
    g_free( retval);
  }

  return( NULL);
}


////////////////////////////////////////////////////////////////////////////////
//-
int
zplugin_unload( plugin_context_t *plugin)
{
  int retval = -1;
  if( plugin)
  {
    if( plugin->ref_count > 0)
    {
      plugin->ref_count--;
      retval = plugin->ref_count;
    }

    if( plugin->ref_count <= 0)
    {
      retval = dlclose( plugin->handle);
      g_free( plugin);
    }
  }

  return( retval);
}

