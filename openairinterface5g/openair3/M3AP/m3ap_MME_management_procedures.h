/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief m3ap tasks for eNB
 */

#ifndef __M3AP_MME_MANAGEMENT_PROCEDURES__H__
#define __M3AP_MME_MANAGEMENT_PROCEDURES__H__

void m3ap_MME_prepare_internal_data(void);

void dump_trees_m2(void);

void m3ap_MME_insert_new_instance(m3ap_MME_instance_t *new_instance_p);

m3ap_MME_instance_t *m3ap_MME_get_instance(uint8_t mod_id);

uint16_t m3ap_MME_fetch_add_global_cnx_id(void);

void m3ap_MME_prepare_internal_data(void);

m3ap_MME_data_t* m3ap_is_MME_id_in_list(uint32_t MME_id);

m3ap_MME_data_t* m3ap_is_MME_assoc_id_in_list(uint32_t sctp_assoc_id);

m3ap_MME_data_t* m3ap_is_MME_pci_in_list (const uint32_t pci);

struct m3ap_MME_data_s *m3ap_get_MME(m3ap_MME_instance_t *instance_p, sctp_assoc_t assoc_id, uint16_t cnx_id);

#endif /* __M3AP_MME_MANAGEMENT_PROCEDURES__H__ */
