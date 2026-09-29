/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

# include "intertask_interface.h"
# include "create_tasks.h"
# include "common/utils/LOG/log.h"
# include "executables/softmodem-common.h"

  #include "sctp_eNB_task.h"
  #include "s1ap_eNB.h"
  #include "openair3/NAS/UE/nas_ue_task.h"
  #include "RRC/LTE/rrc_defs.h"
# include "enb_app.h"

int create_tasks_ue(uint32_t ue_nb)
{
  LOG_D(ENB_APP, "%s(ue_nb:%d)\n", __FUNCTION__, ue_nb);
  itti_wait_ready(1);

  if (!IS_SOFTMODEM_NOS1) {
#      if defined(NAS_BUILT_IN_UE)

    if (ue_nb > 0) {
      nas_user_container_t *users = calloc(1, sizeof(*users));

      if (users == NULL) abort();

      users->count = ue_nb;
      ittiTask_parms_t parms = {users, NULL};
      if (itti_create_task(TASK_NAS_UE, nas_ue_task, &parms) < 0) {
        LOG_E(NAS, "Create task for NAS UE failed\n");
        free(users);
        return -1;
      }
    }

#      endif
  }

  if (ue_nb > 0) {
    if (itti_create_task (TASK_RRC_UE, rrc_ue_task, NULL) < 0) {
      LOG_E(RRC, "Create task for RRC UE failed\n");
      return -1;
    }
  }

  itti_wait_ready(0);
  return 0;
}

