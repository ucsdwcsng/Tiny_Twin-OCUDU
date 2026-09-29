#!/usr/bin/env bash
# Run the testbed with N OAI UEs behind the ZMQ broker (zmq_broker/), check that every UE attaches and gets a working
# PDU session, ping the core from all UEs at once, and capture the gNB slot timing CSV.
#
# Usage: ./run_multi_ue.sh [--ues N] [--direct] [--csi-period MS] [--gnb-config FILE] [--duration S] [--warmup S]
#                          [--attach-timeout S] [--stop-timeout S] [--out DIR]
#
#   --ues N          number of UEs behind the broker (1-32, default 2)
#   --direct         no broker: the single UE connects straight to the gNB, as in docker-compose.yml (baseline)
#   --csi-period MS  gNB CSI-RS/report period (10, 20, 40, 80; default: gNB config, 20). Each UE needs its own CSI
#                    PUCCH slot offset; with pucch.nof_cell_csi_res = 1 a 20 ms period has room for 8 UEs.
#   --gnb-config F   extra gNB YAML merged last (e.g. prach: preamble_trans_max); copied to the output directory
#
# The compose override for the run is written to DIR/compose.override.yml. The UE channel settings come from
# configs/oaiue_zmq.conf, shared by all UEs. Summary in DIR/summary.txt, slot timing stats in DIR/stats.txt.

set -euo pipefail

NUM_UES=2
DIRECT=0
CSI_PERIOD=""
GNB_EXTRA=""
DURATION=60         # measured seconds after every UE attached + warmup
WARMUP=10           # extra seconds skipped after the last UE attached
ATTACH_TIMEOUT=240  # seconds for every UE to attach and get a PDU session
STOP_TIMEOUT=30     # grace period for the gNB to exit cleanly and write the CSV
OUT=""

usage() { sed -n '2,15p' "$0" | sed 's/^# \{0,1\}//'; exit "${1:-0}"; }

while [[ $# -gt 0 ]]; do
  case "$1" in
    --ues)            NUM_UES="$2"; shift 2 ;;
    --direct)         DIRECT=1; shift ;;
    --csi-period)     CSI_PERIOD="$2"; shift 2 ;;
    --gnb-config)     GNB_EXTRA="$2"; shift 2 ;;
    --duration)       DURATION="$2"; shift 2 ;;
    --warmup)         WARMUP="$2"; shift 2 ;;
    --attach-timeout) ATTACH_TIMEOUT="$2"; shift 2 ;;
    --stop-timeout)   STOP_TIMEOUT="$2"; shift 2 ;;
    --out)            OUT="$2"; shift 2 ;;
    -h|--help)        usage 0 ;;
    *)                echo "Unknown option: $1" >&2; usage 1 ;;
  esac
done
if (( DIRECT )); then
  NUM_UES=1
elif (( NUM_UES < 1 || NUM_UES > 32 )); then
  echo "--ues must be 1-32" >&2
  exit 1
fi
if [[ -n "$CSI_PERIOD" && ! "$CSI_PERIOD" =~ ^(10|20|40|80)$ ]]; then
  echo "--csi-period must be 10, 20, 40 or 80" >&2
  exit 1
fi
if (( DIRECT )) && [[ -n "$CSI_PERIOD$GNB_EXTRA" ]]; then
  echo "--csi-period and --gnb-config are only applied with the broker" >&2
  exit 1
fi
if [[ -n "$GNB_EXTRA" && ! -f "$GNB_EXTRA" ]]; then
  echo "--gnb-config: $GNB_EXTRA not found" >&2
  exit 1
fi
[[ -n "$GNB_EXTRA" ]] && GNB_EXTRA=$(readlink -f "$GNB_EXTRA")

cd "$(dirname "$(readlink -f "$0")")"
mode=$([[ $DIRECT == 1 ]] && echo direct || echo "broker_${NUM_UES}ue${CSI_PERIOD:+_csi$CSI_PERIOD}")
OUT="${OUT:-logs/multi_ue_$(date +%Y%m%d_%H%M%S)_$mode}"
mkdir -p "$OUT"
OUT=$(readlink -f "$OUT")
GNB_CONTAINER=ocudu_gnb
CSV_IN_CONTAINER=/tmp/slot_timing.csv
BROKER_IP=10.53.1.254  # outside the UE range 10.53.1.4..
CORE_GW=10.45.0.1
PYTHON=python3
[[ -x .venv-tap-sweep/bin/python ]] && PYTHON=.venv-tap-sweep/bin/python

log() { echo "[$(date +%H:%M:%S)] $*"; }

