/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*! 
 * \brief: Include file defining telnet commands related to softmodem linux process
 */

#ifdef TELNETSRV_LOADER_MAIN

#include "common/utils/LOG/log.h"


#include "common/utils/load_module_shlib.h"

telnetshell_vardef_t loader_globalvardef[] = {{"mainversion", TELNET_VARTYPE_STRING, TELNET_CHECKVAL_RDONLY, &(loader_data.mainexec_buildversion)},
                                              {"defpath", TELNET_VARTYPE_STRING, TELNET_CHECKVAL_RDONLY, &(loader_data.shlibpath)},
                                              {"maxshlibs", TELNET_VARTYPE_INT32, TELNET_CHECKVAL_RDONLY, &(loader_data.maxshlibs)},
                                              {"numshlibs", TELNET_VARTYPE_INT32, TELNET_CHECKVAL_RDONLY, &(loader_data.numshlibs)},
                                              {"", 0, 0, NULL}};
telnetshell_vardef_t *loader_modulesvardef;

extern void add_loader_cmds(void);

#endif

/*-------------------------------------------------------------------------------------*/

