/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "symbol_reorder.h"

#include <condition_variable>
#include <mutex>
#include <unordered_map>
#include <vector>

struct symbol_reorder_s {
  std::mutex mutex;
  std::condition_variable cv;
  std::unordered_map<uint64_t, uint64_t> completed; // key: start index, value: count (usually 1)
  uint64_t next_expected;
  std::vector<uint8_t> valid_position_mask; // empty if every index is expected
  uint16_t mask_bit_length;

  bool is_valid_position(uint64_t index) const
  {
    if (valid_position_mask.empty()) {
      return true;
    }
    uint16_t bit = index % mask_bit_length;
    return (valid_position_mask[bit / 8] & (1 << (bit % 8))) != 0;
  }
};

symbol_reorder_t *symbol_reorder_create(uint64_t start_index, const uint8_t *valid_position_mask, uint16_t mask_bit_length)
{
  symbol_reorder_t *reorder = new symbol_reorder_s();
  reorder->next_expected = start_index;
  reorder->mask_bit_length = mask_bit_length;
  if (valid_position_mask && mask_bit_length > 0) {
    size_t mask_bytes = (mask_bit_length + 7) / 8;
    reorder->valid_position_mask.assign(valid_position_mask, valid_position_mask + mask_bytes);
  }
  return reorder;
}

void symbol_reorder_destroy(symbol_reorder_t *reorder)
{
  delete reorder;
}

bool symbol_reorder_advance(symbol_reorder_t *reorder, uint64_t abs_start_index, uint32_t num_indices)
{
  std::unique_lock<std::mutex> lock(reorder->mutex);
  reorder->completed[abs_start_index] = num_indices;

  bool advanced = false;
  while (true) {
    if (!reorder->is_valid_position(reorder->next_expected)) {
      // Positions outside the valid mask never produce a completion - skip over them.
      reorder->next_expected++;
      continue;
    }
    auto it = reorder->completed.find(reorder->next_expected);
    if (it == reorder->completed.end()) {
      break;
    }
    reorder->next_expected += it->second;
    reorder->completed.erase(it);
    advanced = true;
  }
  lock.unlock();

  if (advanced) {
    reorder->cv.notify_all();
  }
  return advanced;
}

uint64_t symbol_reorder_wait_at_least(symbol_reorder_t *reorder, uint64_t target, const volatile int *stop_flag)
{
  std::unique_lock<std::mutex> lock(reorder->mutex);
  reorder->cv.wait(lock, [&] { return reorder->next_expected > target || (stop_flag && *stop_flag); });
  return reorder->next_expected - 1;
}

void symbol_reorder_notify_all(symbol_reorder_t *reorder)
{
  // No state changes, just wake waiters so they re-check their stop_flag.
  reorder->cv.notify_all();
}
