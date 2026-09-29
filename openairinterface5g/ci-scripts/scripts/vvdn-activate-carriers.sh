#!/bin/sh
# SPDX-License-Identifier: MIT

set -e

RU_IP="10.10.0.111"
RU_HOST="root@10.10.0.111"

ssh_exec() {
    ssh $RU_HOST $@ </dev/null
}

echo "→ Checking for PTP fluctuations in the last 60 minutes..."

logs=$(journalctl -u ptp4l.service --since "60 minutes ago")

check_ptp=$(echo "$logs" | awk '
/rms/ {
    rms=$8
    if (rms > 100) {
        print "✗ PTP FLUCTUATION DETECTED → " $0
        exit 0
    }
}
')

if [ -n "$check_ptp" ]; then
    echo "$check_ptp"
else
    echo "✓ No PTP fluctuations detected."
fi

echo "→ Checking if VVDN LPRU is PTP synchronized (timeout 1 min)..."
if ssh_exec 'timeout 1m sh -c "tail -F /var/log/synctimingptp2.log | grep -m 1 -F ,\ synchronized"'; then
    echo "✓ VVDN LPRU synchronized"
else
    echo "✗ VVDN LPRU not synchronized"
fi

echo "→ Checking TX/RX carrier configuration..."
if ssh_exec 'sysrepocfg -X -d running -f xml | grep -q "<active>INACTIVE</active>"'; then
    echo "→ Some carriers are not active, activating via sysrepocfg..."
    ssh_exec 'sysrepocfg -X -d running -f xml | sed "s/<active>INACTIVE<\/active>/<active>ACTIVE<\/active>/g" | sysrepocfg -I -d running -f xml'
    echo "✓ All TX/RX carriers activated."
else
    echo "✓ All TX/RX carriers already activated."
fi
exit 0
