#!/bin/bash

if [ -z "${ZEROVM_ROOT}" ]; then
  ZEROVM_ROOT=../../../
fi

${ZEROVM_ROOT}/zerovm ./hello_world.manifest -v 2 -t 2
