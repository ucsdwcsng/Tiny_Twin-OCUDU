/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef M2AP_TIMERS_H_
#define M2AP_TIMERS_H_

#include <stdint.h>
#include "common/platform_types.h"

typedef struct {
  /* incremented every TTI (every millisecond when in realtime).
   * Used to check timers.
   * 64 bits gives us more than 500 million years of (realtime) processing.
   * It should be enough.
   */
  uint64_t tti;

  /* timer values (unit: TTI, ie. millisecond when in realtime) */
  int      t_reloc_prep;
  int      tm2_reloc_overall;
} m2ap_timers_t;

void m2ap_timers_init(m2ap_timers_t *t, int t_reloc_prep, int tm2_reloc_overall);
void m2ap_check_timers(instance_t instance);
uint64_t m2ap_timer_get_tti(m2ap_timers_t *t);

#endif /* M2AP_TIMERS_H_ */
