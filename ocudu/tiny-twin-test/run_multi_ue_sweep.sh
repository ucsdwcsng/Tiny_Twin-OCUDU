#!/usr/bin/env bash
# Run run_multi_ue.sh once per UE count with the same settings, then plot the slot timing CCDFs of all runs together.
#
# Usage: ./run_multi_ue_sweep.sh [--ues "1 4 8 16"] [--out DIR] [run_multi_ue.sh options...]
#
#   --ues LIST   UE counts to run (default "1 4 8 16")
#   --out DIR    sweep directory (default logs/multi_ue_sweep_<ts>); run N goes to DIR/<N>ue
#   Any other option is passed to every run_multi_ue.sh call. Default: --csi-period 80
#   --gnb-config configs/gnb_multi_ue.yml --duration 60 --warmup 10 --attach-timeout 240, the settings that 16 UEs
#   need (see CLAUDE.md), so every run uses the same gNB configuration.
#
# Output: DIR/<N>ue/ per run, DIR/slot_timing_ccdf.png and DIR/stats.txt comparing all runs over an equal window.

set -euo pipefail

UES="1 4 8 16"
OUT=""
RUN_ARGS=()
while [[ $# -gt 0 ]]; do
  case "$1" in
    --ues) UES="$2"; shift 2 ;;
    --out) OUT="$2"; shift 2 ;;
    -h|--help) sed -n '2,13p' "$0" | sed 's/^# \{0,1\}//'; exit 0 ;;
    *) RUN_ARGS+=("$1"); shift ;;
  esac
done
(( ${#RUN_ARGS[@]} )) || RUN_ARGS=(--csi-period 80 --gnb-config configs/gnb_multi_ue.yml --duration 60 --warmup 10
                                   --attach-timeout 240)

cd "$(dirname "$(readlink -f "$0")")"
OUT="${OUT:-logs/multi_ue_sweep_$(date +%Y%m%d_%H%M%S)}"
mkdir -p "$OUT"
PYTHON=python3
[[ -x .venv-tap-sweep/bin/python ]] && PYTHON=.venv-tap-sweep/bin/python
log() { echo "[$(date +%H:%M:%S)] $*"; }

echo "UE counts: $UES; run options: ${RUN_ARGS[*]}" | tee "$OUT/sweep.txt"
runs=()
for n in $UES; do
  log "=== $n UE(s) ==="
  if ./run_multi_ue.sh --ues "$n" "${RUN_ARGS[@]}" --out "$OUT/${n}ue" >"$OUT/${n}ue.log" 2>&1 \
      && [[ -f "$OUT/${n}ue/slot_timing.csv" && -f "$OUT/${n}ue/skip_sec" ]]; then
    runs+=("${n}ue=$OUT/${n}ue/slot_timing.csv@$(cat "$OUT/${n}ue/skip_sec")")
    log "$n UE(s) done: pings received/sent $(grep -o '[0-9]* packets transmitted, [0-9]* received' "$OUT/${n}ue/summary.txt" |
      awk '{t += $1; r += $4} END {print r "/" t}')"
  else
    log "$n UE(s) FAILED, left out of the plot; see $OUT/${n}ue.log and $OUT/${n}ue/" | tee -a "$OUT/sweep.txt"
  fi
done

(( ${#runs[@]} )) || { log "No run succeeded"; exit 1; }
log "=== comparison ==="
"$PYTHON" slot_timing_ccdf.py "${runs[@]}" --output "$OUT/slot_timing_ccdf.png" | tee "$OUT/stats.txt"
log "Results in $OUT"
