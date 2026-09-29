#!/bin/sh
#### 100G interface --> enp1s0f0 | pcie-address --> 01:00.0
#### 25G interface (PTP) --> enp193s0f1 | pcie-address --> c1:00.1
# SPDX-License-Identifier: MIT

set -eu

IF_NAME=ens2f1
DPDK_DEVBIND_PREFIX=/usr/local/bin

NUM_VFs=4

U_PLANE_MAC_ADD_1=00:11:22:33:44:66
C_PLANE_MAC_ADD_1=00:11:22:33:44:67
U_PLANE_MAC_ADD_2=00:11:22:33:44:68
C_PLANE_MAC_ADD_2=00:11:22:33:44:69

VLAN_1=5
VLAN_2=6

MTU=9216

U_PLANE_PCI_1=41:11.0
C_PLANE_PCI_1=41:11.1
U_PLANE_PCI_2=41:11.2
C_PLANE_PCI_2=41:11.3

ethtool -G $IF_NAME rx 8160 tx 8160
sh -c "echo 0 > /sys/class/net/$IF_NAME/device/sriov_numvfs"
sh -c "echo $NUM_VFs > /sys/class/net/$IF_NAME/device/sriov_numvfs"
modprobe -r iavf
modprobe iavf

ip link set $IF_NAME vf 0 mac $U_PLANE_MAC_ADD_1 vlan $VLAN_1 spoofchk off mtu $MTU
ip link set $IF_NAME vf 1 mac $C_PLANE_MAC_ADD_1 vlan $VLAN_1 spoofchk off mtu $MTU
ip link set $IF_NAME vf 2 mac $U_PLANE_MAC_ADD_2 vlan $VLAN_2 spoofchk off mtu $MTU
ip link set $IF_NAME vf 3 mac $C_PLANE_MAC_ADD_2 vlan $VLAN_2 spoofchk off mtu $MTU

sleep 1

${DPDK_DEVBIND_PREFIX}/dpdk-devbind.py --unbind $U_PLANE_PCI_1
${DPDK_DEVBIND_PREFIX}/dpdk-devbind.py --unbind $C_PLANE_PCI_1
${DPDK_DEVBIND_PREFIX}/dpdk-devbind.py --unbind $U_PLANE_PCI_2
${DPDK_DEVBIND_PREFIX}/dpdk-devbind.py --unbind $C_PLANE_PCI_2

modprobe vfio-pci

${DPDK_DEVBIND_PREFIX}/dpdk-devbind.py --bind vfio-pci $U_PLANE_PCI_1
${DPDK_DEVBIND_PREFIX}/dpdk-devbind.py --bind vfio-pci $C_PLANE_PCI_1
${DPDK_DEVBIND_PREFIX}/dpdk-devbind.py --bind vfio-pci $U_PLANE_PCI_2
${DPDK_DEVBIND_PREFIX}/dpdk-devbind.py --bind vfio-pci $C_PLANE_PCI_2
