/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef COMMON_UTIL_TIME_MANAGER_TIME_SERVER
#define COMMON_UTIL_TIME_MANAGER_TIME_SERVER

#include "time_source.h"

/* opaque data type */
typedef void time_server_t;

time_server_t *new_time_server(const char *ip,
                               int port,
                               void (*callback)(void *),
                               void *callback_data);
void free_time_server(time_server_t *time_server);

void time_server_attach_time_source(time_server_t *time_server,
                                    time_source_t *time_source);

#endif /* COMMON_UTIL_TIME_MANAGER_TIME_SERVER */

