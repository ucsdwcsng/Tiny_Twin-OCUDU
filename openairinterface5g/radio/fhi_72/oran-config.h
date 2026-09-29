/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef ORAN_CONFIG_H
#define ORAN_CONFIG_H

#include "stdbool.h"
#include "stdint.h"

struct xran_fh_init;
struct xran_fh_config;
struct openair0_config;

bool get_xran_config(void *mplane_api, const struct openair0_config *openair0_cfg, struct xran_fh_init *fh_init, struct xran_fh_config *fh_config);

void print_fh_init(const struct xran_fh_init *fh_init);
void print_fh_config(const struct xran_fh_config *fh_config);

// hack to workaround LiteOn limitation
extern int g_kbar;

#endif /* ORAN_CONFIG_H */
