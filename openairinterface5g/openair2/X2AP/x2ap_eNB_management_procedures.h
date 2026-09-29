/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief x2ap tasks for eNB
 */

#ifndef __X2AP_ENB_MANAGEMENT_PROCEDURES__H__
#define __X2AP_ENB_MANAGEMENT_PROCEDURES__H__

void x2ap_eNB_prepare_internal_data(void);

void dump_trees(void);

void x2ap_eNB_insert_new_instance(x2ap_eNB_instance_t *new_instance_p);

x2ap_eNB_instance_t *x2ap_eNB_get_instance(uint8_t mod_id);

uint16_t x2ap_eNB_fetch_add_global_cnx_id(void);

void x2ap_eNB_prepare_internal_data(void);

x2ap_eNB_data_t* x2ap_is_eNB_id_in_list(uint32_t eNB_id);

x2ap_eNB_data_t* x2ap_is_eNB_assoc_id_in_list(uint32_t sctp_assoc_id);

x2ap_eNB_data_t* x2ap_is_eNB_pci_in_list (const uint32_t pci);

struct x2ap_eNB_data_s *x2ap_get_eNB(x2ap_eNB_instance_t *instance_p, sctp_assoc_t assoc_id, uint16_t cnx_id);

#endif /* __X2AP_ENB_MANAGEMENT_PROCEDURES__H__ */
