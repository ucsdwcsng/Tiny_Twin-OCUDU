#!/usr/bin/env bash

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


set -o pipefail

if [ -n "$RETINA_PORTS" ]; then
  # In this mode, we expect to receive data over UDP, telling websocket ip/port of the server.
  export WS_URL=$(socat -u UDP-RECVFROM:"${RETINA_PORTS}",reuseaddr STDOUT)
fi
telegraf --config /etc/ocudu/telegraf.conf $TELEGRAF_CLI_EXTRA_ARGS &
child=$!

health_code=0
_term() {
    curl -sf -o /dev/null http://localhost:9273/health
    health_code=$?
    echo "$(date -u '+%Y-%m-%dT%H:%M:%SZ') I! Health check returned code $health_code"
    kill -TERM "$child" 2>/dev/null
}
trap _term SIGTERM SIGINT

wait "$child"

exit $health_code
