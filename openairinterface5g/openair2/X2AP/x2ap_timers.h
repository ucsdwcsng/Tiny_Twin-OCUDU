/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef X2AP_TIMERS_H_
#define X2AP_TIMERS_H_

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
  int      tx2_reloc_overall;
  int      t_dc_prep;
  int      t_dc_overall;
} x2ap_timers_t;

void x2ap_timers_init(x2ap_timers_t *t,
    int t_reloc_prep,
    int tx2_reloc_overall,
    int t_dc_prep,
    int t_dc_overall);
void x2ap_check_timers(instance_t instance);
uint64_t x2ap_timer_get_tti(x2ap_timers_t *t);

#endif /* X2AP_TIMERS_H_ */
