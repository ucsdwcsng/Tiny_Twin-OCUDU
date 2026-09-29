/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*
                                mce_config.h
                             -------------------
  AUTHOR  : Javier Morgade
  COMPANY : VICOMTECH, Spain
  EMAIL   : javier.morgade@ieee.org
*/

#ifndef MCE_CONFIG_H_
#define MCE_CONFIG_H_
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

#define IPV4_STR_ADDR_TO_INT_NWBO(AdDr_StR,NwBo,MeSsAgE ) do {\
            struct in_addr inp;\
            if ( inet_aton(AdDr_StR, &inp ) < 0 ) {\
                AssertFatal (0, MeSsAgE);\
            } else {\
                NwBo = inp.s_addr;\
            }\
        } while (0);


void read_config_and_init(void);

int RCconfig_MCE(void);
int RCconfig_M3(MessageDef *msg_p, uint32_t i);
int RCconfig_M2_MCCH(MessageDef *msg_p, uint32_t i);
int RCconfig_m2_mcch(m2ap_setup_resp_t * m2ap_setup_resp, uint32_t i);
int RCconfig_M2_SCHEDULING(MessageDef *msg_p, uint32_t i);
int RCconfig_m2_scheduling(m2ap_mbms_scheduling_information_t *m2ap_mbms_scheduling_information, uint32_t i);

#endif /* MCE_CONFIG_H_ */
