#!/bin/bash
## Copyright (c) Microsoft Corporation. All rights reserved.
# shellcheck disable=SC1091

set -x 

SUBM=$1

HERE="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)"

pushd .
cd "$HERE" || exit 1

pushd .
cd ../../
sudo docker build -f $HERE/Dockerfile  --no-cache --force-rm -t srsran-jbpf .
popd || exit 1

popd || exit 1

exit 0

