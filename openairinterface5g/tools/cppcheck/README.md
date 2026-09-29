<!-- SPDX-License-Identifier: CC-BY-4.0 -->

# Static code analysis

How to run static code analysis tool(s).

[[_TOC_]]

## cppcheck Overview

This can run a (dockerized) cppcheck static code analysis. The docker-compose
file represents the status as run as in the CI until it's removal (because it
was not enforced). It is now here so that people could use it to find bugs (and
use the tool more easily).

## cppcheck Usage

From the openairinterface5g root, simply run

    ./tools/cppcheck/run-cppcheck.sh

This will run cppcheck with options listed in this file, assuming you have it
installed. Alternatively, you can run the CI version as of Ubuntu 20 with

    cd tools/cppcheck && docker compose up --no-log-prefix

You can upgrade to a newer version by modifying the `FROM` line in the
docker-compose file.
