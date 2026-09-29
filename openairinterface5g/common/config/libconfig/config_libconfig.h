/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief configuration module, include file for libconfig implementation 
 */

#ifndef INCLUDE_CONFIG_LIBCONFIG_H
#define INCLUDE_CONFIG_LIBCONFIG_H


#ifdef __cplusplus
extern "C"
{
#endif

#include "common/config/config_paramdesc.h"



 
typedef struct libconfig_privatedata {
      char *configfile;
      config_t cfg;
      config_t runtcfg;
} libconfig_privatedata_t;
 
#ifdef __cplusplus
}
#endif
#endif  /* INCLUDE_CONFIG_LIBCONFIG_H */
