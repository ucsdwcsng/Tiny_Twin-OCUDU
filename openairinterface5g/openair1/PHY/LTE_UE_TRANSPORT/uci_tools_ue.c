/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief Routines for generation of and computations regarding the uplink control information (UCI) for PUSCH. V8.6 2009-03
 */
#include "PHY/defs_UE.h"
#include "PHY/phy_extern_ue.h"
#ifdef DEBUG_UCI_TOOLS
#include "PHY/phy_vars.h"
#endif

//#define DEBUG_UCI 1


uint64_t cqi2hex(uint32_t cqi)
{

  uint64_t cqil = (uint64_t)cqi;
  return ((cqil&3) + (((cqil>>2)&3)<<4) + (((cqil>>4)&3)<<8) + (((cqil>>6)&3)<<12) +
          (((cqil>>8)&3)<<16) + (((cqil>>10)&3)<<20) + (((cqil>>12)&3)<<24) +
          (((cqil>>14)&3)<<28) + (((cqil>>16)&3)<<32) + (((cqil>>18)&3)<<36) +
          (((cqil>>20)&3)<<40) + (((cqil>>22)&3)<<44) + (((cqil>>24)&3)<<48));
}


void print_CQI(void *o,UCI_format_t uci_format,unsigned char eNB_id,int N_RB_DL)
{


  switch(uci_format) {
  case wideband_cqi_rank1_2A:
#ifdef DEBUG_UCI
    switch(N_RB_DL) {
    case 6:
      LOG_I(PHY,"[PRINT CQI] wideband_cqi rank 1: eNB %d, cqi %d\n",eNB_id,
            ((wideband_cqi_rank1_2A_1_5MHz *)o)->cqi1);
      LOG_I(PHY,"[PRINT CQI] wideband_cqi rank 1: eNB %d, pmi (%x) %8x\n",eNB_id,
            ((wideband_cqi_rank1_2A_1_5MHz *)o)->pmi,
            pmi2hex_2Ar1(((wideband_cqi_rank1_2A_1_5MHz *)o)->pmi));
      break;

    case 25:
      LOG_I(PHY,"[PRINT CQI] wideband_cqi rank 1: eNB %d, cqi %d\n",eNB_id,
            ((wideband_cqi_rank1_2A_5MHz *)o)->cqi1);
      LOG_I(PHY,"[PRINT CQI] wideband_cqi rank 1: eNB %d, pmi (%x) %8x\n",eNB_id,
            ((wideband_cqi_rank1_2A_5MHz *)o)->pmi,
            pmi2hex_2Ar1(((wideband_cqi_rank1_2A_5MHz *)o)->pmi));
      break;

    case 50:
      LOG_I(PHY,"[PRINT CQI] wideband_cqi rank 1: eNB %d, cqi %d\n",eNB_id,
            ((wideband_cqi_rank1_2A_10MHz *)o)->cqi1);
      LOG_I(PHY,"[PRINT CQI] wideband_cqi rank 1: eNB %d, pmi (%x) %8x\n",eNB_id,
            ((wideband_cqi_rank1_2A_10MHz *)o)->pmi,
            pmi2hex_2Ar1(((wideband_cqi_rank1_2A_10MHz *)o)->pmi));
      break;

    case 100:
      LOG_I(PHY,"[PRINT CQI] wideband_cqi rank 1: eNB %d, cqi %d\n",eNB_id,
            ((wideband_cqi_rank1_2A_20MHz *)o)->cqi1);
      LOG_I(PHY,"[PRINT CQI] wideband_cqi rank 1: eNB %d, pmi (%x) %8x\n",eNB_id,
            ((wideband_cqi_rank1_2A_20MHz *)o)->pmi,
            pmi2hex_2Ar1(((wideband_cqi_rank1_2A_20MHz *)o)->pmi));
      break;
    }

#endif //DEBUG_UCI
    break;

  case wideband_cqi_rank2_2A:
#ifdef DEBUG_UCI
    switch(N_RB_DL) {
    case 6:
      LOG_I(PHY,"[PRINT CQI] wideband_cqi rank 2: eNB %d, cqi1 %d\n",eNB_id,((wideband_cqi_rank2_2A_1_5MHz *)o)->cqi1);
      LOG_I(PHY,"[PRINT CQI] wideband_cqi rank 2: eNB %d, cqi2 %d\n",eNB_id,((wideband_cqi_rank2_2A_1_5MHz *)o)->cqi2);
      LOG_I(PHY,"[PRINT CQI] wideband_cqi rank 2: eNB %d, pmi %8x\n",eNB_id,pmi2hex_2Ar2(((wideband_cqi_rank2_2A_1_5MHz *)o)->pmi));
      break;

    case 25:
      LOG_I(PHY,"[PRINT CQI] wideband_cqi rank 2: eNB %d, cqi1 %d\n",eNB_id,((wideband_cqi_rank2_2A_5MHz *)o)->cqi1);
      LOG_I(PHY,"[PRINT CQI] wideband_cqi rank 2: eNB %d, cqi2 %d\n",eNB_id,((wideband_cqi_rank2_2A_5MHz *)o)->cqi2);
      LOG_I(PHY,"[PRINT CQI] wideband_cqi rank 2: eNB %d, pmi %8x\n",eNB_id,pmi2hex_2Ar2(((wideband_cqi_rank2_2A_5MHz *)o)->pmi));
      break;

    case 50:
      LOG_I(PHY,"[PRINT CQI] wideband_cqi rank 2: eNB %d, cqi1 %d\n",eNB_id,((wideband_cqi_rank2_2A_10MHz *)o)->cqi1);
      LOG_I(PHY,"[PRINT CQI] wideband_cqi rank 2: eNB %d, cqi2 %d\n",eNB_id,((wideband_cqi_rank2_2A_10MHz *)o)->cqi2);
      LOG_I(PHY,"[PRINT CQI] wideband_cqi rank 2: eNB %d, pmi %8x\n",eNB_id,pmi2hex_2Ar2(((wideband_cqi_rank2_2A_10MHz *)o)->pmi));
      break;

    case 100:
      LOG_I(PHY,"[PRINT CQI] wideband_cqi rank 2: eNB %d, cqi1 %d\n",eNB_id,((wideband_cqi_rank2_2A_20MHz *)o)->cqi1);
      LOG_I(PHY,"[PRINT CQI] wideband_cqi rank 2: eNB %d, cqi2 %d\n",eNB_id,((wideband_cqi_rank2_2A_20MHz *)o)->cqi2);
      LOG_I(PHY,"[PRINT CQI] wideband_cqi rank 2: eNB %d, pmi %8x\n",eNB_id,pmi2hex_2Ar2(((wideband_cqi_rank2_2A_20MHz *)o)->pmi));
      break;
    }

#endif //DEBUG_UCI
    break;

  case HLC_subband_cqi_nopmi:
#ifdef DEBUG_UCI
    switch(N_RB_DL) {
    case 6:
      LOG_I(PHY,"[PRINT CQI] hlc_cqi (no pmi) : eNB %d, cqi1 %d\n",eNB_id,((HLC_subband_cqi_rank1_2A_1_5MHz *)o)->cqi1);
      LOG_I(PHY,"[PRINT CQI] hlc_cqi (no pmi) : eNB %d, diffcqi1 %8x\n",eNB_id,cqi2hex(((HLC_subband_cqi_rank1_2A_1_5MHz *)o)->diffcqi1));
      break;

    case 25:
      LOG_I(PHY,"[PRINT CQI] hlc_cqi (no pmi) : eNB %d, cqi1 %d\n",eNB_id,((HLC_subband_cqi_rank1_2A_5MHz *)o)->cqi1);
      LOG_I(PHY,"[PRINT CQI] hlc_cqi (no pmi) : eNB %d, diffcqi1 %8x\n",eNB_id,cqi2hex(((HLC_subband_cqi_rank1_2A_5MHz *)o)->diffcqi1));
      break;

    case 50:
      LOG_I(PHY,"[PRINT CQI] hlc_cqi (no pmi) : eNB %d, cqi1 %d\n",eNB_id,((HLC_subband_cqi_rank1_2A_10MHz *)o)->cqi1);
      LOG_I(PHY,"[PRINT CQI] hlc_cqi (no pmi) : eNB %d, diffcqi1 %8x\n",eNB_id,cqi2hex(((HLC_subband_cqi_rank1_2A_10MHz *)o)->diffcqi1));
      break;

    case 100:
      LOG_I(PHY,"[PRINT CQI] hlc_cqi (no pmi) : eNB %d, cqi1 %d\n",eNB_id,((HLC_subband_cqi_rank1_2A_20MHz *)o)->cqi1);
      LOG_I(PHY,"[PRINT CQI] hlc_cqi (no pmi) : eNB %d, diffcqi1 %8x\n",eNB_id,cqi2hex(((HLC_subband_cqi_rank1_2A_20MHz *)o)->diffcqi1));
      break;
    }

#endif //DEBUG_UCI
    break;

  case HLC_subband_cqi_rank1_2A:
#ifdef DEBUG_UCI
    switch(N_RB_DL) {
    case 6:
      LOG_I(PHY,"[PRINT CQI] hlc_cqi rank 1: eNB %d, cqi1 %d\n",eNB_id,((HLC_subband_cqi_rank1_2A_5MHz *)o)->cqi1);
      LOG_I(PHY,"[PRINT CQI] hlc_cqi rank 1: eNB %d, diffcqi1 %8x\n",eNB_id,cqi2hex(((HLC_subband_cqi_rank1_2A_5MHz *)o)->diffcqi1));
      LOG_I(PHY,"[PRINT CQI] hlc_cqi rank 1: eNB %d, pmi %d\n",eNB_id,((HLC_subband_cqi_rank1_2A_5MHz *)o)->pmi);
      break;

    case 25:
      LOG_I(PHY,"[PRINT CQI] hlc_cqi rank 1: eNB %d, cqi1 %d\n",eNB_id,((HLC_subband_cqi_rank1_2A_5MHz *)o)->cqi1);
      LOG_I(PHY,"[PRINT CQI] hlc_cqi rank 1: eNB %d, diffcqi1 %8x\n",eNB_id,cqi2hex(((HLC_subband_cqi_rank1_2A_5MHz *)o)->diffcqi1));
      LOG_I(PHY,"[PRINT CQI] hlc_cqi rank 1: eNB %d, pmi %d\n",eNB_id,((HLC_subband_cqi_rank1_2A_5MHz *)o)->pmi);
      break;

    case 50:
      LOG_I(PHY,"[PRINT CQI] hlc_cqi rank 1: eNB %d, cqi1 %d\n",eNB_id,((HLC_subband_cqi_rank1_2A_5MHz *)o)->cqi1);
      LOG_I(PHY,"[PRINT CQI] hlc_cqi rank 1: eNB %d, diffcqi1 %8x\n",eNB_id,cqi2hex(((HLC_subband_cqi_rank1_2A_5MHz *)o)->diffcqi1));
      LOG_I(PHY,"[PRINT CQI] hlc_cqi rank 1: eNB %d, pmi %d\n",eNB_id,((HLC_subband_cqi_rank1_2A_5MHz *)o)->pmi);
      break;

    case 100:
      LOG_I(PHY,"[PRINT CQI] hlc_cqi rank 1: eNB %d, cqi1 %d\n",eNB_id,((HLC_subband_cqi_rank1_2A_5MHz *)o)->cqi1);
      LOG_I(PHY,"[PRINT CQI] hlc_cqi rank 1: eNB %d, diffcqi1 %8x\n",eNB_id,cqi2hex(((HLC_subband_cqi_rank1_2A_5MHz *)o)->diffcqi1));
      LOG_I(PHY,"[PRINT CQI] hlc_cqi rank 1: eNB %d, pmi %d\n",eNB_id,((HLC_subband_cqi_rank1_2A_5MHz *)o)->pmi);
      break;
    }

#endif //DEBUG_UCI
    break;

  case HLC_subband_cqi_rank2_2A:
#ifdef DEBUG_UCI
    switch(N_RB_DL) {
    case 6:
      LOG_I(PHY,"[PRINT CQI] hlc_cqi rank 2: eNB %d, cqi1 %d\n",eNB_id,((HLC_subband_cqi_rank2_2A_1_5MHz *)o)->cqi1);
      LOG_I(PHY,"[PRINT CQI] hlc_cqi rank 2: eNB %d, cqi2 %d\n",eNB_id,((HLC_subband_cqi_rank2_2A_1_5MHz *)o)->cqi2);
      LOG_I(PHY,"[PRINT CQI] hlc_cqi rank 2: eNB %d, diffcqi1 %8x\n",eNB_id,cqi2hex(((HLC_subband_cqi_rank2_2A_1_5MHz *)o)->diffcqi1));
      LOG_I(PHY,"[PRINT CQI] hlc_cqi rank 2: eNB %d, diffcqi2 %8x\n",eNB_id,cqi2hex(((HLC_subband_cqi_rank2_2A_1_5MHz *)o)->diffcqi2));
      LOG_I(PHY,"[PRINT CQI] hlc_cqi rank 2: eNB %d, pmi %d\n",eNB_id,((HLC_subband_cqi_rank2_2A_1_5MHz *)o)->pmi);
      break;

    case 25:
      LOG_I(PHY,"[PRINT CQI] hlc_cqi rank 2: eNB %d, cqi1 %d\n",eNB_id,((HLC_subband_cqi_rank2_2A_5MHz *)o)->cqi1);
      LOG_I(PHY,"[PRINT CQI] hlc_cqi rank 2: eNB %d, cqi2 %d\n",eNB_id,((HLC_subband_cqi_rank2_2A_5MHz *)o)->cqi2);
      LOG_I(PHY,"[PRINT CQI] hlc_cqi rank 2: eNB %d, diffcqi1 %8x\n",eNB_id,cqi2hex(((HLC_subband_cqi_rank2_2A_5MHz *)o)->diffcqi1));
      LOG_I(PHY,"[PRINT CQI] hlc_cqi rank 2: eNB %d, diffcqi2 %8x\n",eNB_id,cqi2hex(((HLC_subband_cqi_rank2_2A_5MHz *)o)->diffcqi2));
      LOG_I(PHY,"[PRINT CQI] hlc_cqi rank 2: eNB %d, pmi %d\n",eNB_id,((HLC_subband_cqi_rank2_2A_5MHz *)o)->pmi);
      break;

    case 50:
      LOG_I(PHY,"[PRINT CQI] hlc_cqi rank 2: eNB %d, cqi1 %d\n",eNB_id,((HLC_subband_cqi_rank2_2A_10MHz *)o)->cqi1);
      LOG_I(PHY,"[PRINT CQI] hlc_cqi rank 2: eNB %d, cqi2 %d\n",eNB_id,((HLC_subband_cqi_rank2_2A_10MHz *)o)->cqi2);
      LOG_I(PHY,"[PRINT CQI] hlc_cqi rank 2: eNB %d, diffcqi1 %8x\n",eNB_id,cqi2hex(((HLC_subband_cqi_rank2_2A_10MHz *)o)->diffcqi1));
      LOG_I(PHY,"[PRINT CQI] hlc_cqi rank 2: eNB %d, diffcqi2 %8x\n",eNB_id,cqi2hex(((HLC_subband_cqi_rank2_2A_10MHz *)o)->diffcqi2));
      LOG_I(PHY,"[PRINT CQI] hlc_cqi rank 2: eNB %d, pmi %d\n",eNB_id,((HLC_subband_cqi_rank2_2A_10MHz *)o)->pmi);
      break;

    case 100:
      LOG_I(PHY,"[PRINT CQI] hlc_cqi rank 2: eNB %d, cqi1 %d\n",eNB_id,((HLC_subband_cqi_rank2_2A_20MHz *)o)->cqi1);
      LOG_I(PHY,"[PRINT CQI] hlc_cqi rank 2: eNB %d, cqi2 %d\n",eNB_id,((HLC_subband_cqi_rank2_2A_20MHz *)o)->cqi2);
      LOG_I(PHY,"[PRINT CQI] hlc_cqi rank 2: eNB %d, diffcqi1 %8x\n",eNB_id,cqi2hex(((HLC_subband_cqi_rank2_2A_20MHz *)o)->diffcqi1));
      LOG_I(PHY,"[PRINT CQI] hlc_cqi rank 2: eNB %d, diffcqi2 %8x\n",eNB_id,cqi2hex(((HLC_subband_cqi_rank2_2A_20MHz *)o)->diffcqi2));
      LOG_I(PHY,"[PRINT CQI] hlc_cqi rank 2: eNB %d, pmi %d\n",eNB_id,((HLC_subband_cqi_rank2_2A_20MHz *)o)->pmi);
      break;
    }

#endif //DEBUG_UCI
    break;

  case ue_selected:
#ifdef DEBUG_UCI
    LOG_W(PHY,"[PRINT CQI] ue_selected CQI not supported yet!!!\n");
#endif //DEBUG_UCI
    break;

  default:
#ifdef DEBUG_UCI
    LOG_E(PHY,"[PRINT CQI] unsupported CQI mode (%d)!!!\n",uci_format);
#endif //DEBUG_UCI
    break;
  }

}






