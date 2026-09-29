/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef RPC_SEND_RECEIVE_MPLANE_H
#define RPC_SEND_RECEIVE_MPLANE_H

#include "ru-mplane-api.h"

#include <libyang/libyang.h>
#include <nc_client.h>

#define CLI_RPC_REPLY_TIMEOUT 60000   // time [ms] to wait for server reply

bool rpc_send_recv(struct nc_session *session, struct nc_rpc *rpc, const NC_WD_MODE wd_mode, const int timeout_ms, char **answer);

#endif /* RPC_SEND_RECEIVE_MPLANE_H */
