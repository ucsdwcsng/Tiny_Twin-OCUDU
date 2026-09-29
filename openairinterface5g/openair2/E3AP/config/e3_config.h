/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef E3_CONFIG_H
#define E3_CONFIG_H

#include <stdint.h>

/* link_layer values (match libe3 e3_config_t; -1 = libe3 default) */
#define E3_LINK_ZMQ 0
#define E3_LINK_POSIX 1

/* transport_layer values (match libe3 e3_config_t; -1 = libe3 default) */
#define E3_TRANSPORT_SCTP 0
#define E3_TRANSPORT_TCP 1
#define E3_TRANSPORT_IPC 2

/* encoding values (match libe3 e3_config_t; -1 = libe3 default) */
#define E3_ENCODING_ASN1 0
#define E3_ENCODING_JSON 1

typedef struct {
  int link_layer; /* E3_LINK_{ZMQ,POSIX}; -1 = libe3 default */
  int transport_layer; /* E3_TRANSPORT_{SCTP,TCP,IPC}; -1 = libe3 default */
  int encoding; /* E3_ENCODING_{ASN1,JSON}; -1 = libe3 default */
  uint16_t setup_port; /* 0 = libe3 default (9990) */
  uint16_t subscriber_port; /* 0 = libe3 default */
  uint16_t publisher_port; /* 0 = libe3 default */
} e3_cmdline_config_t;

/**
 * @brief Read E3 configuration from the OAI config file
 * @param config Pointer to configuration structure to fill
 */
void e3_readconfig(e3_cmdline_config_t *config);

#endif // E3_CONFIG_H
