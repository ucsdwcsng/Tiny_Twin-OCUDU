# Tiny-Twin-OCUDU quick start

Run all commands from `ocudu/tiny-twin-test/` unless a step says otherwise.

## Prerequisites

- Docker with the Compose v2 plugin (`docker compose`). With snap Docker, `/tmp` is private to Docker, so keep any file you mount under `$HOME`.
- The UE image `oai_ue_tinytwin:latest` must exist locally. Compose never pulls it. To build it, from `openairinterface5g/`:
  ```bash
  docker build . -f docker/Dockerfile.build.ubuntu -t ran-build
  docker build . -f docker/Dockerfile.nrUE.ubuntu  -t oai_ue_tinytwin
  ```
  `Dockerfile.build.ubuntu` needs the FlexRIC submodule. CLAUDE.md gives the one-time `git clone`.
- Compose builds the other images itself: the core (`tiny-twin-test-5gc`), the gNB (`ocudu/gnb`, `ocudu/gnb:jbpf`) and the ZMQ broker (`tiny-twin-zmq-broker`). For EdgeRIC it pulls `ghcr.io/microsoft/jrt-controller/jrt-controller-azurelinux` (pinned by digest), and `build_codelets.sh` uses `ghcr.io/microsoft/jrtc-apps/srs-jbpf-sdk:srsran25.10-latest`.
- UE channel emulation: UE 1 reads `configs/oaiue_zmq.conf` and UE 2 reads `configs/oaiue2_zmq.conf`. In each file `rx_tap_file*` is the downlink trace and `tx_tap_file*` the uplink trace. Both currently have `channel_effects_enabled = 1`, so the trace files they name must exist in `traces/`. Set the flag to `0` to run without a channel.

## 1. Single-UE system

**Runs:** `open5gs_5gc` (10.53.1.2), `ocudu_gnb` (10.53.1.3) and `ocudu_ue_tinytwin` (10.53.1.4). The gNB and UE connect directly over ZMQ.

**Build** (only after gNB code changes):
```bash
docker compose build gnb
```

**Start:**
```bash
docker compose up -d
```

**Verify:**
```bash
docker exec ocudu_ue_tinytwin ip -4 addr show oaitun_ue1            # 10.45.1.2 means attached
docker exec ocudu_ue_tinytwin ping -I oaitun_ue1 -c 5 10.45.0.1      # traffic through the core
docker compose logs ue | grep "trace:"                               # channel traces loaded
```

**Stop:**
```bash
docker compose down
```

## 2. Multi-UE system

**Runs:** everything from mode 1, plus the ZMQ broker `ocudu_zmq_broker` (10.53.1.254) and one more UE per extra user. The committed `docker-compose.2ue.yml` adds `ocudu_ue2_tinytwin` (10.53.1.5). The broker copies the downlink to every UE and adds up their uplink. Each UE keeps its own channel config.

For N UEs, generate the override. The generator also writes `open5gs/subscriber_db_<N>ue.csv` and, above 8 UEs, adds the gNB settings that need:
```bash
./gen_compose.py --ues 4          # writes docker-compose.4ue.yml; UE i uses configs/oaiue{i}_zmq.conf if it exists
```
The commands below use the 2-UE file. For N UEs, replace `docker-compose.2ue.yml` with `docker-compose.<N>ue.yml`.

**Build** (the broker image, only if missing or after changing `zmq_broker/`):
```bash
docker compose -f docker-compose.yml -f docker-compose.2ue.yml build broker
```

**Start:**
```bash
docker compose -f docker-compose.yml -f docker-compose.2ue.yml up -d
```

**Verify:**
```bash
docker logs ocudu_zmq_broker 2>&1 | grep "connected, forwarding"      # "all 2 UE(s) connected, forwarding"
docker exec ocudu_ue_tinytwin  ip -4 addr show oaitun_ue1             # 10.45.1.2
docker exec ocudu_ue2_tinytwin ip -4 addr show oaitun_ue1             # 10.45.1.3
docker exec ocudu_ue2_tinytwin ping -I oaitun_ue1 -c 5 10.45.0.1
```

**Stop:**
```bash
docker compose -f docker-compose.yml -f docker-compose.2ue.yml down
```

## 3. Multi-UE system with EdgeRIC

**Runs:** everything from mode 2, plus:
- `jrtc`, the jrt-controller, at 10.53.1.251
- `jrtc_decoder`
- `jbpf_proxy`, which loads codelets into the gNB

The decoder and proxy share `jrtc`'s network namespace. The gNB runs the jBPF build `ocudu/gnb:jbpf` and connects to `jrtc` over jBPF IPC.

The EdgeRIC apps are in `edgeric/apps/edgeric/` and the codelets in `edgeric/codelets/edgeric/`, both at the repo root:
- **`dl_sched`** attaches to the hook `mac_sched_dl_ctrl` and caps each UE's PRBs.
- **`dl_mcs`** attaches to `mac_sched_dl_mcs_ctrl` and overrides each UE's downlink MCS.

Two settings are already in place:
- **1 ms app polling:** `sleep_timeout_secs = 0.001` in both apps.
- **51 PRBs:** `EDGERIC_TOTAL_RBS: "51"` in `docker-compose.edgeric.yml`.

