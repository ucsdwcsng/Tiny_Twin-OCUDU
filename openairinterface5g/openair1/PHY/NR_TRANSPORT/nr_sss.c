/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "PHY/NR_TRANSPORT/nr_transport_proto.h"

//#define NR_SSS_DEBUG

int nr_generate_sss(c16_t *txdataF, int16_t amp, uint8_t ssb_start_symbol, int Nid, NR_DL_FRAME_PARMS *frame_parms)
{
  int16_t x0[NR_SSS_LENGTH];
  int16_t x1[NR_SSS_LENGTH];
  const int x0_initial[7] = { 1, 0, 0, 0, 0, 0, 0 };
  const int x1_initial[7] = { 1, 0, 0, 0, 0, 0, 0 };

  /// Sequence generation
  int Nid2 = Nid % 3;
  int Nid1 = (Nid - Nid2)/3;

  for (int i=0; i < 7; i++) {
    x0[i] = x0_initial[i];
    x1[i] = x1_initial[i];
  }

  for (int i=0; i < NR_SSS_LENGTH - 7; i++) {
    x0[i+7] = (x0[i + 4] + x0[i]) % 2;
    x1[i+7] = (x1[i + 1] + x1[i]) % 2;
  }

  int m0 = 15*(Nid1/112) + (5*Nid2);
  int m1 = Nid1 % 112;

#ifdef NR_SSS_DEBUG
  write_output("d_sss.m", "d_sss", (void*)d_sss, NR_SSS_LENGTH, 1, 1);
#endif

  /// Resource mapping

  // SSS occupies a predefined position (subcarriers 56-182, symbol 2) within the SSB block starting from
  c16_t *tx = txdataF + (ssb_start_symbol + 2) * frame_parms->ofdm_symbol_size + frame_parms->ssb_start_subcarrier + 56;

  for (int i = 0; i < NR_SSS_LENGTH; i++) {
    int16_t d_sss = (1 - 2*x0[(i + m0) % NR_SSS_LENGTH] ) * (1 - 2*x1[(i + m1) % NR_SSS_LENGTH] ) * 23170;
    *tx++ = (c16_t){.r = (amp * d_sss) >> 15};
  }
#ifdef NR_SSS_DEBUG
  //  write_output("sss_0.m", "sss_0", (void*)txdataF[0][l*frame_parms->ofdm_symbol_size], frame_parms->ofdm_symbol_size, 1, 1);
#endif

  return 0;
}
