/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief mac phy interface primitives
 */

#ifndef __PHY_INTERFACE_H__
#    define __PHY_INTERFACE_H__

#include "LAYER2/MAC/mac.h"

#define DCI 0
#define DLSCH 1
#define ULSCH 2

#define mac_exit_wrapper(sTRING)                                                            \
do {                                                                                        \
    char temp[300];                                                                         \
    snprintf(temp, sizeof(temp), "%s in file "__FILE__" at line %d\n", sTRING, __LINE__);   \
    mac_xface->macphy_exit(temp);                                                           \
} while(0)

#endif
