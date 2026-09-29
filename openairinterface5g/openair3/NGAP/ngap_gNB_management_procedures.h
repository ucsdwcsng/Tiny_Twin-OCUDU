/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief NGAP gNB task 
 * @ingroup _ngap
 */

#include "ds/byte_array.h"
#include "BIT_STRING.h"

#ifndef NGAP_GNB_MANAGEMENT_PROCEDURES_H_
#define NGAP_GNB_MANAGEMENT_PROCEDURES_H_

struct ngap_gNB_amf_data_s *ngap_gNB_get_AMF(ngap_gNB_instance_t *instance_p, sctp_assoc_t assoc_id, uint16_t cnx_id);

struct ngap_gNB_amf_data_s *ngap_gNB_get_AMF_from_instance(ngap_gNB_instance_t *instance_p);

void ngap_gNB_remove_amf_desc(ngap_gNB_instance_t * instance);

void ngap_gNB_insert_new_instance(ngap_gNB_instance_t *new_instance_p);

ngap_gNB_instance_t *ngap_gNB_get_instance(uint8_t mod_id);

uint16_t ngap_gNB_fetch_add_global_cnx_id(void);

void ngap_gNB_prepare_internal_data(void);

#endif /* NGAP_GNB_MANAGEMENT_PROCEDURES_H_ */
