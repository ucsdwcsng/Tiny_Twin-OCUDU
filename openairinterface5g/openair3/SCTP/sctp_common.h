/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 *  \brief eNB/MME SCTP related common procedures
 *  @ingroup _sctp
 */

#ifndef SCTP_COMMON_H_
#define SCTP_COMMON_H_

#include <stdio.h>
#include <stdint.h>
#include <sys/socket.h>

#include "common/utils/LOG/log.h"
#define SCTP_ERROR(x, args...) LOG_E(SCTP, x, ##args)
#define SCTP_DEBUG(x, args...) LOG_D(SCTP, x, ##args)
#define SCTP_WARN(x, args...)  LOG_W(SCTP, x, ##args)

int sctp_set_init_opt(int sd, uint16_t instreams, uint16_t outstreams,
                      uint16_t max_attempts, uint16_t init_timeout);

int sctp_get_sockinfo(int sock, uint16_t *instream, uint16_t *outstream,
                      int32_t *assoc_id);

int sctp_get_peeraddresses(int sock, struct sockaddr **remote_addr,
                           int *nb_remote_addresses);

int sctp_get_localaddresses(int sock, struct sockaddr **local_addr,
                            int *nb_local_addresses);

#endif /* SCTP_COMMON_H_ */
