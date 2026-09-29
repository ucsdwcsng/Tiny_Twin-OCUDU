/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief ngap procedures for overload messages within gNB
 */

#ifndef NGAP_GNB_OVERLOAD_H_
#define NGAP_GNB_OVERLOAD_H_

#include <netinet/in.h>
#include <netinet/sctp.h>
#include <stdint.h>
#include "NGAP_NGAP-PDU.h"

/**
 * \brief Handle an overload stop message
 **/
int ngap_gNB_handle_overload_stop(sctp_assoc_t assoc_id, uint32_t stream, NGAP_NGAP_PDU_t *pdu);

#endif /* NGAP_GNB_OVERLOAD_H_ */
