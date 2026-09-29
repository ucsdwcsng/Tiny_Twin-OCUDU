/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef _NR_SDAP_CONFIGURATION_H_
#define _NR_SDAP_CONFIGURATION_H_

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  // SDAP Headers
  bool header_dl_absent;
  bool header_ul_absent;
  // Default DRB
  uint8_t default_drb;
} nr_sdap_configuration_t;

#endif /* _NR_SDAP_CONFIGURATION_H_ */
