/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef __IF4_TOOLS_H__
#define __IF4_TOOLS_H__
#include "openair1/PHY/defs_RU.h"

/// Macro for IF4 packet type
#define IF4p5_PACKET_TYPE 0x080A 
#define IF4p5_PULFFT 0x0019 
#define IF4p5_PDLFFT 0x0020
#define IF4p5_PRACH 0x0021
#define IF4p5_PRACH_BR_CE0 0x0022
#define IF4p5_PRACH_BR_CE1 0x0023
#define IF4p5_PRACH_BR_CE2 0x0024
#define IF4p5_PRACH_BR_CE3 0x0025
#define IF4p5_PULTICK 0x0026

struct IF4p5_header {  
  /// Type
  uint16_t type; 
  /// Sub-Type
  uint16_t sub_type;
  /// Reserved
  uint32_t rsvd;
  /// Frame Status
  uint32_t frame_status;

} __attribute__ ((__packed__));

typedef struct IF4p5_header IF4p5_header_t;
#define sizeof_IF4p5_header_t 12 

void gen_IF4p5_dl_header(IF4p5_header_t*, int, int);

void gen_IF4p5_ul_header(IF4p5_header_t*, uint16_t, int, int);

void gen_IF4p5_prach_header(IF4p5_header_t*, int, int);

void send_IF4p5(RU_t*, int, int, uint16_t);

void recv_IF4p5(RU_t*, int*, int*, uint16_t*, uint32_t*);

void malloc_IF4p5_buffer(RU_t*);

#endif
