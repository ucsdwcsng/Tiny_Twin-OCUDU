/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef NRPPA_GNB_HANDLERS_H_
#define NRPPA_GNB_HANDLERS_H_

int nrppa_handle_downlink_ue_associated_nrppa_transport(instance_t instance, const ngap_downlink_ue_associated_nrppa_t *msg);
int nrppa_handle_downlink_non_ue_associated_nrppa_transport(instance_t instance,
                                                            const ngap_downlink_non_ue_associated_nrppa_t *msg);
#endif /* NRPPA_GNB_HANDLERS_H_ */
