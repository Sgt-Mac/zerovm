
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "zvm.h"
#include "zplugin_context.h"


struct ChannelDesc *
test_open( char *_string)
{
  struct ChannelDesc *retval = 0;

  fprintf( stdout, "[-] test_open: [%s]\n", _string);

  return( retval);
}

int
test_close( struct ChannelDesc *channel)
{
  int retval = -1;

  fprintf( stdout, "[-] test_close: \n" );

  return( retval);
}

#define MIN(a,b) (((a) < (b)) ? (a):(b))
ssize_t
test_read( struct ChannelDesc *channel, char *buf, size_t len, off_t offset)
{
  ssize_t retval = -1;
  char *msg="Hello World";

  if( buf)
  {
    retval = MIN( len, strlen( msg));
    memcpy( buf, msg, retval);
    fprintf( stdout, "[-] test_read: buf: [%s] len:[%d] \n", buf, (int )len );
  }

  return( retval);
}

ssize_t
test_write( struct ChannelDesc *channel, const char *buf, size_t len, off_t offset)
{
  ssize_t retval = -1; 

  retval = fprintf( stdout, "[-] test_write: buf: [%s] len: [%zu]\n", buf, len );

  return( retval);
}

void __attribute__ ((constructor))
_init( void)
{
  fprintf( stdout, "[-] test_init...\n");
}

void __attribute__ ((destructor))
_fini( void)
{
  fprintf( stdout, "[-] test_fini...\n");
}

void
init_zplugin( plugin_context_t *plugin)
{
  if( plugin)
  {
    plugin->protocol = "bogus";

    plugin->open  = test_open;
    plugin->close = test_close;
    plugin->read  = test_read;
    plugin->write = test_write;
  }
}
