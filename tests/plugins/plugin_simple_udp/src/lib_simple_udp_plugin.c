
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "api/zvm.h"
#include "manifest.h"
#include "zplugin_context.h"

//////
//
static FILE *logging_out = 0;

//////
//
struct simple_udp_data
{
  int                socket_fd  ;
  struct sockaddr_in server_addr;
  struct sockaddr_in self_addr  ;

};

//////
//- Forward declarations
int simple_udp_close( struct ChannelDesc *channel);

//- rjm FIXME add proper error checking...
void
simple_udp_open( struct ChannelDesc *channel)
{
  struct simple_udp_data *handle = g_malloc0( sizeof( struct simple_udp_data));


  //////-
  if( handle)
  {
    char **tokens = g_strsplit( channel->name, ":", 2);

    //////-
    handle->server_addr.sin_family = AF_INET;
    handle->server_addr.sin_port   = htons( atoi( tokens[1]));

    handle->self_addr.sin_family      = AF_INET;
    handle->self_addr.sin_addr.s_addr = htonl( INADDR_ANY);
    handle->self_addr.sin_port        = 0;
  
    //-
    if( inet_aton( tokens[0],
          (struct in_addr *)&( handle->server_addr.sin_addr.s_addr)))
    {
      //////-
      handle->socket_fd = socket( AF_INET, SOCK_DGRAM, 0);

      if( handle->socket_fd > 0)
      {
        //////-
        int optval = 1;
        setsockopt( handle->socket_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof( 1));

        //////-
        if( bind( handle->socket_fd, (struct sockaddr *)&handle->self_addr,
              sizeof( handle->self_addr)) == 0)
        {
          //////-
          channel->handle = handle;

          fprintf( logging_out, "[*] simple_udp_open: [%s]\n", channel->name);
        }
      }
    }
    else
    { //- set errno to reflect the error
      errno = EDESTADDRREQ; //- Destination address required
    }
    g_strfreev( tokens);
  }

}

int
simple_udp_close( struct ChannelDesc *channel)
{
  int retval = -1;

  fprintf( logging_out, "[*] simple_udp_close: \n" );
  if( channel && channel->handle)
  {
    struct simple_udp_data *handle = channel->handle;
    close( handle->socket_fd);
    g_free( channel->handle);
    channel->handle = 0;
  }

  return( retval);
}

ssize_t
simple_udp_read( struct ChannelDesc *channel, char *buf, size_t max_len, off_t offset)
{
  ssize_t retval = -1;


  //////-
  if( buf)
  {
    struct sockaddr src_addr     = {0};
    socklen_t       src_addr_len = sizeof( src_addr);
    char            len          = 0;

    struct simple_udp_data *handle  = channel->handle;

    //////-
    //- recv length of buffer
    //-  (this implementation is limited to a single byte length)
    retval = recvfrom( handle->socket_fd, (void *)&len, 1, 0,
        &src_addr, &src_addr_len);

    if( len <= max_len)
    {
      //////-
      //- recv the buffer
      bzero( (void *)&src_addr, sizeof( src_addr));
      retval = recvfrom( handle->socket_fd, buf, len, 0,
          &src_addr, &src_addr_len);
    }
    else
    { 
      errno = EMSGSIZE; //- Message too long
    }
  }
  fprintf( logging_out, "[*] simple_udp_read:[%zu] buf: [%s] max_len: [%zu]\n", retval, buf, max_len );

  return( retval);
}

ssize_t
simple_udp_write( struct ChannelDesc *channel, const char *buf, size_t buf_len, off_t offset)
{
  ssize_t retval = -1; 

 
  //////-
  if( buf)
  {
    struct simple_udp_data *handle = channel->handle;
    char   len                     = (char )buf_len;

    //////-
    //- send length of buffer
    //-  (this implementation is limited to a single byte length)
    retval = sendto( handle->socket_fd, (void *)&len, 1, 0,
                     (struct sockaddr *)&(handle->server_addr),
                     sizeof( handle->server_addr));
    //////-
    //- Send the buffer...
    retval = sendto( handle->socket_fd, buf, buf_len, 0,
                     (struct sockaddr *)&(handle->server_addr),
                     sizeof( handle->server_addr));
  }
  fprintf( logging_out, "[*] simple_udp_write:[%zu] buf: [%s] len: [%zu]\n", retval, buf, buf_len );

  return( retval);
}

void __attribute__ ((constructor))
_init( void)
{

  logging_out = fopen( "./simple_udp_plugin.log", "a");
  fprintf( logging_out, "[*] simple_udp_init...\n");
}

void __attribute__ ((destructor))
_fini( void)
{
  fprintf( logging_out, "[*] simple_udp_fini...\n");
  fclose( logging_out);
}

void
init_zplugin( plugin_context_t *plugin)
{
  if( plugin)
  {
    plugin->protocol = "simple_udp";

    plugin->open  = simple_udp_open;
    plugin->close = simple_udp_close;
    plugin->read  = simple_udp_read;
    plugin->write = simple_udp_write;
  }
}
