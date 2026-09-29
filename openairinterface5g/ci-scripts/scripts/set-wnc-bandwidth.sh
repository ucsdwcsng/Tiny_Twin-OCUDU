#!/bin/bash
# SPDX-License-Identifier: MIT

if [ -z "$1" ]; then
    echo "Usage: $0 <bandwidth> [reboot]"
    exit 1
fi

BANDWIDTH="$1"
DO_REBOOT="$2"
RU_IP="10.10.0.115"
HOST="root@$RU_IP"
LOGFILE="/tmp/radio_config_$(date +%Y%m%d_%H%M%S).log"

echo "=== Starting at $(date) ===" | tee "$LOGFILE"
echo "Target bandwidth: $BANDWIDTH" | tee -a "$LOGFILE"
echo "Reboot before configuration: ${DO_REBOOT:-no}" | tee -a "$LOGFILE"

if [ "$DO_REBOOT" == "reboot" ]; then
    echo "=== Rebooting RU ($RU_IP) ... ===" | tee -a "$LOGFILE"

    expect <<EOF | tee -a "$LOGFILE" > /dev/null 2>&1
set timeout 10
spawn ssh -tt $HOST

expect "#"
send "reboot\r"
expect "Continue?"
send "Y\r"
# We expect disconnect now
expect eof
EOF

    echo "→ Waiting for $RU_IP SSH to go down..." | tee -a "$LOGFILE"
    sleep 10

    echo "→ Waiting for $RU_IP SSH port 22 to be open..." | tee -a "$LOGFILE"
    while ! nc -zv $RU_IP 22 >/dev/null 2>&1; do
        echo "SSH port 22 not open, waiting 5 seconds..." | tee -a "$LOGFILE"
        sleep 5
    done
    echo "✓ SSH port 22 is open on $RU_IP" | tee -a "$LOGFILE"
fi

echo "=== Applying bandwidth configuration ===" | tee -a "$LOGFILE"

expect <<EOF | tee -a "$LOGFILE" > /dev/null 2>&1
set timeout 10
spawn ssh -tt $HOST

expect "#"
send "radio disable\r"

expect "#"
send "config\r"

expect "(config)#"
send "radio 1\r"

expect "(conf-rf 1)#"
send "bandwidth ${BANDWIDTH}\r"

expect "(conf-rf 1)#"
send "exit\r"

expect "(config)#"
send "exit\r"

expect "#"
send "radio enable\r"

expect "#"
send "show running-config\r"

expect "#"
send "exit\r"
EOF

echo "=== Checking bandwidth in running-config ===" | tee -a "$LOGFILE"

FOUND_BW=$(grep -E "bandwidth[[:space:]]+[0-9]+" "$LOGFILE" \
    | tail -n 1 \
    | awk '{print $2}' \
    | tr -d '\r[:space:]')

if [ "$FOUND_BW" -eq "$BANDWIDTH" ]; then
    echo "✓ SUCCESS: Bandwidth correctly set to $FOUND_BW" | tee -a "$LOGFILE"
    rm "$LOGFILE"
    exit 0
else
    echo "✖ ERROR: Expected bandwidth $BANDWIDTH but found: $FOUND_BW" | tee -a "$LOGFILE"
    exit 1
fi

