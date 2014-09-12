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
        
#define PIPEOUT handle( "/dev/pipeout")
#define PIPEIN  handle( "/dev/pipein")
#define TEST    handle( "/dev/test")


int
main(int argc, char* argv[])
{
  char buf[128] = {0};
  char *msg1 = "This is a test...";

  //READ( STDIN, buf, 128);
  PRINTF( "[-] Beginning test...\n");
  PRINTF( "[-] Sending: [%s]\n", msg1);
  z_pwrite( PIPEOUT, msg1, STRLEN( msg1), 0);
  z_pread ( PIPEIN , buf, STRLEN( buf), 0);
  PRINTF( "[-] Recvd  : [%s]\n", buf);
  //z_pwrite( TEST, buf, STRLEN( buf), 0);
  
  return 0;
}
