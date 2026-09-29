/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef __ETHERNET_USERSPACE_LIB_IF_DEFS__H__
#define __ETHERNET_USERSPACE_LIB_IF_DEFS__H__

#include <netinet/ether.h>
#include <stdint.h>

#ifndef LITE_COMPILATION
#include "PHY/if4_tools.h"
#endif

// ETH transport preference modes
#define ETH_UDP_MODE          0
#define ETH_RAW_MODE          1
#define ETH_UDP_IF4p5_MODE    2
#define ETH_RAW_IF4p5_MODE    3
#define ETH_UDP_IF5_ECPRI_MODE  4    

// COMMOM HEADER LENGTHS

#define NO_COMPRESS      0       
#define ALAW_COMPRESS    1

#define UDP_HEADER_SIZE_BYTES	8
#define IPV4_HEADER_SIZE_BYTES	60	// This is the maximum IPv4 header length

// Time domain RRH packet sizes
#define MAC_HEADER_SIZE_BYTES (sizeof(struct ether_header))
#define MAX_PACKET_SEQ_NUM(spp,spf) (spf/spp)
#define PAYLOAD_SIZE_BYTES(nsamps) (nsamps<<2)
#define UDP_PACKET_SIZE_BYTES(nsamps) (APP_HEADER_SIZE_BYTES + PAYLOAD_SIZE_BYTES(nsamps))
#define RAW_PACKET_SIZE_BYTES(nsamps) (APP_HEADER_SIZE_BYTES + MAC_HEADER_SIZE_BYTES + PAYLOAD_SIZE_BYTES(nsamps))

#define PAYLOAD_SIZE_BYTES_ALAW(nsamps) (nsamps<<1)
#define UDP_PACKET_SIZE_BYTES_ALAW(nsamps) (APP_HEADER_SIZE_BYTES + PAYLOAD_SIZE_BYTES_ALAW(nsamps))
#define RAW_PACKET_SIZE_BYTES_ALAW(nsamps) (APP_HEADER_SIZE_BYTES + MAC_HEADER_SIZE_BYTES + PAYLOAD_SIZE_BYTES_ALAW(nsamps))

// Packet sizes for IF4p5 interface format
#define DATA_BLOCK_SIZE_BYTES(scaled_nblocks) (sizeof(uint16_t)*scaled_nblocks)
#define PRACH_NUM_SAMPLES (839*2)
#define PRACH_BLOCK_SIZE_BYTES (sizeof(int16_t)*PRACH_NUM_SAMPLES)  
 
#define RAW_IF4p5_PDLFFT_SIZE_BYTES(nblocks) (MAC_HEADER_SIZE_BYTES + sizeof_IF4p5_header_t + DATA_BLOCK_SIZE_BYTES(nblocks))  
#define RAW_IF4p5_PULFFT_SIZE_BYTES(nblocks) (MAC_HEADER_SIZE_BYTES + sizeof_IF4p5_header_t + DATA_BLOCK_SIZE_BYTES(nblocks))  
#define RAW_IF4p5_PULTICK_SIZE_BYTES (MAC_HEADER_SIZE_BYTES + sizeof_IF4p5_header_t)  
#define RAW_IF4p5_PRACH_SIZE_BYTES (MAC_HEADER_SIZE_BYTES + sizeof_IF4p5_header_t + PRACH_BLOCK_SIZE_BYTES)
#define UDP_IF4p5_PDLFFT_SIZE_BYTES(nblocks) (UDP_HEADER_SIZE_BYTES + IPV4_HEADER_SIZE_BYTES + sizeof_IF4p5_header_t + DATA_BLOCK_SIZE_BYTES(nblocks))  
#define UDP_IF4p5_PULFFT_SIZE_BYTES(nblocks) (UDP_HEADER_SIZE_BYTES + IPV4_HEADER_SIZE_BYTES + sizeof_IF4p5_header_t + DATA_BLOCK_SIZE_BYTES(nblocks))  
#define UDP_IF4p5_PULTICK_SIZE_BYTES (UDP_HEADER_SIZE_BYTES + IPV4_HEADER_SIZE_BYTES + sizeof_IF4p5_header_t)  
#define UDP_IF4p5_PRACH_SIZE_BYTES (UDP_HEADER_SIZE_BYTES + IPV4_HEADER_SIZE_BYTES + sizeof_IF4p5_header_t + PRACH_BLOCK_SIZE_BYTES)

// Mobipass packet sizes
#define RAW_IF5_MOBIPASS_BLOCK_SIZE_BYTES 1280
#define RAW_IF5_MOBIPASS_SIZE_BYTES (MAC_HEADER_SIZE_BYTES + sizeof_IF5_mobipass_header_t + RAW_IF5_MOBIPASS_BLOCK_SIZE_BYTES)

#endif
