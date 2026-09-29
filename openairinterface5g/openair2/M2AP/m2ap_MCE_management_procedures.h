/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief m2ap tasks for eNB
 */

#ifndef __M2AP_MCE_MANAGEMENT_PROCEDURES__H__
#define __M2AP_MCE_MANAGEMENT_PROCEDURES__H__

void m2ap_MCE_prepare_internal_data(void);

void dump_trees_m2(void);

void m2ap_MCE_insert_new_instance(m2ap_MCE_instance_t *new_instance_p);

m2ap_MCE_instance_t *m2ap_MCE_get_instance(uint8_t mod_id);

uint16_t m2ap_MCE_fetch_add_global_cnx_id(void);

void m2ap_MCE_prepare_internal_data(void);

m2ap_MCE_data_t* m2ap_is_MCE_id_in_list(uint32_t MCE_id);

m2ap_MCE_data_t* m2ap_is_MCE_assoc_id_in_list(uint32_t sctp_assoc_id);

m2ap_MCE_data_t* m2ap_is_MCE_pci_in_list (const uint32_t pci);

struct m2ap_MCE_data_s *m2ap_get_MCE(m2ap_MCE_instance_t *instance_p, sctp_assoc_t assoc_id, uint16_t cnx_id);

#endif /* __M2AP_MCE_MANAGEMENT_PROCEDURES__H__ */
