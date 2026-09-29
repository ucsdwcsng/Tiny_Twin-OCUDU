/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "config-mplane.h"
#include "rpc-send-recv.h"
#include "common/utils/assertions.h"

#include <libyang/libyang.h>

static bool edit_config_mplane(ru_session_t *ru_session, const char *content, const NC_RPC_EDIT_DFLTOP op)
{
  int timeout = CLI_RPC_REPLY_TIMEOUT;
  struct nc_rpc *rpc;
  NC_WD_MODE wd = NC_WD_ALL;
  NC_PARAMTYPE param = NC_PARAMTYPE_CONST;
  NC_DATASTORE target = NC_DATASTORE_CANDIDATE;
  NC_RPC_EDIT_TESTOPT test = NC_RPC_EDIT_TESTOPT_UNKNOWN;
  NC_RPC_EDIT_ERROPT err = NC_RPC_EDIT_ERROPT_UNKNOWN;

  MP_LOG_I("RPC request to RU \"%s\" = <edit-config>:\n%s\n", ru_session->ru_ip_add, content);
  rpc = nc_rpc_edit(target, op, test, err, content, param);
  AssertError(rpc != NULL, return false, "[MPLANE] <edit-config> RPC creation failed.\n");

  bool success = rpc_send_recv((struct nc_session *)ru_session->session, rpc, wd, timeout, NULL);
  AssertError(success, return false, "[MPLANE] Failed to edit configuration for the candidate datastore.\n");

  MP_LOG_I("Successfully edited the candidate datastore for RU \"%s\".\n", ru_session->ru_ip_add);

  nc_rpc_free(rpc);

  return true;
}

static bool validate_config_mplane(ru_session_t *ru_session)
{
  int timeout = CLI_RPC_REPLY_TIMEOUT;
  struct nc_rpc *rpc;
  NC_WD_MODE wd = NC_WD_ALL;
  NC_PARAMTYPE param = NC_PARAMTYPE_CONST;
  char *src_start = NULL;
  NC_DATASTORE source = NC_DATASTORE_CANDIDATE;

  MP_LOG_I("RPC request to RU \"%s\" = <validate> candidate datastore.\n", ru_session->ru_ip_add);
  rpc = nc_rpc_validate(source, src_start, param);
  AssertError(rpc != NULL, return false, "[MPLANE] <validate> RPC creation failed.\n");

  bool success = rpc_send_recv((struct nc_session *)ru_session->session, rpc, wd, timeout, NULL);
  AssertError(success, return false, "[MPLANE] Failed to validate candidate datastore.\n");

  MP_LOG_I("Successfully validated candidate datastore for RU \"%s\".\n", ru_session->ru_ip_add);

  nc_rpc_free(rpc);

  return true;
}

static bool commit_config_mplane(ru_session_t *ru_session)
{
  int timeout = CLI_RPC_REPLY_TIMEOUT;
  struct nc_rpc *rpc;
  NC_WD_MODE wd = NC_WD_ALL;
  NC_PARAMTYPE param = NC_PARAMTYPE_CONST;
  int confirmed = 0;
  int32_t confirm_timeout = 0;
  char *persist = NULL, *persist_id = NULL;

  MP_LOG_I("RPC request to RU \"%s\" = <commit> candidate datastore.\n", ru_session->ru_ip_add);
  rpc = nc_rpc_commit(confirmed, confirm_timeout, persist, persist_id, param);
  AssertError(rpc != NULL, return false, "[MPLANE] <commit> RPC creation failed.\n");

  bool success = rpc_send_recv((struct nc_session *)ru_session->session, rpc, wd, timeout, NULL);
  AssertError(success, return false, "[MPLANE] Failed to commit candidate datastore.\n");

  MP_LOG_I("Successfully commited configuration into running datastore for RU \"%s\".\n", ru_session->ru_ip_add);

  nc_rpc_free(rpc);

  return true;
}

bool edit_val_commmit_rpc(ru_session_t *ru_session, const char *content, const NC_RPC_EDIT_DFLTOP op)
{
  bool success = false;

  success = edit_config_mplane(ru_session, content, op);
  AssertError(success, return false, "[MPLANE] Unable to edit the RU configuration.\n");

  success = validate_config_mplane(ru_session);
  AssertError(success, return false, "[MPLANE] Unable to validate the RU configuration.\n");

  success = commit_config_mplane(ru_session);
  AssertError(success, return false, "[MPLANE] Unable to commit the RU configuration.\n");

  return success;
}

bool get_running_u_plane_config(ru_session_t *ru_session)
{
  bool success = false;

  int timeout = CLI_RPC_REPLY_TIMEOUT;
  struct nc_rpc *rpc;
  NC_WD_MODE wd = NC_WD_ALL;
  NC_PARAMTYPE param = NC_PARAMTYPE_CONST;
  NC_DATASTORE target = NC_DATASTORE_RUNNING;

  MP_LOG_I("RPC request to RU \"%s\" = <get-config> running datastore.\n", ru_session->ru_ip_add);
  rpc = nc_rpc_getconfig(target, "/o-ran-uplane-conf:user-plane-configuration", wd, param);
  AssertError(rpc != NULL, return false, "[MPLANE] <get-config> RPC creation failed.\n");
  char *cur_u_plane_config = NULL;
  success = rpc_send_recv((struct nc_session *)ru_session->session, rpc, wd, timeout, &cur_u_plane_config);
  AssertError(success, return false, "[MPLANE] Failed to get running datastore.\n");
  MP_LOG_I("Current U-plane configuration of RU \"%s\":\n%s\n", ru_session->ru_ip_add, cur_u_plane_config);
  // delete any current U-plane configuration whether exists or not
  const char *delete_u_plane_config = "<user-plane-configuration xmlns=\"urn:o-ran:uplane-conf:1.0\">\n\
</user-plane-configuration>";
  /* We cannot use NC_RPC_EDIT_DFLTOP_MERGE because it would merge `delete_u_plane_config` with the current
   * configuration which will lead to a conflict. The NC_RPC_EDIT_DFLTOP_REPLACE will replace `delete_u_plane_config`
   * with the current one. */
  success = edit_val_commmit_rpc(ru_session, delete_u_plane_config, NC_RPC_EDIT_DFLTOP_REPLACE);
  AssertError(success, return false, "[MPLANE] Unable to continue.\n");

  free(cur_u_plane_config);

  return success;
}
