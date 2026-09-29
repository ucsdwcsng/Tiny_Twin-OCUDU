/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef CONFIGURE_MPLANE_H
#define CONFIGURE_MPLANE_H

#include "ru-mplane-api.h"
#include "radio/COMMON/common_lib.h"
#include <nc_client.h>

bool edit_val_commmit_rpc(ru_session_t *ru_session, const char *content, const NC_RPC_EDIT_DFLTOP op);

bool get_running_u_plane_config(ru_session_t *ru_session);

#endif /* CONFIGURE_MPLANE_H */
