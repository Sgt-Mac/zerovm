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
         

int
main(int argc, char* argv[])
{
  char buf[128] = {0};

  READ( STDIN, buf, 128);
  //z_pwrite( STDOUT, buf, STRLEN( buf), 0);
  PRINTF( buf);
  
  return 0;
}
