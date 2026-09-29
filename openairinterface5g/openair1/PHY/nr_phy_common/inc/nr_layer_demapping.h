/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef NR_LAYER_DEMAPPING_H
#define NR_LAYER_DEMAPPING_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief De-map received LLRs from per-layer buffers into a single flat output buffer.
 * @param Nl          Number of layers (1-4)
 * @param mod_order   Modulation order (2/4/6/8)
 * @param nb_re       Number of REs in this symbol
 * @param llr_layers  Per-layer LLR pointers: llr_layers[l] points to
 *                    nb_re * mod_order int16_t values for layer l
 * @param llr_out     Output flat buffer (nb_re * mod_order * Nl int16_t values)
 */
void nr_layer_demapping(uint8_t Nl, uint8_t mod_order, int nb_re, int16_t **llr_layers, int16_t *llr_out);

#ifdef __cplusplus
}
#endif

#endif /* NR_LAYER_DEMAPPING_H */
