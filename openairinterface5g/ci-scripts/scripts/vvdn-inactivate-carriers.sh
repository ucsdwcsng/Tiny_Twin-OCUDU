#!/bin/sh
# SPDX-License-Identifier: MIT

set -e

RU_HOST="root@10.10.0.111"

ssh_exec() {
    ssh $RU_HOST $@ </dev/null
}

echo "→ Inactivating TX/RX carriers on VVDN LPRU..."
ssh_exec 'sysrepocfg -X -d running -f xml | sed "s/ACTIVE/INACTIVE/g" | sysrepocfg -I -d running -f xml'
echo "✓ TX/RX carriers inactivated."
