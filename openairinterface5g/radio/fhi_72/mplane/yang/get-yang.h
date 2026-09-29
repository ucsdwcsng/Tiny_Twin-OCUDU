/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef GET_MPLANE_YANG_MODELS_H
#define GET_MPLANE_YANG_MODELS_H

#include "../ru-mplane-api.h"

#include <libyang/libyang.h>

bool load_yang_models(ru_session_t *ru_session, const char *buffer);

#endif /* GET_MPLANE_YANG_MODELS_H */
