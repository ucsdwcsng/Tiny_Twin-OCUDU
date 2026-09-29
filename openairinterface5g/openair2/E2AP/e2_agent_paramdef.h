/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef E2_AGENT_PARAMDEF_H_
#define E2_AGENT_PARAMDEF_H_

/* E2 Agent configuration */
#define CONFIG_STRING_E2AGENT "e2_agent"

#define E2AGENT_CONFIG_IP    "near_ric_ip_addr"
#define E2AGENT_CONFIG_SMDIR "sm_dir"

static const char* const e2agent_config_ip_default = "127.0.0.1";
static const char* const e2agent_config_smdir_default = ".";

#define E2AGENT_PARAMS_DESC { \
  {E2AGENT_CONFIG_IP,    "RIC IP address",             0, strptr:NULL, defstrval:(char*)e2agent_config_ip_default,    TYPE_STRING, 0}, \
  {E2AGENT_CONFIG_SMDIR, "Directory with SMs to load", 0, strptr:NULL, defstrval:(char*)e2agent_config_smdir_default, TYPE_STRING, 0}, \
}

#define E2AGENT_CONFIG_IP_IDX    0
#define E2AGENT_CONFIG_SMDIR_IDX 1

#endif 