# UE i (1-based): compose service, container, IP, IMSI, PDU session IP.
ue_service()   { (( $1 == 1 )) && echo ue || echo "ue$1"; }
ue_container() { (( $1 == 1 )) && echo ocudu_ue_tinytwin || echo "ocudu_ue${1}_tinytwin"; }
ue_ip()        { echo "10.53.1.$((3 + $1))"; }
ue_imsi()      { printf '00101%010d' $((123456780 + $1 - 1)); }

ue_command() {  # compose command list for UE $1 behind the broker
  cat <<EOF
    command:
      - -O
      - /oaie_zmq.conf
      - -C
      - "3489420000"
      - -r
      - "51"
      - --numerology
      - "1"
      - --band
      - "78"
      - -E
      - --ue-scan-carrier
      - --uicc0.imsi
      - "$(ue_imsi "$1")"
      - --zmq.[0].rx_channels
      - tcp://$BROKER_IP:$((4999 + $1))
EOF
}

write_override() {
  local f=$OUT/compose.override.yml
  {
    echo "name: tiny-twin-test"
    echo "services:"
    echo "  5gc:"
    echo "    environment:"
    echo "      SUBSCRIBER_DB: /open5gs/subscriber_db.csv"
    echo "    volumes:"
    echo "      - $OUT/subscriber_db.csv:/open5gs/subscriber_db.csv:ro"
    echo "  gnb:"
    echo "    configs:"
    echo "      - gnb_config.yml"
    echo "      - gnb_compose_config.yml"
    echo "      - gnb_broker_config.yml"
    [[ -n "$GNB_EXTRA" ]] && echo "      - gnb_extra_config.yml"
    echo "    command: gnb -c /gnb_config.yml -c /gnb_compose_config.yml -c /gnb_broker_config.yml${GNB_EXTRA:+ -c /gnb_extra_config.yml}"
    echo "  broker:"
    echo "    container_name: ocudu_zmq_broker"
    echo "    image: tiny-twin-zmq-broker"
    echo "    pull_policy: never"
    echo "    depends_on:"
    echo "      gnb:"
    echo "        condition: service_started"
    echo "    networks:"
    echo "      ran:"
    echo "        ipv4_address: $BROKER_IP"
    echo "    command:"
    echo "      - --gnb-tx"
    echo "      - tcp://10.53.1.3:4556"
    echo "      - --gnb-rx"
    echo "      - tcp://0.0.0.0:4557"
    for ((i = 1; i <= NUM_UES; i++)); do
      echo "      - --ue"
      echo "      - tcp://0.0.0.0:$((4999 + i)),tcp://$(ue_ip "$i"):4557"
    done
    for ((i = 1; i <= NUM_UES; i++)); do
      echo "  $(ue_service "$i"):"
      if (( i > 1 )); then
        echo "    extends:"
        echo "      file: $PWD/docker-compose.yml"
        echo "      service: ue"
        echo "    container_name: $(ue_container "$i")"
        echo "    depends_on:"
        echo "      broker:"
        echo "        condition: service_started"
        echo "    networks:"
        echo "      ran:"
        echo "        ipv4_address: $(ue_ip "$i")"
      fi
      ue_command "$i"
    done
    echo "configs:"
    echo "  gnb_broker_config.yml:"
    echo "    content: |"
    echo "      ru_sdr:"
    echo "        device_args: tx_port=tcp://0.0.0.0:4556,rx_port=tcp://$BROKER_IP:4557,base_srate=23.04e6"
    if [[ -n "$CSI_PERIOD" ]]; then
      echo "      cell_cfg:"
      echo "        csi:"
      echo "          csi_rs_period: $CSI_PERIOD"
    fi
    if [[ -n "$GNB_EXTRA" ]]; then
      cp "$GNB_EXTRA" "$OUT/gnb_extra_config.yml"
      echo "  gnb_extra_config.yml:"
      echo "    file: $OUT/gnb_extra_config.yml"
    fi
  } >"$f"

  {
    echo "# Kept in the format of open5gs/subscriber_db.csv"
    for ((i = 1; i <= NUM_UES; i++)); do
      echo "ue$i,$(ue_imsi "$i"),00112233445566778899aabbccddeeff,opc,63bfa50ee6523365ff14c1f45f88737d,8000,9,10.45.1.$((1 + i))"
    done
  } >"$OUT/subscriber_db.csv"
}

if (( DIRECT )); then
  compose() { docker compose -f docker-compose.yml "$@"; }
else
  log "Building the broker image"
  docker build -q -t tiny-twin-zmq-broker ../../zmq_broker >/dev/null
  write_override
  compose() { docker compose -f docker-compose.yml -f "$OUT/compose.override.yml" "$@"; }
