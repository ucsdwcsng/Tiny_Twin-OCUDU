/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef _OPENAIR2_LAYER2_RLC_V2_ASN1_UTILS_H_
#define _OPENAIR2_LAYER2_RLC_V2_ASN1_UTILS_H_

int decode_t_reordering(int v);
int decode_t_status_prohibit(int v);
int decode_t_poll_retransmit(int v);
int decode_poll_pdu(int v);
int decode_poll_byte(int v);
int decode_max_retx_threshold(int v);
int decode_sn_field_length(int v);

#endif /* _OPENAIR2_LAYER2_RLC_V2_ASN1_UTILS_H_ */
