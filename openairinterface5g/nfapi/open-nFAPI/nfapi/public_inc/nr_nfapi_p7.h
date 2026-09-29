/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef OPENAIRINTERFACE_NR_NFAPI_P7_H
#define OPENAIRINTERFACE_NR_NFAPI_P7_H
#include "nfapi_common_interface.h"

void *nfapi_p7_allocate(size_t size, nfapi_p7_codec_config_t *config);

uint8_t pack_nr_dl_node_sync(void *msg, uint8_t **ppWritePackedMsg, uint8_t *end, nfapi_p7_codec_config_t *config);

uint8_t pack_nr_ul_node_sync(void *msg, uint8_t **ppWritePackedMsg, uint8_t *end, nfapi_p7_codec_config_t *config);

uint8_t pack_nr_timing_info(void *msg, uint8_t **ppWritePackedMsg, uint8_t *end, nfapi_p7_codec_config_t *config);

#endif // OPENAIRINTERFACE_NR_NFAPI_P7_H
