/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef COMMON_UTIL_TIME_MANAGER_TIME_CLIENT
#define COMMON_UTIL_TIME_MANAGER_TIME_CLIENT

/* opaque data type */
typedef void time_client_t;

time_client_t *new_time_client(const char *server_ip,
                               int server_port,
                               void (*callback)(void *),
                               void *callback_data);

void free_time_client(time_client_t *time_client);


#endif /* COMMON_UTIL_TIME_MANAGER_TIME_CLIENT */