For a single UE, leave out `-f docker-compose.2ue.yml` below.

**Build** (once, and again after gNB or codelet changes):
```bash
docker compose -f docker-compose.yml -f docker-compose.jrtc.yml build gnb    # ocudu/gnb:jbpf (gNB + srsran_reverse_proxy)
../../edgeric/build_codelets.sh                                              # dl_sched.o, dl_mcs.o, serializers, ctypes modules
```
A good codelet build prints `Program terminates within N instructions` for each codelet.

**Start the runtime.** Compose starts `jrtc` first and waits until it's healthy, then the gNB:
```bash
F="-f docker-compose.yml -f docker-compose.2ue.yml -f docker-compose.jrtc.yml -f docker-compose.edgeric.yml"
docker compose $F up -d --no-build
```

**Verify the runtime:**
```bash
docker logs ocudu_gnb 2>&1 | grep "Registration succeeded"     # gNB connected to jrtc over jBPF IPC
docker logs jrtc 2>&1 | grep "Registered new peer"
```

**Load the scheduling and MCS apps.** `jrtc-ctl` isn't on the `PATH` in the container, so use its full path:
```bash
J='source /jrtc/setup_jrtc_env.sh && /jrtc/out/bin/jrtc-ctl'
docker exec jrtc bash -c "$J load -c /apps/edgeric/dl_sched_deployment.yaml"
docker exec jrtc bash -c "$J load -c /apps/edgeric/dl_mcs_deployment.yaml"
```

**Verify the codelets are attached:**
```bash
docker logs ocudu_gnb 2>&1 | grep "Registered codelet"
#   Registered codelet dl_sched to hook mac_sched_dl_ctrl
#   Registered codelet dl_mcs to hook mac_sched_dl_mcs_ctrl
docker exec jrtc tail -1 /tmp/edgeric_sched.log    # "... reports=... sent=... failed=0 | ue0(cqi=15,bl=...,rbs=../51) ..."
docker exec jrtc tail -1 /tmp/edgeric_mcs.log
```

**Switch policies.** Each app reads its policy file inside `jrtc` about once a second.

| File | Policies | Default |
|---|---|---|
| `/tmp/edgeric_policy` | `maxweight`, `fixed:w0,w1,...` | `maxweight` |
| `/tmp/edgeric_mcs_policy` | `passthrough`, `fixed:N`, `fixed:m0,m1,...`, `offset:K` | `passthrough` |

```bash
docker exec jrtc sh -c 'echo "fixed:20,1" > /tmp/edgeric_policy'      # PRB caps 49 (UE index 0) and 2 (UE index 1)
docker exec jrtc sh -c 'echo "fixed:5" > /tmp/edgeric_mcs_policy'     # every new DL transmission at MCS 5 (16QAM)
docker exec jrtc sh -c 'rm -f /tmp/edgeric_policy /tmp/edgeric_mcs_policy'   # back to the defaults
```

**Test that the decisions reach the scheduler.** Caps only show when a UE has more data than its cap allows, so saturate the downlink first:
```bash
docker exec -d ocudu_ue2_tinytwin iperf3 -s -1 -p 5201
docker exec -d open5gs_5gc iperf3 -c 10.45.1.3 -p 5201 -u -b 60M -t 30
```
Then check:
- **App log:** `docker exec jrtc tail -1 /tmp/edgeric_sched.log` shows `rbs=49/51 ... rbs=2/51`, and `docker exec jrtc tail -1 /tmp/edgeric_mcs.log` shows `cur_mcs=27->5`.
- **Scheduler PRB grants:** `docker exec ocudu_gnb sh -c 'grep -a "DL: ue=1 " /tmp/gnb.log | tail -3'` shows 2-RB grants, e.g. `rb=[0..2)`.
- **Scheduler MCS:** `docker exec ocudu_gnb sh -c 'grep -a "PDSCH: rnti=0x4602" /tmp/gnb.log | tail -3'` shows `mod=16QAM` instead of `256QAM`. RNTIs follow attach order: take UE index 1's `c-rnti` from the `DL: ue=1` lines above.

For a complete 2-UE check, run the automated test. It drives saturating downlink traffic to both UEs, runs `fixed:1,1`, `fixed:20,1` and MCS `fixed:5` for 15 s each, prints each UE's actual grants from the scheduler log, and resets the policies:
```bash
../../edgeric/fixed_policy_test.sh
```
Expected output: 100% of grants within each cap (26 / 26, then 49 / 2), and 16QAM/QPSK instead of 256QAM under `fixed:5`.

**Unload the apps:**
```bash
docker exec jrtc bash -c "$J unload -c /apps/edgeric/dl_mcs_deployment.yaml"
docker exec jrtc bash -c "$J unload -c /apps/edgeric/dl_sched_deployment.yaml"
```

**Stop.** Stop the gNB first so it disconnects from `jrtc` cleanly, then bring everything down:
```bash
docker compose $F stop gnb
docker compose $F down
```
Always restart the whole stack. Restarting `jrtc` under a running gNB breaks the IPC link.

**Note:** EdgeRIC caps only *reduce* the grant of a UE the scheduler has already selected. With every UE backlogged, the gNB schedules one UE per slot. So capped RBs stay unused, and a heavily capped UE can end up scheduled in most slots.
