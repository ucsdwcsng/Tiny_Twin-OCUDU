/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief cli interface
 * @ingroup util
 */
#ifndef __CLI_IF_H__
#    define __CLI_IF_H__


/*--- INCLUDES ---------------------------------------------------------------*/
#    include "cli.h"
/*----------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup _cli_if Interfaces of CLI
 * @{*/


void cli_init (void); 
int cli_server_init(cli_handler_t handler); 
void cli_server_cleanup(void);
void cli_server_recv(const void * data, socklen_t len);
/* @}*/

#ifdef __cplusplus
}
#endif

#endif

