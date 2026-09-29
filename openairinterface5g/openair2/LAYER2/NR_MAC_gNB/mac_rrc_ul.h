/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef MAC_RRC_UL_H
#define MAC_RRC_UL_H

#include "common/platform_types.h"
#include "f1ap_messages_types.h"

typedef void (*f1_reset_du_initiated_func_t)(const f1ap_reset_t *reset);
typedef void (*f1_reset_acknowledge_cu_initiated_func_t)(const f1ap_reset_ack_t *ack);

typedef void (*f1_setup_request_func_t)(const f1ap_setup_req_t* req);
typedef void (*gnb_du_configuration_update_t)(const f1ap_gnb_du_configuration_update_t *upd);

typedef void (*ue_context_setup_response_func_t)(const f1ap_ue_context_setup_resp_t *resp);
typedef void (*ue_context_modification_response_func_t)(const f1ap_ue_context_mod_resp_t *resp);
typedef void (*ue_context_modification_required_func_t)(const f1ap_ue_context_modif_required_t *t);
typedef void (*ue_context_release_request_func_t)(const f1ap_ue_context_rel_req_t* req);
typedef void (*ue_context_release_complete_func_t)(const f1ap_ue_context_rel_cplt_t *complete);

typedef void (*initial_ul_rrc_message_transfer_func_t)(module_id_t module_id, const f1ap_initial_ul_rrc_message_t *ul_rrc);
typedef void (*trp_information_response_func_t)(const f1ap_trp_information_resp_t *resp);
typedef void (*trp_information_failure_func_t)(const f1ap_trp_information_failure_t *fail);
typedef void (*positioning_information_response_func_t)(const f1ap_positioning_information_resp_t *resp);
typedef void (*positioning_activation_response_func_t)(const f1ap_positioning_activation_resp_t *resp);
typedef void (*positioning_measurement_response_func_t)(const f1ap_positioning_measurement_resp_t *resp);
typedef void (*positioning_measurement_failure_func_t)(const f1ap_positioning_measurement_failure_t *failure);

struct nr_mac_rrc_ul_if_s;
void mac_rrc_ul_direct_init(struct nr_mac_rrc_ul_if_s *mac_rrc);
void mac_rrc_ul_f1ap_init(struct nr_mac_rrc_ul_if_s *mac_rrc);

#endif /* MAC_RRC_UL_H */
