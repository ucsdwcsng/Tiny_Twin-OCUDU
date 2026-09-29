/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef _SYMBOL_REORDER_H_
#define _SYMBOL_REORDER_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct symbol_reorder_s symbol_reorder_t;

/**
 * @brief Create a generic index reorder buffer.
 *
 * Tracks completions that can arrive out of order, keyed by an arbitrary monotonically increasing
 * integer index space (e.g. an absolute symbol number), and reports the new contiguous high-water
 * mark each time the reorder buffer's expected-next index advances.
 *
 * Distinct from the transport-layer TX packet reordering (radio/COMMON's re_order_t) - this tool
 * has no notion of packets or timestamps, it is a plain contiguous-index tracker.
 *
 * @param start_index First index the caller expects to complete.
 * @param valid_position_mask Optional bitmask (one bit per index, repeating every mask_bit_length
 *        indices) marking which positions in the index space are ever expected to complete. Indices
 *        whose bit is unset are skipped when advancing. Pass NULL (with mask_bit_length 0) if every
 *        index is expected to complete - the common case for a plain, non-periodic sequence.
 * @param mask_bit_length Length of valid_position_mask in bits (period of the repeating pattern),
 *        ignored if valid_position_mask is NULL.
 * @return symbol_reorder_t* Opaque handle, to be released with symbol_reorder_destroy().
 */
symbol_reorder_t *symbol_reorder_create(uint64_t start_index, const uint8_t *valid_position_mask, uint16_t mask_bit_length);

void symbol_reorder_destroy(symbol_reorder_t *reorder);

/**
 * @brief Notify the reorder buffer that a range of indices completed, possibly out of order.
 *
 * Wakes any thread blocked in symbol_reorder_wait_at_least() if the contiguous frontier advanced.
 *
 * @param reorder Handle returned by symbol_reorder_create().
 * @param abs_start_index First index of the completed range.
 * @param num_indices Number of consecutive indices completed starting at abs_start_index (usually 1).
 * @return true if the contiguous frontier advanced, false otherwise.
 */
bool symbol_reorder_advance(symbol_reorder_t *reorder, uint64_t abs_start_index, uint32_t num_indices);

/**
 * @brief Block until a given index has completed or abort.
 *
 * @param reorder Handle returned by symbol_reorder_create().
 * @param target Index to wait for; returns as soon as it (and everything before it) has completed.
 * @param stop_flag Optional (may be NULL): polled each time the reorder buffer is notified; a
 *        nonzero value causes the wait to return early, without necessarily having reached target.
 * @return The contiguous high-water mark observed when the wait returned. Only guaranteed to be
 *         >= target if the wait wasn't cut short by stop_flag - the caller should check that itself
 *         (e.g. via the same flag it passed in) before trusting the return value reached target.
 */
uint64_t symbol_reorder_wait_at_least(symbol_reorder_t *reorder, uint64_t target, const volatile int *stop_flag);

/**
 * @brief Wake every thread currently blocked in symbol_reorder_wait_at_least(), e.g. on shutdown.
 *
 * @param reorder Handle returned by symbol_reorder_create().
 */
void symbol_reorder_notify_all(symbol_reorder_t *reorder);

#ifdef __cplusplus
}
#endif

#endif
