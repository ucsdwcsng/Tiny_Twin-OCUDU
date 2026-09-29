/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "nr_modulation.h"
#include "openair1/PHY/TOOLS/tools_defs.h"

void nr_beam_precoding(c16_t **txdataF,
	               c16_t **txdataF_BF,
                       NR_DL_FRAME_PARMS *frame_parms,
	               int32_t ***beam_weights,
                       int symbol,
                       int aa,
                       int nb_antenna_ports,
                       int offset)
{
  // clear txdata_BF[aa][re] for each call of ue_spec_beamforming
  memset(&txdataF_BF[aa][symbol*frame_parms->ofdm_symbol_size], 0, sizeof(c16_t) *(frame_parms->ofdm_symbol_size));

  for (int p = 0; p < nb_antenna_ports; p++) {
    multadd_cpx_vector((c16_t *)&txdataF[p][(symbol * frame_parms->ofdm_symbol_size) + offset],
                       (c16_t *)beam_weights[p][aa],
                       (c16_t *)&txdataF_BF[aa][symbol * frame_parms->ofdm_symbol_size],
                       frame_parms->ofdm_symbol_size,
                       15);
  }
}
