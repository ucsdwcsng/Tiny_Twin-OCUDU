<!-- SPDX-License-Identifier: CC-BY-4.0 -->

# Record to ClickHouse

This document describes the database-backed T tracer. Unlike [`record`](./record.md), 
which writes a `.raw` file, `record_db` writes T tracer events 
directly into a [ClickHouse database](https://clickhouse.com/docs/intro).

## What `record_db` does

`record_db` connects to a running OAI softmodem instance through 
the standard T tracer TCP interface and:

- Enables the selected events
- Creates one ClickHouse table per enabled event
- Batches received rows and inserts them into ClickHouse

Each table always contains:

- `TsSwNs`: software receive timestamp created by `record_db`

If an event has both `frame` and `slot` (or `subframe`) fields, the
tool also adds:

- `TsTxUtcNs`: transmission timestamp reconstructed from frame/slot

The remaining columns come from the event definition in
`T_messages.txt`.

## Build

`record_db` pulls external dependencies (`clickhouse-cpp` and `fmtlog`).
Also make sure you have `libzstd-dev` version > 1.5.X.

From the repository using build_oai:

```shell
cd cmake_targets
./build_oai --gNB --nrUE --cmake-opt -DT_RECORD_DB=ON
```

Or, from an existing build directory using cmake:

```shell
cd cmake_targets/ran_build/build
cmake -DT_RECORD_DB=ON .
# use make or ninja
make T_tools
```

The binary is then available in:

```shell
cmake_targets/ran_build/build/common/utils/T/tracer/record_db
```

**NOTE*: Unlike other docs on T Tracer for `record_db` we are building in cmake 
directory rather than in the directory of T.

## Prerequisites

- a running ClickHouse server reachable from `record_db`
- an OAI softmodem started with T tracer enabled, typically with `--T_stdout 2`
- the tracer message description file, usually `common/utils/T/T_messages.txt`

By default `record_db` connects to:

- T tracer endpoint: `127.0.0.1:2021`
- ClickHouse native endpoint: `localhost:9000`
- ClickHouse database: `oai`

## Start the softmodem

Start the gNB or UE with T tracer enabled as described in
[basic usage](./basic.md). For example:

```shell
cd cmake_targets/ran_build/build
sudo ./nr-softmodem -O <config-file> --T_stdout 2 --T_nowait
```

Use `--T_port <port>` on the softmodem side if you want a port other
than `2021`, and pass the same value to `record_db` with `-p`.

## Basic example

To enable all events except the most verbose groups and write them to
ClickHouse:

```shell
cd cmake_targets/ran_build/build/
./common/utils/T/tracer/record_db -d ../../../common/utils/T/T_messages.txt \
  -ON \
  -off VCD \
  -off HEAVY \
  -off LEGACY_GROUP_TRACE \
  -off LEGACY_GROUP_DEBUG
```

To stop recording, press `Ctrl-C`. The tool flushes pending buffered
rows before exiting.

## Select specific events

The event selection rules are the same as for [`record`](./record.md):

- `-ON` enables all events
- `-OFF` disables all events
- `-on <GROUP or ID>` enables one group or one event
- `-off <GROUP or ID>` disables one group or one event

These options are processed in order. For example:

```shell
./common/utils/T/tracer/record_db -d ../../../common/utils/T/T_messages.txt -OFF -on GNB_PHY_UL_FD_PUSCH_IQ -on GNB_PHY_UL_PAYLOAD_RX_BITS
```

This starts from everything disabled and enables only the two listed
events.

## ClickHouse options

`record_db` accepts the following database-related options:

```shell
-ch-host <host>      ClickHouse host, default `localhost`
-ch-port <port>      ClickHouse native port, default `9000`
-ch-db <database>    database name, default `oai`
-ch-user <user>      optional ClickHouse user
-ch-pass <password>  optional ClickHouse password
-ch-droptables       drop the target database before starting
-debug               enable debug logging
```

Example using a remote ClickHouse instance:

```shell
./record_db -d ../T_messages.txt \
  -ON -off HEAVY \
  -ip 192.168.1.10 -p 2021 \
  -ch-host 192.168.1.20 -ch-port 9000 -ch-db oai_traces
```

## Inspect the data

Once `record_db` is running, connect to ClickHouse and inspect the
generated tables:

```shell
clickhouse-client --host localhost --port 9000 --query "SHOW TABLES FROM oai"
```

Count rows in one event table:

```shell
clickhouse-client --host localhost --port 9000 --query "SELECT count(*) FROM oai.GNB_PHY_UL_FD_PUSCH_IQ"
```

Inspect the latest samples:

```shell
clickhouse-client --host localhost --port 9000 --query "SELECT * FROM oai.GNB_PHY_UL_FD_PUSCH_IQ ORDER BY TsSwNs DESC LIMIT 5"
```

## Important behavior

- Tables are created only for enabled events.
- Table names match the event names from `T_messages.txt`.
- The current implementation creates tables with ClickHouse
  `ENGINE = Memory()`.
- Because of that engine choice, recorded data remains available while
  ClickHouse is running, but is not persistent across a ClickHouse
  restart.
- If `-ch-droptables` is used, the whole target database is dropped and
  recreated before capture starts.

## Get the full option list

```shell
./record_db -h
```
