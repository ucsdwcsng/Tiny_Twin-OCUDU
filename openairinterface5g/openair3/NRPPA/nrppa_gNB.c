/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "tree.h"
#include "queue.h"

#include "intertask_interface.h"

#include "assertions.h"
#include "conversions.h"

#include "nrppa_gNB.h"
#include "nrppa_common.h"
#include "nrppa_gNB_handlers.h"
#include "nrppa_gNB_location_information_transfer.h"
#include "nrppa_gNB_measurement_information_transfer.h"

void nrppa_gNB_init(void)
{
  LOG_I(NRPPA, "Starting NRPPA layer\n");
  itti_mark_task_ready(TASK_NRPPA);
}

void *nrppa_gNB_process_itti_msg(void *notUsed)
{
  UNUSED(notUsed);
  MessageDef *received_msg = NULL;
  int result;
  itti_receive_msg(TASK_NRPPA, &received_msg);
  if (received_msg) {
    instance_t instance = ITTI_MSG_DESTINATION_INSTANCE(received_msg);
    LOG_I(NRPPA, "Received message %s\n", ITTI_MSG_NAME(received_msg));
    switch (ITTI_MSG_ID(received_msg)) {
      case TERMINATE_MESSAGE:
        LOG_W(NRPPA, " *** Exiting NRPPA thread\n");
        itti_exit_task();
        break;
      case NGAP_DOWNLINKUEASSOCIATEDNRPPA:
        nrppa_handle_downlink_ue_associated_nrppa_transport(instance, &NGAP_DOWNLINKUEASSOCIATEDNRPPA(received_msg));
        break;
      case NGAP_DOWNLINKNONUEASSOCIATEDNRPPA:
        nrppa_handle_downlink_non_ue_associated_nrppa_transport(instance, &NGAP_DOWNLINKNONUEASSOCIATEDNRPPA(received_msg));
        break;
      case NRPPA_TRP_INFORMATION_RESP:
        nrppa_gNB_trp_information_response(instance, received_msg);
        break;
      case NRPPA_TRP_INFORMATION_FAILURE:
        nrppa_gNB_trp_information_failure(instance, received_msg);
        break;
      case NRPPA_POSITIONING_INFORMATION_RESP:
        nrppa_gNB_positioning_information_response(instance, received_msg);
        break;
      case NRPPA_POSITIONING_ACTIVATION_RESP:
        nrppa_gNB_positioning_activation_response(instance, received_msg);
        break;
      case NRPPA_MEASUREMENT_RESP:
        nrppa_gNB_measurement_response(instance, received_msg);
        break;
      case NRPPA_MEASUREMENT_FAILURE:
        nrppa_gNB_measurement_failure(instance, received_msg);
        break;
      default:
        LOG_E(NRPPA, "Received unhandled message: %d:%s\n", ITTI_MSG_ID(received_msg), ITTI_MSG_NAME(received_msg));
        break;
    }
    result = itti_free(ITTI_MSG_ORIGIN_ID(received_msg), received_msg);
    AssertFatal(result == EXIT_SUCCESS, "Failed to free memory (%d)!\n", result);
  }
  return NULL;
}

void *nrppa_gNB_task(void *arg)
{
  UNUSED(arg);
  nrppa_gNB_init();

  while (1) {
    (void)nrppa_gNB_process_itti_msg(NULL);
  }

  return NULL;
}
