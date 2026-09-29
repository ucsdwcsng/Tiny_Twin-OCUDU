/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef CONNECT_MPLANE_H
#define CONNECT_MPLANE_H

#include "ru-mplane-api.h"

bool connect_mplane(ru_session_t *ru_session);

void disconnect_mplane(void *rus_disconnect);

#endif /* CONNECT_MPLANE_H */
