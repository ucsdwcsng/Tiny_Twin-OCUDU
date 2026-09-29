#!/bin/sh
# SPDX-License-Identifier: MIT

set -eu

pci_addr()
{
	PF_IF=$1
	VF_INDEX=$2
	SYSFS_PATH="/sys/class/net/${PF_IF}/device/virtfn${VF_INDEX}"

	if [ ! -e "$SYSFS_PATH" ]; then
		    echo "VF $VF_INDEX not found for interface $PF_IF"
		        exit 1
	fi
	PCI_ADDR=$(basename "$(readlink "$SYSFS_PATH")")
	echo "$PCI_ADDR"
}
IF_NAME=ens7f1
## O-DU C Plane MAC ADDR and VLAN
C_U_PLANE_MAC_ADD=76:76:64:6e:00:01
C_U_PLANE_VLAN=4
MTU=9000
DPDK_DEVBIND_PREFIX=/usr/local/bin
NUM_VFs=1
ethtool -G $IF_NAME rx 8160 tx 8160
sh -c "echo 0 > /sys/class/net/$IF_NAME/device/sriov_numvfs"
sh -c "echo $NUM_VFs > /sys/class/net/$IF_NAME/device/sriov_numvfs"
C_U_PLANE_PCI=$(pci_addr $IF_NAME 0)
# this next 2 lines is for C/U planes
ip link set $IF_NAME vf 0 mac $C_U_PLANE_MAC_ADD vlan $C_U_PLANE_VLAN spoofchk off mtu $MTU
sleep 1
modprobe iavf
${DPDK_DEVBIND_PREFIX}/dpdk-devbind.py --unbind $C_U_PLANE_PCI
modprobe vfio-pci
${DPDK_DEVBIND_PREFIX}/dpdk-devbind.py --bind vfio-pci $C_U_PLANE_PCI
echo "Successfully configured C-PLANE and U-PLANE:
  - C-PLANE MAC: $C_U_PLANE_MAC_ADD, VLAN: $C_U_PLANE_VLAN, PCI: $C_U_PLANE_PCI"
exit 0
