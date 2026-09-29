/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright 2017 Cisco Systems, Inc.
 */


#ifndef _PNF_H_
#define _PNF_H_

#include "nfapi_pnf_interface.h"

typedef struct pnf_s {
  nfapi_pnf_config_t _public;

  int p5_sock;
	uint8_t tx_message_buffer[NFAPI_MAX_PACKED_MESSAGE_SIZE];

	uint8_t sctp;

	uint8_t terminate;

} pnf_t;

int pnf_connect(pnf_t *pnf);
int pnf_message_pump(pnf_t *pnf);

void pnf_nr_handle_p5_message(pnf_t* pnf, void* pRecvMsg, int recvMsgLen);
int pnf_pack_and_send_p5_message(pnf_t* pnf, nfapi_p4_p5_message_header_t* msg, uint32_t msg_len);
int pnf_pack_and_send_p4_message(pnf_t* pnf, nfapi_p4_p5_message_header_t* msg, uint32_t msg_len);
int pnf_send_message(pnf_t* pnf, uint8_t* msg, uint32_t msg_len, uint16_t stream_id);

nfapi_pnf_phy_config_t* nfapi_pnf_phy_config_find(nfapi_pnf_config_t* config, uint16_t phy_id);

void pnf_nr_handle_pnf_param_request(pnf_t* pnf, void *pRecvMsg, int recvMsgLen);
void pnf_nr_handle_pnf_config_request(pnf_t* pnf, void *pRecvMsg, int recvMsgLen);
void pnf_nr_handle_pnf_start_request(pnf_t* pnf, void *pRecvMsg, int recvMsgLen);
void pnf_handle_pnf_stop_request(pnf_t* pnf, void *pRecvMsg, int recvMsgLen);
void pnf_nr_handle_param_request(pnf_t* pnf, void *pRecvMsg, int recvMsgLen);
void pnf_nr_handle_config_request(pnf_t* pnf, void *pRecvMsg, int recvMsgLen);
void pnf_nr_handle_start_request(pnf_t* pnf, void *pRecvMsg, int recvMsgLen);
void pnf_nr_handle_stop_request(pnf_t* pnf, void* pRecvMsg, int recvMsgLen);


#endif // _PNF_H_

