/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef COMMON_UTIL_TIME_MANAGER_TIME_MANAGER
#define COMMON_UTIL_TIME_MANAGER_TIME_MANAGER

#include <stdint.h>

#include "time_source.h"

typedef void (*time_manager_tick_function_t)(void);

void time_manager_start(time_manager_tick_function_t *tick_functions,
                        int tick_functions_count,
                        time_source_type_t time_source);
void time_manager_iq_samples(uint64_t iq_samples_count,
                             uint64_t iq_samples_per_second);
void time_manager_finish(void);

#endif /* COMMON_UTIL_TIME_MANAGER_TIME_MANAGER */
