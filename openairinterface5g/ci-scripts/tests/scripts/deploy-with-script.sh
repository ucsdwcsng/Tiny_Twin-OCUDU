#!/bin/bash
# SPDX-License-Identifier: MIT

function die() { echo $@; exit 1; }

echo "Deploy script executed with options: $@"

[ $# -lt 2 ] && die "failing"

exit 0
