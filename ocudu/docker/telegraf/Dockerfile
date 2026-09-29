# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


ARG TELEGRAF_VERSION=1.35.0
FROM telegraf:${TELEGRAF_VERSION}
RUN DEBIAN_FRONTEND=noninteractive apt-get update && apt-get install -y --no-install-recommends python3-websocket socat

ADD *.conf /etc/ocudu/
ADD entrypoint.sh ws_adapter.py /usr/local/bin/

ENTRYPOINT [ "/usr/local/bin/entrypoint.sh" ]
