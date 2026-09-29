/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief: Include file defining telnet commands related to softmodem linux process
 */

#ifndef __TELNETSRV_PHYCMD__H__
#define __TELNETSRV_PHYCMD__H__

#ifdef TELNETSRV_PHYCMD_MAIN

#include "common/utils/LOG/log.h"


#include "openair1/PHY/phy_extern.h"



telnetshell_vardef_t phy_vardef[] = {{"", 0, 0, NULL}};

#else

extern void add_phy_cmds(void);

#endif

/*-------------------------------------------------------------------------------------*/

#endif
