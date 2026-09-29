/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief Top-level routines for generating primary synchronization signal (PSS) V8.6 2009-03
 * purpose: generate the primary synchronization signals of LTE
 */

//#include "defs.h"
#include "PHY/defs_eNB.h"
#include "PHY/phy_extern.h"
#include "executables/lte-softmodem.h"

int generate_pss(int32_t **txdataF,
                 short amp,
                 LTE_DL_FRAME_PARMS *frame_parms,
                 unsigned short symbol,
                 unsigned short slot_offset) {
  unsigned int Nsymb;
  unsigned short k,m,aa,a;
  uint8_t Nid2;
  const short *primary_sync;
  Nid2 = frame_parms->Nid_cell % 3;

  switch (Nid2) {
    case 0:
      primary_sync = primary_synch0;
      break;

    case 1:
      primary_sync = primary_synch1;
      break;

    case 2:
      primary_sync = primary_synch2;
      break;

    default:
      LOG_E(PHY,"[PSS] eNb_id has to be 0,1,2\n");
      return(-1);
  }

  a = (frame_parms->nb_antenna_ports_eNB == 1) ? amp: (amp*ONE_OVER_SQRT2_Q15)>>15;
  //printf("[PSS] amp=%d, a=%d\n",amp,a);

  Nsymb = (frame_parms->Ncp==NORMAL)?14:12;

  for (aa=0; aa<frame_parms->nb_antenna_ports_eNB; aa++) {
    //  aa = 0;
    // The PSS occupies the inner 6 RBs, which start at
    k = frame_parms->ofdm_symbol_size-3*12+5;

    //printf("[PSS] k = %d\n",k);
    for (m=5; m<67; m++) {
      ((short *)txdataF[aa])[2*(slot_offset*Nsymb/2*frame_parms->ofdm_symbol_size +
                                symbol*frame_parms->ofdm_symbol_size + k)] =
                                  (a * primary_sync[2*m]) >> 15;
      ((short *)txdataF[aa])[2*(slot_offset*Nsymb/2*frame_parms->ofdm_symbol_size +
                                symbol*frame_parms->ofdm_symbol_size + k) + 1] =
                                  (a * primary_sync[2*m+1]) >> 15;
      k+=1;

      if (k >= frame_parms->ofdm_symbol_size) {
        k++; //skip DC
        k-=frame_parms->ofdm_symbol_size;
      }
    }
  }

  return(0);
}

