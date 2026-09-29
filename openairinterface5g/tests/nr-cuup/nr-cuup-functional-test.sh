#!/bin/bash
# SPDX-License-Identifier: LicenseRef-CSSL-1.0

# this script takes one argument, which is the path to the CU-UP configuration
if [ $# -ne 1 ]; then
  echo "usage: $0 <cuup-config>"
  exit 1
fi

CONFIG=$1

set -x

# current dir should be CMAKE_BINARY_DIR
./nr-cuup -O ${CONFIG} &
CUUP_PID=$!

timeout 7s ./tests/nr-cuup/nr-cuup-load-test -t 3 -d 10 -u 10
RET=$?

kill ${CUUP_PID}
exit ${RET}
