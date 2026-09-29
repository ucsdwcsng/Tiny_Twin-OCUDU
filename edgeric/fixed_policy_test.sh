#!/usr/bin/env bash
# Check that EdgeRIC decisions reach the gNB scheduler, with scout's fixed policies (no new policy logic).
#
# Needs the 2-UE EdgeRIC stack up with both apps loaded:
#   docker compose -f docker-compose.yml -f docker-compose.2ue.yml -f docker-compose.jrtc.yml \
#                  -f docker-compose.edgeric.yml up -d --no-build
#   jrtc-ctl load -c /apps/edgeric/dl_sched_deployment.yaml ; jrtc-ctl load -c /apps/edgeric/dl_mcs_deployment.yaml
#
# Usage: <repo>/edgeric/fixed_policy_test.sh [--duration S] [--rate R]      (from any directory)
#   --duration S  seconds per phase (default 15)
#   --rate R      iperf3 UDP DL rate per UE (default 60M; must exceed the cell's capacity so grants hit the caps)
#
# Phases (policy files read by the apps about once a second: /tmp/edgeric_policy, /tmp/edgeric_mcs_policy in jrtc):
#   A  sched fixed:1,1   mcs passthrough   equal PRB caps round(51/2) = 26 each (reference)
#   B  sched fixed:20,1  mcs passthrough   caps 49 (UE index 0) and 2 (UE index 1)
#   C  sched fixed:1,1   mcs fixed:5       every newTx PDSCH at MCS 5 (QPSK) instead of link adaptation's choice
# For each phase it reads the gNB scheduler/PHY log (/tmp/gnb.log) and prints per UE: newTx DL grants, their RB
# sizes, the share of grants within the phase's cap, the PDSCH modulation mix and the scheduled DL rate (sum of TBS).
# The policies are reset to the apps' defaults (maxweight / passthrough) at the end.

set -euo pipefail

DURATION=15
RATE=60M
while [[ $# -gt 0 ]]; do
  case "$1" in
    --duration) DURATION="$2"; shift 2 ;;
    --rate)     RATE="$2"; shift 2 ;;
    -h|--help)  sed -n '2,20p' "$0" | sed 's/^# \{0,1\}//'; exit 0 ;;
    *)          echo "Unknown option: $1" >&2; exit 1 ;;
  esac
done

log() { echo "[$(date +%H:%M:%S)] $*"; }
now() { date -u +%Y-%m-%dT%H:%M:%S.%6N; }
set_policy() { docker exec jrtc sh -c "echo '$1' > /tmp/edgeric_policy; echo '$2' > /tmp/edgeric_mcs_policy"; }

apps=$(docker exec jrtc curl -s http://127.0.0.1:3001/app | python3 -c \
  'import sys,json; print(" ".join(sorted(a.get("request",{}).get("app_name","?") for a in json.load(sys.stdin))))')
for a in dl_sched_app dl_mcs_app; do
  [[ " $apps " == *" $a "* ]] || { echo "jrtc app $a is not loaded (loaded: ${apps:-none})" >&2; exit 1; }
done
ue1=$(docker exec ocudu_ue_tinytwin ip -4 -o addr show oaitun_ue1 | awk '{print $4}' | cut -d/ -f1)
ue2=$(docker exec ocudu_ue2_tinytwin ip -4 -o addr show oaitun_ue1 | awk '{print $4}' | cut -d/ -f1)
log "UE IPs: $ue1 $ue2; apps loaded: $apps"

total=$((3 * (DURATION + 3) + 5))
for c in ocudu_ue_tinytwin ocudu_ue2_tinytwin; do docker exec -d "$c" iperf3 -s -1 -p 5201; done
sleep 1
docker exec -d open5gs_5gc iperf3 -c "$ue1" -p 5201 -u -b "$RATE" -t "$total"
docker exec -d open5gs_5gc iperf3 -c "$ue2" -p 5201 -u -b "$RATE" -t "$total"
log "DL UDP $RATE per UE for ${total}s"

windows=()
run_phase() {
  local name=$1 sched=$2 mcs=$3 caps=$4
  set_policy "$sched" "$mcs"
  sleep 3   # let both apps pick up the policy files
  local start end
  start=$(now)
  sleep "$DURATION"
  end=$(now)
  windows+=("$name|$sched|$mcs|$start|$end|$caps")
  log "phase $name: sched=$sched mcs=$mcs"
  docker exec jrtc sh -c 'tail -1 /tmp/edgeric_sched.log; tail -1 /tmp/edgeric_mcs.log' | sed 's/^/    /'
}

# caps = the PRB caps dl_sched_app derives for UE index 0,1 from the weights and EDGERIC_TOTAL_RBS=51
run_phase A "fixed:1,1" "passthrough" "26,26"
run_phase B "fixed:20,1" "passthrough" "49,2"
run_phase C "fixed:1,1" "fixed:5" "26,26"
set_policy maxweight passthrough
docker exec jrtc sh -c 'rm -f /tmp/edgeric_policy /tmp/edgeric_mcs_policy'
log "policies reset to the app defaults (maxweight / passthrough)"

docker exec ocudu_gnb cat /tmp/gnb.log | python3 "$(dirname "$0")/analyze_scheduler_log.py" "${windows[@]}"
