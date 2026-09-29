/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief computation of some PRACH variables used in both MAC and PHY
 */

uint8_t get_prach_fmt(int prach_ConfigIndex,int frame_type);

uint8_t get_prach_prb_offset(int frame_type,
                             int tdd_config,
                             int N_RB_UL,
                             uint8_t prach_ConfigIndex,
                             uint8_t n_ra_prboffset,
                             uint8_t tdd_mapindex, uint16_t Nf);
int is_prach_subframe0(int tdd_config,int frame_type,uint8_t prach_ConfigIndex,uint32_t frame, uint8_t subframe);
