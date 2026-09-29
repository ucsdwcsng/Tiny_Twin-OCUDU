/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief ngap NAS node selection functions
 */
 
#ifndef NGAP_GNB_NNSF_H_
#define NGAP_GNB_NNSF_H_

#include <stdint.h>
#include "ngap_gNB_defs.h"
#include "ngap_messages_types.h"

ngap_gNB_amf_data_t *ngap_gNB_nnsf_select_amf(ngap_gNB_instance_t *instance_p, const ngap_rrc_establishment_cause_t cause);

ngap_gNB_amf_data_t *ngap_gNB_nnsf_select_amf_by_plmn_id(ngap_gNB_instance_t *instance_p,
                                                         const ngap_rrc_establishment_cause_t cause,
                                                         const plmn_id_t selected_plmn_identity);

ngap_gNB_amf_data_t *ngap_gNB_nnsf_select_amf_by_amf_setid(ngap_gNB_instance_t *instance_p,
                                                           const ngap_rrc_establishment_cause_t cause,
                                                           const plmn_id_t selected_plmn_identity,
                                                           uint16_t amf_setid);

ngap_gNB_amf_data_t *ngap_gNB_nnsf_select_amf_by_guami(ngap_gNB_instance_t *instance_p,
                                                       const ngap_rrc_establishment_cause_t cause,
                                                       const nr_guami_t guami);

ngap_gNB_amf_data_t *ngap_gNB_nnsf_select_amf_by_guami_no_cause(ngap_gNB_instance_t *instance_p, const nr_guami_t guami);

#endif /* NGAP_GNB_NNSF_H_ */
