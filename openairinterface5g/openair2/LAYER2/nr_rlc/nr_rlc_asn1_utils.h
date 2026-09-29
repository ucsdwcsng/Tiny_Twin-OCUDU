/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef _OPENAIR2_LAYER2_NR_RLC_ASN1_UTILS_H_
#define _OPENAIR2_LAYER2_NR_RLC_ASN1_UTILS_H_

int decode_t_reassembly(int v);
int decode_t_status_prohibit(int v);
int decode_t_poll_retransmit(int v);
int decode_poll_pdu(int v);
int decode_poll_byte(int v);
int decode_max_retx_threshold(int v);
int decode_sn_field_length_um(int v);
int decode_sn_field_length_am(int v);

int encode_t_reassembly(int v);
int encode_t_status_prohibit(int v);
int encode_t_poll_retransmit(int v);
int encode_poll_pdu(int v);
int encode_poll_byte(int v);
int encode_max_retx_threshold(int v);
int encode_sn_field_length_um(int v);
int encode_sn_field_length_am(int v);

#endif /* _OPENAIR2_LAYER2_NR_RLC_ASN1_UTILS_H_ */
