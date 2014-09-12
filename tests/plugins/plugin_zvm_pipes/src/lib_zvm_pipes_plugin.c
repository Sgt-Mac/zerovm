
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <zvm_zpipes.h>

#include "api/zvm.h"
#include "manifest.h"
#include "zplugin_context.h"


//- rjm FIXME add proper error checking...

void
zp_open( struct ChannelDesc *channel)
{
  fprintf( stdout, "[-] zvm_pipe_open: [%s]\n", channel->name);
  channel->handle = zvm_pipe_open( channel->name);
}

int
zp_close( struct ChannelDesc *channel)
{
  int retval = -1;

  fprintf( stdout, "[-] zvm_pipe_close: \n" );
  retval = zvm_pipe_close( channel->handle);

  return( retval);
}

ssize_t
zp_read( struct ChannelDesc *channel, char *buf, size_t len, off_t offset)
{
  ssize_t retval = -1;

  retval = zvm_pipe_read( channel->handle, buf, len);
  fprintf( stdout, "[-] zvm_pipe_read: buf: [%s] len: [%zu]\n", buf, len );

  return( retval);
}

ssize_t
zp_write( struct ChannelDesc *channel, const char *buf, size_t len, off_t offset)
{
  ssize_t retval = -1; 

  fprintf( stdout, "[-] zvm_pipe_write: buf: [%s] len: [%zu]\n", buf, len );
  retval = zvm_pipe_write( channel->handle, (void *)buf, len);

  return( retval);
}

void __attribute__ ((constructor))
_init( void)
{
  fprintf( stdout, "[-] zvm_pipe_init...\n");
}

void __attribute__ ((destructor))
_fini( void)
{
  fprintf( stdout, "[-] zvm_pipe_fini...\n");
}

void
init_zplugin( plugin_context_t *plugin)
{
  if( plugin)
  {
    plugin->protocol = "zvm_pipe";

    plugin->open  = zp_open;
    plugin->close = zp_close;
    plugin->read  = zp_read;
    plugin->write = zp_write;
  }
}
