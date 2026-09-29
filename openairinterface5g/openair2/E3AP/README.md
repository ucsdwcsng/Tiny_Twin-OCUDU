<!-- SPDX-License-Identifier: CC-BY-4.0 -->

# Overview

This document describes the E3 Agent for dApps integrated in OAI gNB, and explains how to
build, configure, and connect a dApp.

[[_TOC_]]

dApps can be real-time microservices co-located with the RAN node (DU or CU). Unlike xApps,
which run on a nearRT-RIC and operate at 10 ms–1 s timescales, dApps execute directly on the
RAN node and interact with user-plane data that is unavailable to RICs due to timing or
privacy constraints. This enables sub-millisecond control loops for use cases such as spectrum
sharing, positioning, and scheduling optimization.

The E3 interface connects the gNB to co-located dApps. The `libe3` library implements both
sides of this interface — the RAN agent (this module) and the dApp client — using a unified
C++/Python API that hides transport, encoding, and protocol complexity.

For a complete description of the dApp architecture and the E3 interface, refer to:

- [Paper: dApp architecture and E3 interface (Computer Networks, 2025)](https://doi.org/10.1016/j.comnet.2025.111342)
- [Tutorial: deploying dApps with OAI](https://openrangym.com/tutorials/dapps-oai)
- [dApp Python library](https://pypi.org/project/dapps/)

## 1. Prerequisites — libe3

The E3 Agent requires the external `libe3` library, discovered at build time via
`pkg_check_modules(CLIBE3 REQUIRED libe3)`. It is not vendored in OAI.

```bash
git clone https://github.com/wineslab/libe3 && cd libe3
./build_libe3 -I       # install system prerequisites if not already present
./build_libe3 --install
```

## 2. Build OAI with the E3 Agent

The E3 Agent is **opt-in** and gated behind the `E3_AGENT` CMake option (`OFF` by default).
The default OAI build is entirely unaffected when `E3_AGENT` is not set.

```bash
cd cmake_targets/
./build_oai -w SIMU --ninja --gNB --nrUE --build-e3
```

To build without the E3 Agent, omit `--build-e3`. To clean and rebuild, add the `-c` flag.

## 3. Configuration

The agent reads an optional `E3Configuration` block from the gNB configuration file.
If the block is absent, the agent falls back to built-in defaults (`posix`/`ipc`).

```
E3Configuration : {
    link      = "zmq";   # posix | zmq
    transport = "ipc";   # tcp | sctp | ipc
};
```

The same parameters can be set on the command line without editing the config file:

```bash
sudo ./nr-softmodem -O <gnb.conf> ... --E3Configuration.link zmq --E3Configuration.transport ipc
```

Valid link/transport combinations:

| link  | transport |
|-------|-----------|
| zmq   | ipc       |
| zmq   | tcp       |
| posix | ipc       |
| posix | tcp       |
| posix | sctp      |

## 4. dApp development

dApps are written in Python using the `dapps` library:

```bash
pip install "dapps[all]"
```

A dApp subclasses the `DApp` base class and implements service-model-specific encode/decode
and a control loop. The library handles the E3 connection transparently: setup handshake,
subscription, indication receive, and control send.

At the current status, no service models have been included so far.

For a full end-to-end example including service model usage, follow the
[tutorial on the OpenRAN Gym website](https://openrangym.com/tutorials/dapps-oai).

## 5. Start the process

### 5.1 Start the gNB with E3 agent enabled

The example below uses RFsim for local testing. A 5G Core Network must already be running;
see [`doc/NR_SA_Tutorial_OAI_CN5G.md`](../../doc/NR_SA_Tutorial_OAI_CN5G.md).

```bash
cd cmake_targets/ran_build/build
sudo ./nr-softmodem \
  -O <path>/targets/PROJECTS/GENERIC-NR-5GC/CONF/gnb.sa.band78.fr1.106PRB.pci0.rfsim.conf \
  --rfsim \
  --E3Configuration.link zmq --E3Configuration.transport ipc
```

If the agent initializes correctly, the log will show `Init E3 Agent` and the setup socket
will appear at `/tmp/dapps/setup`.

An `E3AP` log component is available for debugging: pass `--log_config.e3ap_log_level debug`
to increase verbosity.

### 5.2 Connect a dApp

Follow the [deployment tutorial](https://openrangym.com/tutorials/dapps-oai) for a complete
end-to-end example.
