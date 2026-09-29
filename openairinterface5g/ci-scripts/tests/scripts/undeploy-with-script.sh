#!/bin/bash
# SPDX-License-Identifier: MIT

function die() { echo $@; exit 1; }

echo "Undeploy script executed with options: $@"

LOG_DIR="$1"
mkdir -p "$LOG_DIR"
echo "Undeployment started" > "$LOG_DIR/log1.txt"
echo "Undeployment finished successfully" > "$LOG_DIR/log2.txt"

[ $# -lt 3 ] && die "failing"

exit 0
