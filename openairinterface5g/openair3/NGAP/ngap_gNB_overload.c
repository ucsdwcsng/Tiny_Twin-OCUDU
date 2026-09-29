/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief ngap procedures for overload messages within gNB
 */

#include "ngap_gNB_overload.h"
#include <stdint.h>
#include <stdio.h>
#include "assertions.h"
#include "ngap_gNB_defs.h"
#include "ngap_gNB_management_procedures.h"

int ngap_gNB_handle_overload_stop(sctp_assoc_t assoc_id, uint32_t stream, NGAP_NGAP_PDU_t *pdu)
{
    /* We received Overload stop message, meaning that the AMF is no more
     * overloaded. This is an empty message, with only message header and no
     * Information Element.
     */
    DevAssert(pdu != NULL);

    ngap_gNB_amf_data_t *amf_desc_p;

    /* Non UE-associated signalling -> stream 0 */
    DevCheck(stream == 0, stream, 0, 0);

    if ((amf_desc_p = ngap_gNB_get_AMF(NULL, assoc_id, 0)) == NULL) {
        /* No AMF context associated */
        return -1;
    }

    amf_desc_p->state = NGAP_GNB_STATE_CONNECTED;
    amf_desc_p->overload_state = NGAP_NO_OVERLOAD;
    return 0;
}
