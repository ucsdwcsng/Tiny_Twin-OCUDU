/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef NAS_UE_TASK_H_
#define NAS_UE_TASK_H_

#include "common/openairinterface5g_limits.h"
#include "user_defs.h"

// XXX simple array container for multiple users
typedef struct {
    size_t count;
    nas_user_t item[NUMBER_OF_UE_MAX];
} nas_user_container_t;

nas_user_t *find_user_from_fd(nas_user_container_t *users, int fd);

void *nas_ue_task(void *args_p);

#endif /* NAS_TASK_H_ */
