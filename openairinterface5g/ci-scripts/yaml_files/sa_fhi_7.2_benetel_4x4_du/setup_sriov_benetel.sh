#!/bin/sh
# SPDX-License-Identifier: MIT

set -eu

IF_NAME=ens2f1
NUM_VFs=2
U_PLANE_MAC_ADD=00:11:22:33:44:66
C_PLANE_MAC_ADD=00:11:22:33:44:67
VLAN=9
MTU=9216
U_PLANE_PCI=41:11.0
C_PLANE_PCI=41:11.1
## It will be something like this --> $DPDK_INST/bin
DPDK_DEVBIND_PREFIX=/usr/local/bin

ethtool -G $IF_NAME rx 8160 tx 8160
sh -c "echo 0 > /sys/class/net/$IF_NAME/device/sriov_numvfs"
sh -c "echo $NUM_VFs > /sys/class/net/$IF_NAME/device/sriov_numvfs"
modprobe -r iavf
modprobe iavf
# this next 2 lines is for C/U planes
ip link set $IF_NAME vf 0 mac $U_PLANE_MAC_ADD vlan $VLAN spoofchk off mtu $MTU
ip link set $IF_NAME vf 1 mac $C_PLANE_MAC_ADD vlan $VLAN spoofchk off mtu $MTU
sleep 1
${DPDK_DEVBIND_PREFIX}/dpdk-devbind.py --unbind $U_PLANE_PCI
${DPDK_DEVBIND_PREFIX}/dpdk-devbind.py --unbind $C_PLANE_PCI
modprobe vfio-pci
${DPDK_DEVBIND_PREFIX}/dpdk-devbind.py --bind vfio-pci $U_PLANE_PCI
${DPDK_DEVBIND_PREFIX}/dpdk-devbind.py --bind vfio-pci $C_PLANE_PCI

