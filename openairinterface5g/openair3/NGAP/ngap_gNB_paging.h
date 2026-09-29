/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef NGAP_GNB_PAGING_H_
#define NGAP_GNB_PAGING_H_

#include <stdbool.h>
#include <stdint.h>
#include "ngap_messages_types.h"

/** @brief Encode NGAP Paging message (9.2.4.1 of 3GPP TS 38.413)
 * @param msg Pointer to paging indication structure to encode
 * @return Pointer to allocated NGAP_NGAP_PDU_t, or NULL on failure */
NGAP_NGAP_PDU_t *encode_ng_paging(const ngap_paging_ind_t *msg);

/** @brief Decode NGAP Paging message (9.2.4.1 of 3GPP TS 38.413)
 * @param out Pointer to output structure to fill
 * @param pdu Pointer to NGAP PDU containing the Paging message
 * @return true on success, false on failure */
bool decode_ng_paging(ngap_paging_ind_t *out, const NGAP_NGAP_PDU_t *pdu);

/** @brief Free memory allocated for NGAP paging indication
 * @param msg Pointer to paging indication structure to free */
void free_ng_paging(ngap_paging_ind_t *msg);

/** @brief Check equality of two NGAP paging indication structures
 * @param a First structure
 * @param b Second structure
 * @return true if equal, false otherwise  */
bool eq_ng_paging(const ngap_paging_ind_t *a, const ngap_paging_ind_t *b);

#endif /* NGAP_GNB_PAGING_H_ */
