#!/bin/bash

if [ -z "${ZEROVM_ROOT}" ]; then
  ZEROVM_ROOT=../../../
fi

fork_this()
{
  (python ./udp_simple_echo_server.py 127.0.0.1 9000) &> server.out 
}

fork_this &

${ZEROVM_ROOT}/zerovm ./hello_world.manifest -v 2 -t 2

kill %1

