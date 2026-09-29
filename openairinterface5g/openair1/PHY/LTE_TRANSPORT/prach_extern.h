/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief Common routines for UE/eNB PRACH physical channel V8.6 2009-03
 */

#ifndef __PHY_LTE_TRANSPORT_PRACH_EXTERN__H__
#define __PHY_LTE_TRANSPORT_PRACH_EXTERN__H__

#include "PHY/sse_intrin.h"
#include "PHY/defs_eNB.h"
//#include "PHY/phy_extern.h"

//#define PRACH_DEBUG 1
//#define PRACH_WRITE_OUTPUT_DEBUG 1

extern const uint16_t NCS_unrestricted[16];
extern const uint16_t NCS_restricted[15];
extern const uint16_t NCS_4[7];
extern c16_t root_unit[839]; // quantized roots of unity
extern uint16_t du[838];
// This is table 5.7.1-4 from 36.211
extern PRACH_TDD_PREAMBLE_MAP tdd_preamble_map[64][7];

extern const uint16_t prach_root_sequence_map0_3[838];

extern const uint16_t prach_root_sequence_map4[138];

void dump_prach_config(LTE_DL_FRAME_PARMS *frame_parms,uint8_t subframe);


// This function computes the du
void fill_du(uint8_t prach_fmt);


uint8_t get_num_prach_tdd(module_id_t Mod_id);


uint8_t get_fid_prach_tdd(module_id_t Mod_id,uint8_t tdd_map_index);


int is_prach_subframe(LTE_DL_FRAME_PARMS *frame_parms,uint32_t frame, uint8_t subframe);

#endif
