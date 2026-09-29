/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright 2017 Cisco Systems, Inc.
 */

#ifndef _MAC_H_
#define _MAC_H_

#include "nfapi_interface.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct mac mac_t;

typedef struct mac
{
	void* user_data;

	void (*dl_config_req)(mac_t* mac, nfapi_dl_config_request_t* req);
	void (*ul_config_req)(mac_t* mac, nfapi_ul_config_request_t* req);
	void (*hi_dci0_req)(mac_t* mac, nfapi_hi_dci0_request_t* req);
	void (*tx_req)(mac_t* mac, nfapi_tx_request_t* req);
} mac_t;

mac_t* mac_create(uint8_t wireshark_test_mode);
void mac_destroy(mac_t* mac);

void mac_start_data(mac_t* mac, unsigned rx_port, const char* tx_addres, unsigned tx_port);

void mac_subframe_ind(mac_t* mac, uint16_t phy_id, uint16_t sfn_sf);
void mac_harq_ind(mac_t* mac, nfapi_harq_indication_t* ind);
void mac_crc_ind(mac_t* mac, nfapi_crc_indication_t* ind);
void mac_rx_ind(mac_t* mac, nfapi_rx_indication_t* ind);
void mac_rach_ind(mac_t* mac, nfapi_rach_indication_t* ind);
void mac_srs_ind(mac_t* mac, nfapi_srs_indication_t* ind);
void mac_sr_ind(mac_t* mac, nfapi_sr_indication_t* ind);
void mac_cqi_ind(mac_t* mac, nfapi_cqi_indication_t* ind);
void mac_lbt_dl_ind(mac_t* mac, nfapi_lbt_dl_indication_t* ind);
void mac_nb_harq_ind(mac_t* mac, nfapi_nb_harq_indication_t* ind);
void mac_nrach_ind(mac_t* mac, nfapi_nrach_indication_t* ind);


#if defined(__cplusplus)
}
#endif

#endif
