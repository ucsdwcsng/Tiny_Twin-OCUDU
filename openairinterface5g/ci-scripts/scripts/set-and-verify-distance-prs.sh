#!/bin/bash
# SPDX-License-Identifier: MIT

[[ $# -lt 1 ]] && { echo "Usage: $0 <distance_in_meters...>"; exit 1; }
DISTANCES=("$@")

IP=192.168.71.150
PORT=8091
SLEEP_WAIT=4 #s
set_and_verify_distance() {
  local distance=$1
  echo "Testing PRS ToA estimation for distance: $distance m"

  local setdist_resp="$(grep --max-count 1 new_offset <(echo rfsimu setdistance rfsimu_channel_enB0 $distance | ncat ${IP} ${PORT}))"
  echo "> response: ${setdist_resp}"

  local gettoa_resp="$(echo "ciUE get_max_dl_toa" | ncat ${IP} ${PORT} | grep "UE max PRS DL ToA")"
  echo "> response: ${gettoa_resp}"

  [[ -z "$setdist_resp" || -z "$gettoa_resp" ]] && return 1

  # Extract ToA values 
  [[ "$setdist_resp" =~ new_offset\ ([0-9]+) ]] && local set_toa="${BASH_REMATCH[1]}" || { echo "> Set ToA extraction failed for distance: $distance m"; return 1; }

  [[ "$gettoa_resp" =~ UE\ max\ PRS\ DL\ ToA\ ([0-9]+) ]] && local est_toa="${BASH_REMATCH[1]}" || { echo "> Estimated ToA extraction failed for distance: $distance m"; return 1; }

  # Compare extracted ToA values
  [[ $set_toa == $est_toa ]] && echo "PRS SUCCESS for distance: $distance m" || { echo "PRS FAILURE for distance: $distance m (Actual ToA=$set_toa, Estimated ToA=$est_toa)" ; return 1; }

}

test_distance() {
  local distance=$1

    # Always reset to 0 m before testing target distance
  if ! set_and_verify_distance 0; then
    return 1
  fi

  sleep "$SLEEP_WAIT"

  if set_and_verify_distance "$distance"; then
    return 0
  fi
}

num_fail=0

for d in "${DISTANCES[@]}"; do
  if ! test_distance "$d"; then
    ((num_fail++))
  fi
  sleep "$SLEEP_WAIT"
done

if [ $num_fail -gt 0 ]; then
  exit 1
fi

exit 0
