/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*
  mme_config.h
  AUTHOR  : Javier Morgade
  COMPANY : Vicomtech, Spain
  EMAIL   : javier.morgade@ieee.org
*/

#ifndef __MME_APP_MME_CONFIG__H__
#define __MME_APP_MME_CONFIG__H__

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <libconfig.h>

#include "commonDef.h"
#include "common/platform_types.h"
#include "common/platform_constants.h"
#include "PHY/impl_defs_top.h"
#include "PHY/defs_eNB.h"
#include "s1ap_messages_types.h"
#include "LTE_SystemInformationBlockType2.h"
#include "rrc_messages_types.h"
#include "RRC/LTE/rrc_defs.h"
#include <intertask_interface.h>
#include "enb_paramdef.h"

int RCconfig_MME(void);

#endif /* __MME_APP_MME_CONFIG__H__ */
/** @} */
