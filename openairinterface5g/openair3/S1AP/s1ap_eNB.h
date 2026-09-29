/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include <stdio.h>
#include <stdint.h>

/** @defgroup _s1ap_impl_ S1AP Layer Reference Implementation
 * @ingroup _ref_implementation_
 * @{
 */

#ifndef S1AP_ENB_H_
#define S1AP_ENB_H_

#define S1AP_MMEIND     0x80000000
#define S1AP_UEIND      0x00000000
#define S1_SETRSP_WAIT  0x00010000
#define S1_SETREQ_WAIT  0x00020000
#define SCTP_REQ_WAIT   0x00030000
#define S1AP_LINEIND    0x0000ffff
#define S1AP_TIMERIND   0x00ff0000

#define S1AP_TIMERID_INIT   0xffffffffffffffff

typedef enum s1ap_timer_type_s {
  S1AP_TIMER_PERIODIC,
  S1AP_TIMER_ONE_SHOT,
  S1AP_TIMER_TYPE_MAX,
} s1ap_timer_type_t;

void *s1ap_eNB_process_itti_msg(void*);
void  s1ap_eNB_init(void);
void *s1ap_eNB_task(void *arg);

int s1ap_timer_remove(long timer_id);
uint32_t s1ap_generate_eNB_id(void);

#endif /* S1AP_ENB_H_ */

/**
 * @}
 */
