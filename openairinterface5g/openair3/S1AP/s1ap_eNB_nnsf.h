/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef S1AP_ENB_NNSF_H_
#define S1AP_ENB_NNSF_H_

struct s1ap_eNB_mme_data_s *
s1ap_eNB_nnsf_select_mme(s1ap_eNB_instance_t       *instance_p,
                         rrc_establishment_cause_t  cause);

struct s1ap_eNB_mme_data_s *
s1ap_eNB_nnsf_select_mme_by_plmn_id(s1ap_eNB_instance_t       *instance_p,
                                    rrc_establishment_cause_t  cause,
                                    int                        selected_plmn_identity);

struct s1ap_eNB_mme_data_s*
s1ap_eNB_nnsf_select_mme_by_mme_code(s1ap_eNB_instance_t       *instance_p,
                                     rrc_establishment_cause_t  cause,
                                     int                        selected_plmn_identity,
                                     uint8_t                    mme_code);

struct s1ap_eNB_mme_data_s*
s1ap_eNB_nnsf_select_mme_by_gummei(s1ap_eNB_instance_t       *instance_p,
                                   rrc_establishment_cause_t  cause,
                                   s1ap_gummei_t                   gummei);

struct s1ap_eNB_mme_data_s*
s1ap_eNB_nnsf_select_mme_by_gummei_no_cause(s1ap_eNB_instance_t       *instance_p,
                                   s1ap_gummei_t                   gummei);

#endif /* S1AP_ENB_NNSF_H_ */
