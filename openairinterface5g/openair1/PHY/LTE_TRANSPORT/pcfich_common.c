/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief Top-level routines for generating and decoding  the PCFICH/CFI physical/transport channel V8.6 2009-03
 */
#include "PHY/impl_defs_top.h"
#include "PHY/defs_eNB.h"

void generate_pcfich_reg_mapping(LTE_DL_FRAME_PARMS *frame_parms)
{

  uint16_t kbar = 6 * (frame_parms->Nid_cell %(2*frame_parms->N_RB_DL));
  uint16_t first_reg;
  uint16_t *pcfich_reg = frame_parms->pcfich_reg;

  pcfich_reg[0] = kbar/6;
  first_reg = pcfich_reg[0];

  frame_parms->pcfich_first_reg_idx=0;

  pcfich_reg[1] = ((kbar + (frame_parms->N_RB_DL>>1)*6)%(frame_parms->N_RB_DL*12))/6;

  if (pcfich_reg[1] < pcfich_reg[0]) {
    frame_parms->pcfich_first_reg_idx = 1;
    first_reg = pcfich_reg[1];
  }

  pcfich_reg[2] = ((kbar + (frame_parms->N_RB_DL)*6)%(frame_parms->N_RB_DL*12))/6;

  if (pcfich_reg[2] < first_reg) {
    frame_parms->pcfich_first_reg_idx = 2;
    first_reg = pcfich_reg[2];
  }

  pcfich_reg[3] = ((kbar + ((3*frame_parms->N_RB_DL)>>1)*6)%(frame_parms->N_RB_DL*12))/6;

  if (pcfich_reg[3] < first_reg) {
    frame_parms->pcfich_first_reg_idx = 3;
    first_reg = pcfich_reg[3];
  }


  //#ifdef DEBUG_PCFICH
  printf("pcfich_reg : %d,%d,%d,%d\n",pcfich_reg[0],pcfich_reg[1],pcfich_reg[2],pcfich_reg[3]);
  //#endif
}
