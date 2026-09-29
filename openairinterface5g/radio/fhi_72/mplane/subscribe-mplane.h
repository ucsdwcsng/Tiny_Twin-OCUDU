/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef SUBSCRIBE_MPLANE_H
#define SUBSCRIBE_MPLANE_H

#include "ru-mplane-api.h"

#include <stdbool.h>

bool subscribe_mplane(ru_session_t *ru_session, const char *stream, const char *filter, void *answer);

bool update_timer_mplane(ru_session_t *ru_session, char **answer);

#endif /* SUBSCRIBE_MPLANE_H */
