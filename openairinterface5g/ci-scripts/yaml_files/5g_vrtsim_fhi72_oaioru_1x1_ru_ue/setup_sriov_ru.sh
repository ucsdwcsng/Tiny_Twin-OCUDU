#!/bin/sh
# SPDX-License-Identifier: MIT

set -x
IF_NAME=enp1s0f0np0
NUM_VFs=1
C_U_PLANE_MAC_ADD=00:11:22:33:aa:67
VLAN=30
MTU=9000
C_U_PLANE_PCI=01:01.0
DPDK_DEVBIND_PREFIX=/usr/local/bin
ethtool -G $IF_NAME rx 8160 tx 8160
sh -c "echo 0 > /sys/class/net/$IF_NAME/device/sriov_numvfs"
sh -c "echo $NUM_VFs > /sys/class/net/$IF_NAME/device/sriov_numvfs"
modprobe -r iavf
modprobe iavf
# this next 1 lines is for C/U planes
ip link set $IF_NAME vf 0 mac $C_U_PLANE_MAC_ADD vlan $VLAN spoofchk off mtu $MTU
sleep 1
${DPDK_DEVBIND_PREFIX}/dpdk-devbind.py --unbind $C_U_PLANE_PCI
modprobe vfio-pci
${DPDK_DEVBIND_PREFIX}/dpdk-devbind.py --bind vfio-pci $C_U_PLANE_PCI
exit 0
