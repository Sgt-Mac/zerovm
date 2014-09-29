/*
 * Copyright 2008 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can
 * be found in the LICENSE file.
 */

/*
 * It can't get much simpler than this (uh, except for noop.c).
 */

#include <stdio.h>
#include "zvm.h"
#include "zvmlib.h"
#include "printf.h"  

#define PIPEOUT handle( "/dev/pipeout")
#define PIPEIN  handle( "/dev/pipein")
#define TEST    handle( "/dev/test")

#define MAX_BUF_SIZE 128

int
main(int argc, char* argv[])
{
  char *buf        = (char *)malloc( MAX_BUF_SIZE);
  char *msg1       = "This is a test...";
  char  msg_len    = (char )(STRLEN( msg1));
  int   count      = 0;
  int   bytes_read = 0;

  //////-
  int test = OPEN( "/dev/test");

  //////-
  fprintf( STDOUT, "[*] Beginning test...\n");
  fprintf( STDOUT, "[*] Sending: [%s]\n", msg1);

  //////-
  for( count = 0; count < 100; count++)
  {
    memset( buf, '\0', MAX_BUF_SIZE);
    //- Send buffer length then buffer
    z_pwrite( TEST, msg1, msg_len, 0);

    //- Recv buffer length then buffer
    bytes_read = z_pread( TEST, buf, 127, 0);
    fprintf( STDOUT, "[*] Recv'd  : Bytes:[%d]:[%s]\n", bytes_read, buf);
  }
  CLOSE( test);
  free( buf);
  
  return 0;
}
