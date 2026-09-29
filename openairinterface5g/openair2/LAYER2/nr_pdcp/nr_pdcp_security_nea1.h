/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef _NR_PDCP_SECURITY_NEA1_H_
#define _NR_PDCP_SECURITY_NEA1_H_

#include "openair3/SECU/secu_defs.h"

stream_security_context_t *nr_pdcp_security_nea1_init(unsigned char *ciphering_key);

void nr_pdcp_security_nea1_cipher(stream_security_context_t *security_context,
                                  unsigned char *buffer,
                                  int length,
                                  int bearer,
                                  uint32_t count,
                                  int direction);

void nr_pdcp_security_nea1_free_security(stream_security_context_t *security_context);

#endif /* _NR_PDCP_SECURITY_NEA1_H_ */
