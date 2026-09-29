/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef _NR_PDCP_INTEGRITY_NIA1_H_
#define _NR_PDCP_INTEGRITY_NIA1_H_

#include "openair3/SECU/secu_defs.h"

stream_security_context_t *nr_pdcp_integrity_nia1_init(unsigned char *integrity_key);

void nr_pdcp_integrity_nia1_integrity(stream_security_context_t *integrity_context,
                            unsigned char *out,
                            unsigned char *buffer, int length,
                            int bearer, uint32_t count, int direction);

void nr_pdcp_integrity_nia1_free_integrity(stream_security_context_t *integrity_context);

#endif /* _NR_PDCP_INTEGRITY_NIA1_H_ */
