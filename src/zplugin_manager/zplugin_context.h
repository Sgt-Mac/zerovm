#ifndef _zplugin_h_
#define _zplugin_h_


///////////////////////////////////////////////////////////////////////////////
//-
struct ChannelDesc;

///////////////////////////////////////////////////////////////////////////////
//-
typedef struct
{
  uint32_t major;
  uint32_t minor;
} zvm_zplugin_context_api_version_t;

extern zvm_zplugin_context_api_version_t version;


///////////////////////////////////////////////////////////////////////////////
//-
typedef struct 
{
  zvm_zplugin_context_api_version_t version;

  //////-
  //- Opaque protocol type string
  char *protocol;
 
  //////-
  //- Plugin Context Info
  void *handle;

  //////-
  //- 
  int ref_count;

  //////-
  //-
  void    (*open )( struct ChannelDesc *channel);
  int     (*close)( struct ChannelDesc *channel);
  ssize_t (*read )( struct ChannelDesc *channel,       char *buf, size_t len, off_t offset);
  ssize_t (*write)( struct ChannelDesc *channel, const char *buf, size_t len, off_t offset);

  //-
  int   (*truncate)( struct ChannelDesc *channel, off_t len);
  off_t (*lseek   )( struct ChannelDesc *channel, off_t offset, int whence);
  long  (*ftell   )( struct ChannelDesc *channel);


} plugin_context_t;


////////////////////////////////////////////////////////////////////////////////
//-
extern plugin_context_t * zplugin_load  ( char               *plugin_path);
extern int                zplugin_unload( plugin_context_t   *plugin     );

#endif /* _zplugin_h_ */ 