fi

cleanup() { compose down --remove-orphans >/dev/null 2>&1 || true; }
trap cleanup EXIT

save_logs() {
  compose logs --no-color gnb >"$OUT/gnb_stdout.log" 2>&1 || true
  docker cp "$GNB_CONTAINER:/tmp/gnb.log" "$OUT/gnb.log" >/dev/null 2>&1 || true
  (( DIRECT )) || compose logs --no-color broker >"$OUT/broker.log" 2>&1 || true
  for ((i = 1; i <= NUM_UES; i++)); do
    compose logs --no-color "$(ue_service "$i")" >"$OUT/ue${i}.log" 2>&1 || true
  done
}

log "=== $mode: $NUM_UES UE(s), output in $OUT ==="
cp configs/oaiue_zmq.conf "$OUT/"
compose down --remove-orphans >/dev/null 2>&1 || true
compose up -d --no-build
t0=$SECONDS
# The CSV lives in the persistent gnb-storage volume; drop any file left by an earlier run.
docker exec "$GNB_CONTAINER" rm -f "$CSV_IN_CONTAINER" || true

declare -A attach_sec=()
log "Waiting for every UE to attach and get a PDU session..."
while (( ${#attach_sec[@]} < NUM_UES )); do
  for ((i = 1; i <= NUM_UES; i++)); do
    [[ -n "${attach_sec[$i]:-}" ]] && continue
    if docker exec "$(ue_container "$i")" ip -4 -o addr show oaitun_ue1 2>/dev/null | grep -q inet; then
      attach_sec[$i]=$((SECONDS - t0))
      log "UE $i ($(ue_imsi "$i")) has $(docker exec "$(ue_container "$i")" ip -4 -o addr show oaitun_ue1 |
        awk '{print $4}') after ${attach_sec[$i]}s"
    fi
  done
  if (( SECONDS - t0 > ATTACH_TIMEOUT )); then
    save_logs
    log "Only ${#attach_sec[@]}/$NUM_UES UE(s) attached within ${ATTACH_TIMEOUT}s; logs in $OUT"
    exit 1
  fi
  sleep 2
done
last_attach=0
for s in "${attach_sec[@]}"; do (( s > last_attach )) && last_attach=$s; done

log "All UEs attached; warmup ${WARMUP}s, then ${DURATION}s of concurrent pings from every UE"
sleep "$WARMUP"
pids=()
for ((i = 1; i <= NUM_UES; i++)); do
  docker exec "$(ue_container "$i")" ping -I oaitun_ue1 -i 0.2 -c $((DURATION * 5)) -w $((DURATION + 10)) "$CORE_GW" >"$OUT/ping_ue${i}.txt" 2>&1 &
  pids+=($!)
done
for p in "${pids[@]}"; do wait "$p" || true; done

# Stop the gNB first, while the broker and UEs still feed it samples, so it shuts down cleanly and writes the CSV.
compose stop -t "$STOP_TIMEOUT" gnb
save_logs
csv_ok=1
if ! docker cp "$GNB_CONTAINER:$CSV_IN_CONTAINER" "$OUT/slot_timing.csv"; then
  log "Could not copy the slot timing CSV; the gNB may not have exited cleanly (try a larger --stop-timeout)"
  csv_ok=0
fi

{
  echo "mode: $mode, UEs: $NUM_UES, CSI period: ${CSI_PERIOD:-gNB default}, extra gNB config: ${GNB_EXTRA:-none}, measured ${DURATION}s after warmup ${WARMUP}s"
  for ((i = 1; i <= NUM_UES; i++)); do
    stats=$(grep -E 'packets transmitted|rtt' "$OUT/ping_ue${i}.txt" | tr '\n' ' ')
    echo "UE $i ($(ue_imsi "$i")): attached after ${attach_sec[$i]}s; ping: ${stats:-no ping output}"
  done
  if (( !DIRECT )); then
    echo "broker (last stats line):"
    grep 'Msps' "$OUT/broker.log" | tail -1 | sed 's/^[^|]*| //'
  fi
} | tee "$OUT/summary.txt"

if (( csv_ok )); then
  skip=$((last_attach + WARMUP))
  echo "$skip" >"$OUT/skip_sec"  # seconds of the CSV before the measured window, for run_multi_ue_sweep.sh
  log "=== slot timing (skipping first ${skip}s) ==="
  "$PYTHON" slot_timing_ccdf.py "$mode=$OUT/slot_timing.csv@$skip" \
    --output "$OUT/slot_timing_ccdf.png" | tee "$OUT/stats.txt"
fi
log "Results in $OUT"
