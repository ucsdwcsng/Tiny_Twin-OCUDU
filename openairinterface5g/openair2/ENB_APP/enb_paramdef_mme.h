/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief definition of configuration parameters for MME modules 
 */

#ifndef __ENB_APP_ENB_PARAMDEF_MME__H__
#define __ENB_APP_ENB_PARAMDEF_MME__H__

#include "common/config/config_paramdesc.h"
#include "RRC_paramsvalues.h"

#define MME_CONFIG_STRING_NETWORK_INTERFACES_CONFIG     "NETWORK_INTERFACES"

/* interface configuration parameters names   */
/* M3 interface configuration parameters names */
#define MME_CONFIG_STRING_MME_INTERFACE_NAME_FOR_M3_MCE "MME_INTERFACE_NAME_FOR_M3_MCE"
#define MME_CONFIG_STRING_MME_IPV4_ADDRESS_FOR_M3C "MME_IPV4_ADDRESS_FOR_M3C"
#define MME_CONFIG_STRING_MME_PORT_FOR_M3C "MME_PORT_FOR_M3C"


#define MME_NETPARAMS_DESC {  \
{MME_CONFIG_STRING_MME_INTERFACE_NAME_FOR_M3_MCE,        NULL,      0,        .strptr=&mme_interface_name_for_m3_mce,   .defstrval="lo",      TYPE_STRING,      0},      \
{MME_CONFIG_STRING_MME_IPV4_ADDRESS_FOR_M3C,             NULL,      0,        .strptr=&mme_ipv4_address_for_m3c,        .defstrval="127.0.0.18/24",      TYPE_STRING,      0},      \
{MME_CONFIG_STRING_MME_PORT_FOR_M3C,                     NULL,      0,        .uptr=&mme_port_for_m3c,           	.defintval=36444L,    TYPE_UINT,        0},      \
} 

#endif
