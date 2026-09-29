#!/usr/bin/env bash
# Run the testbed twice (UE channel effects off, then on), capture the gNB slot timing CSV of each run and compare
# them with slot_timing_ccdf.py.
#
# Usage: ./run_tti_ccdf.sh [--duration S] [--warmup S] [--attach-timeout S] [--stop-timeout S] [--out DIR]
#
# Requires the ocudu/gnb image built with the MAC slot timing recorder (docker compose build gnb) and the
# oai_ue_tinytwin image. configs/oaiue_zmq.conf is restored to its original content on exit.

set -euo pipefail

DURATION=120        # measured seconds after UE attach + warmup
WARMUP=10           # extra seconds skipped after UE attach
ATTACH_TIMEOUT=180  # seconds to wait for the first "DL Chan:" UE report
STOP_TIMEOUT=30     # grace period for the gNB to exit cleanly and write the CSV
OUT=""

usage() { sed -n '2,8p' "$0" | sed 's/^# \{0,1\}//'; exit "${1:-0}"; }

while [[ $# -gt 0 ]]; do
  case "$1" in
    --duration)       DURATION="$2"; shift 2 ;;
    --warmup)         WARMUP="$2"; shift 2 ;;
    --attach-timeout) ATTACH_TIMEOUT="$2"; shift 2 ;;
    --stop-timeout)   STOP_TIMEOUT="$2"; shift 2 ;;
    --out)            OUT="$2"; shift 2 ;;
    -h|--help)        usage 0 ;;
    *)                echo "Unknown option: $1" >&2; usage 1 ;;
  esac
done

cd "$(dirname "$(readlink -f "$0")")"
CONFIG=configs/oaiue_zmq.conf
GNB_CONTAINER=ocudu_gnb
CSV_IN_CONTAINER=/tmp/slot_timing.csv
OUT="${OUT:-logs/tti_ccdf_$(date +%Y%m%d_%H%M%S)}"
PYTHON=python3
[[ -x .venv-tap-sweep/bin/python ]] && PYTHON=.venv-tap-sweep/bin/python

compose() { docker compose -f docker-compose.yml "$@"; }
log() { echo "[$(date +%H:%M:%S)] $*"; }

EFFECTS_RE='^([[:space:]]*channel_effects_enabled[[:space:]]*=[[:space:]]*)[^;]*;'
if [[ $(grep -cE "$EFFECTS_RE" "$CONFIG") -ne 1 ]]; then
  echo "$CONFIG must contain exactly one 'channel_effects_enabled = ...;' line" >&2
  exit 1
fi

mkdir -p "$OUT"
cp "$CONFIG" "$OUT/oaiue_zmq.conf.orig"
restore() {
  cp "$OUT/oaiue_zmq.conf.orig" "$CONFIG"
  compose down --remove-orphans >/dev/null 2>&1 || true
}
trap restore EXIT

# The UE consumes one RX trace row per slot read; at EOF the channel becomes identity. The slot rate is only known
# after the run, so the check against the recorded slots happens after the channel-on run.
trace_rows=0
trace=$(sed -nE 's/^[[:space:]]*rx_tap_file[[:space:]]*=[[:space:]]*"([^"]*)".*/\1/p' "$CONFIG" | head -1)
if [[ -n "$trace" && -f "traces/${trace#/traces/}" ]]; then
  trace_rows=$(grep -c . "traces/${trace#/traces/}")
  log "RX trace traces/${trace#/traces/}: $trace_rows rows (one row per slot)"
fi

check_trace_coverage() {
  local csv=$1
  (( trace_rows > 0 )) || return 0
  local slots=$(($(wc -l <"$csv") - 1))
  if (( slots > trace_rows )); then
    local eof_sec=$(awk -F, -v n="$trace_rows" 'NR == 2 { t0 = $2 } NR == n + 1 { printf "%.1f", ($2 - t0) / 1e9 }' "$csv")
    log "WARNING: the channel-on run recorded $slots slots but the trace has $trace_rows rows: the trace ended about" \
        "${eof_sec}s after gNB start and the channel was identity afterwards. Use a longer trace for a full window."
  fi
}

attach_sec=()

run_one() {
  local mode=$1 value=$2
  log "=== channel $mode (channel_effects_enabled = $value) ==="
  sed -E -i "s/$EFFECTS_RE/\1$value;/" "$CONFIG"

  compose down --remove-orphans >/dev/null 2>&1 || true
  compose up -d --no-build
  local t0=$SECONDS
  # The CSV lives in the persistent gnb-storage volume; drop any file left by an earlier run.
  docker exec "$GNB_CONTAINER" rm -f "$CSV_IN_CONTAINER" || true

  log "Waiting for UE attach (first DL Chan report)..."
  until compose logs ue 2>/dev/null | grep -q 'DL Chan:'; do
    if (( SECONDS - t0 > ATTACH_TIMEOUT )); then
      compose logs ue >"$OUT/${mode}_ue.log" 2>&1 || true
      log "UE did not attach within ${ATTACH_TIMEOUT}s; see $OUT/${mode}_ue.log"
      exit 1
    fi
    sleep 2
  done
  attach_sec+=($((SECONDS - t0)))
  log "UE attached after ${attach_sec[-1]}s; measuring for $((WARMUP + DURATION))s"
  sleep $((WARMUP + DURATION))

  # Stop the gNB first, while the UE still feeds it samples, so it shuts down cleanly and writes the CSV.
  compose stop -t "$STOP_TIMEOUT" gnb
  compose logs ue >"$OUT/${mode}_ue.log" 2>&1 || true
  docker cp "$GNB_CONTAINER:/tmp/gnb.log" "$OUT/${mode}_gnb.log" 2>/dev/null || true
  if ! docker cp "$GNB_CONTAINER:$CSV_IN_CONTAINER" "$OUT/slot_timing_${mode}.csv"; then
    log "Could not copy the slot timing CSV (see the docker error above). If the file is missing, the gNB did not" \
        "exit cleanly: try a larger --stop-timeout."
    exit 1
  fi
  log "Saved $OUT/slot_timing_${mode}.csv ($(($(wc -l <"$OUT/slot_timing_${mode}.csv") - 1)) slots)"
  [[ $mode == on ]] && check_trace_coverage "$OUT/slot_timing_${mode}.csv"
  compose down --remove-orphans >/dev/null
}

run_one off 0
run_one on 1

# Skip each run's own attach time plus the warmup: the CSV starts at gNB start, the UE attaches later.
skip_off=$((attach_sec[0] + WARMUP))
skip_on=$((attach_sec[1] + WARMUP))
log "=== analysis (skipping first ${skip_off}s of off, ${skip_on}s of on) ==="
"$PYTHON" slot_timing_ccdf.py off="$OUT/slot_timing_off.csv@$skip_off" on="$OUT/slot_timing_on.csv@$skip_on" \
  --output "$OUT/slot_timing_ccdf.png" | tee "$OUT/stats.txt"
log "Results in $OUT"
