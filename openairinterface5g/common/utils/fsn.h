/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef FSN_H_
#define FSN_H_

#include <stdint.h>
#include <stdbool.h>

/// helper type to encapsulate a frame/slot combination in a single type.
typedef struct fsn {
  uint16_t f; /// frame
  uint16_t s; /// slot
  uint8_t mu; /// numerology
} fsn_t;

/* \brief Add delta slots to fsn. Note that this depends also on the stored
 * numerology inside fsn. */
fsn_t fsn_add_delta(fsn_t fsn, uint32_t delta);

/* \brief Get difference of slots for a and b.
 * \note asserts if numerology not the same. */
int fsn_get_diff(fsn_t a, fsn_t b);

/* \brief Gets the larger of two fsns */
fsn_t fsn_get_max(fsn_t a, fsn_t b);

/* \brief Compare if both fsn_t's are equal */
bool fsn_equal(fsn_t a, fsn_t b);

/* \brief Determine if told is in the past of tnew. The threshold is 512
 * frames. */
bool fsn_in_the_past(fsn_t told, fsn_t tnew);

#endif /* FSN_H_ */
