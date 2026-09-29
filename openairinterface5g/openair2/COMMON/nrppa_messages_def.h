/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "openair2/COMMON/nrppa_messages_types.h"

// LMF -> CU
MESSAGE_DEF(NRPPA_TRP_INFORMATION_REQ, MESSAGE_PRIORITY_MED, nrppa_trp_information_req_t, nrppa_trp_information_req)
MESSAGE_DEF(NRPPA_TRP_INFORMATION_RESP, MESSAGE_PRIORITY_MED, nrppa_trp_information_resp_t, nrppa_trp_information_resp)
MESSAGE_DEF(NRPPA_TRP_INFORMATION_FAILURE, MESSAGE_PRIORITY_MED, nrppa_trp_information_failure_t, nrppa_trp_information_failure)
MESSAGE_DEF(NRPPA_POSITIONING_INFORMATION_REQ,
            MESSAGE_PRIORITY_MED,
            nrppa_positioning_information_req_t,
            nrppa_positioning_information_req)
MESSAGE_DEF(NRPPA_POSITIONING_INFORMATION_RESP,
            MESSAGE_PRIORITY_MED,
            nrppa_positioning_information_resp_t,
            nrppa_positioning_information_resp)
MESSAGE_DEF(NRPPA_POSITIONING_ACTIVATION_REQ,
            MESSAGE_PRIORITY_MED,
            nrppa_positioning_activation_req_t,
            nrppa_positioning_activation_req)
MESSAGE_DEF(NRPPA_POSITIONING_ACTIVATION_RESP,
            MESSAGE_PRIORITY_MED,
            nrppa_positioning_activation_resp_t,
            nrppa_positioning_activation_resp)
MESSAGE_DEF(NRPPA_MEASUREMENT_REQ, MESSAGE_PRIORITY_MED, nrppa_measurement_req_t, nrppa_measurement_req)
MESSAGE_DEF(NRPPA_MEASUREMENT_RESP, MESSAGE_PRIORITY_MED, nrppa_measurement_resp_t, nrppa_measurement_resp)
MESSAGE_DEF(NRPPA_MEASUREMENT_FAILURE, MESSAGE_PRIORITY_MED, nrppa_measurement_failure_t, nrppa_measurement_failure)
