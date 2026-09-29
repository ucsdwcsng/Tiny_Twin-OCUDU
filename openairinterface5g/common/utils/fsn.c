/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "fsn.h"

#include <assert.h>

static int num_slots[5] = {10, 20, 40, 80, 160};

fsn_t fsn_add_delta(fsn_t fsn, uint32_t delta)
{
  const int slots_frame = num_slots[fsn.mu];
  int next_f = fsn.f + delta / slots_frame;
  int next_s = fsn.s + delta % slots_frame;
  fsn_t res = {
    .f = (next_f + next_s / slots_frame) % 1024,
    .s = next_s % slots_frame,
    .mu = fsn.mu,
  };
  return res;
}

int fsn_get_diff(fsn_t a, fsn_t b)
{
  assert(a.mu == b.mu);
  const int slots_frame = num_slots[a.mu];
  int diff = (a.f * slots_frame + a.s) - (b.f * slots_frame + b.s);
  if (diff < -512 * slots_frame)
    diff += 1024 * slots_frame;
  else if (diff > 512 * slots_frame)
    diff -= 1024 * slots_frame;
  return diff;
}

fsn_t fsn_get_max(fsn_t a, fsn_t b)
{
  if (fsn_get_diff(a, b) <= 0) {
    return b;
  } else {
    return a;
  }
}

bool fsn_equal(fsn_t a, fsn_t b)
{
  assert(a.mu == b.mu);
  return a.f == b.f && a.s == b.s && a.mu == b.mu;
}

bool fsn_in_the_past(fsn_t old, fsn_t new)
{
  int diff = fsn_get_diff(old, new);
  return diff < 0;
}
