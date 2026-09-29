/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef COMMON_UTIL_TIME_MANAGER_TIME_SOURCE
#define COMMON_UTIL_TIME_MANAGER_TIME_SOURCE

#include <stdint.h>

/* opaque data type */
typedef void time_source_t;

typedef enum {
  TIME_SOURCE_REALTIME,
  TIME_SOURCE_IQ_SAMPLES
} time_source_type_t;

time_source_t *new_time_source(time_source_type_t type);

void free_time_source(time_source_t *time_source);

void time_source_set_callback(time_source_t *time_source,
                              void (*callback)(void *),
                              void *callback_data);

void time_source_iq_add(time_source_t *time_source,
                        uint64_t iq_samples_count,
                        uint64_t iq_samples_per_second);

#endif /* COMMON_UTIL_TIME_MANAGER_TIME_SOURCE */
