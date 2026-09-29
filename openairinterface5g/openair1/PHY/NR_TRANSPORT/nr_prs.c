/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "PHY/defs_gNB.h"
#include "PHY/NR_TRANSPORT/nr_transport_proto.h"
#include "PHY/NR_REFSIG/nr_refsig.h"
#include "PHY/sse_intrin.h"
#include "openair1/PHY/NR_REFSIG/nr_mod_table.h"
//#define DEBUG_PRS_MOD
//#define DEBUG_PRS_MAP

int nr_generate_prs(int slot, c16_t *txdataF, int16_t amp, prs_config_t *prs_cfg, const NR_DL_FRAME_PARMS *frame_parms)
{
  // PRS resource mapping with combsize=k which means PRS symbols exist in every k-th subcarrier in frequency domain
  // According to ts138.211 sec.7.4.1.7.2
  for (int l = prs_cfg->SymbolStart; l < prs_cfg->SymbolStart + prs_cfg->NumPRSSymbols; l++) {
    int k_prime = 0;
    int symInd = l-prs_cfg->SymbolStart;
    if (prs_cfg->CombSize == 2) {
      k_prime = k_prime_table[0][symInd];
    }
    else if (prs_cfg->CombSize == 4){
      k_prime = k_prime_table[1][symInd];
    }
    else if (prs_cfg->CombSize == 6){
      k_prime = k_prime_table[2][symInd];
    }
    else if (prs_cfg->CombSize == 12){
      k_prime = k_prime_table[3][symInd];
    }

    int k = (prs_cfg->REOffset + k_prime) % prs_cfg->CombSize + prs_cfg->RBOffset * NR_NB_SC_PER_RB;

    // QPSK modulation
    uint32_t *gold = nr_gold_prs(prs_cfg->NPRSID, slot, l);
    c16_t *tx = txdataF + l * frame_parms->ofdm_symbol_size;
    for (int m = 0; m < (NR_NB_SC_PER_RB / prs_cfg->CombSize) * prs_cfg->NumRB; m++) {
      int idx = (((gold[(m << 1) >> 5]) >> ((m << 1) & 0x1f)) & 3);
#ifdef DEBUG_PRS_MAP
      LOG_D("m %d at k %d of l %d reIdx %d\n", m, k, l, (l*frame_parms->ofdm_symbol_size + k)<<1);
#endif
      tx[k] = c16mulRealShift(nr_qpsk_mod_table[idx], amp, 15);
      k = k +  prs_cfg->CombSize;
    }
  }
#ifdef DEBUG_PRS_MAP
  LOG_M("nr_prs.m", "prs",(int16_t *)&txdataF[prs_cfg->SymbolStart*frame_parms->ofdm_symbol_size],prs_cfg->NumPRSSymbols*frame_parms->ofdm_symbol_size, 1, 1);
#endif
  return 0;
}
