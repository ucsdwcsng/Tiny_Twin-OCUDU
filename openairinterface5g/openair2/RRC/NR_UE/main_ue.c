/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*
 * \brief RRC layer top level initialization
 */

#include "defs.h"
#include "rrc_proto.h"
#include "common/utils/LOG/log.h"
#include "executables/softmodem-common.h"

void init_nsa_message(NR_UE_RRC_INST_t *rrc, char* reconfig_file, char* rbconfig_file)
{
  if (get_softmodem_params()->phy_test == 1 || get_softmodem_params()->do_ra == 1) {
    // read in files for RRCReconfiguration and RBconfig

    LOG_I(NR_RRC, "using %s for rrc init[1/2]\n", reconfig_file);
    FILE *fd = fopen(reconfig_file, "r");
    AssertFatal(fd,
                "cannot read file %s: errno %d, %s\n",
                reconfig_file,
                errno,
                strerror(errno));
    char buffer[1024];
    int msg_len = fread(buffer, 1, 1024, fd);
    fclose(fd);
    process_nsa_message(rrc, nr_SecondaryCellGroupConfig_r15, buffer, msg_len);

    LOG_I(NR_RRC, "using %s for rrc init[2/2]\n", rbconfig_file);
    fd = fopen(rbconfig_file, "r");
    AssertFatal(fd,
                "cannot read file %s: errno %d, %s\n",
                rbconfig_file,
                errno,
                strerror(errno));
    msg_len = fread(buffer, 1, 1024, fd);
    fclose(fd);
    process_nsa_message(rrc, nr_RadioBearerConfigX_r15, buffer,msg_len);
    MessageDef* ittiMsg = itti_alloc_new_message(TASK_RRC_NRUE, rrc->ue_id, NAS_INIT_NOS1_IF);
    itti_send_msg_to_task(TASK_NAS_NRUE, rrc->ue_id, ittiMsg);
  }
  else
    LOG_D(NR_RRC, "In NSA mode \n");
}
