/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*! \file openair2/ENB_APP/NB_IoT_interface.c
 * \brief: load library implementing coding/decoding algorithms
 * \date 2018
 * \version 0.1
 * \note
 * \warning
 */
#define _GNU_SOURCE 
#include <sys/types.h>


#include "openair1/PHY/phy_extern.h"
#include "common/utils/load_module_shlib.h" 
#define NBIOT_INTERFACE_SOURCE
#include "NB_IoT_interface.h"

#include "common/ran_context.h"
extern RAN_CONTEXT_t RC;




int load_NB_IoT(void) {
 int ret;
 RCConfig_NbIoT_f_t RCConfig;
 loader_shlibfunc_t shlib_fdesc[]=NBIOT_INTERFACE_FLIST; 

     ret=load_module_shlib(NBIOT_MODULENAME,shlib_fdesc,sizeof(shlib_fdesc)/sizeof(loader_shlibfunc_t),NULL);
     if (ret) {
        return ret;
     }
     RCConfig = get_shlibmodule_fptr(NBIOT_MODULENAME,NBIOT_RCCONFIG_FNAME );
     if (RCConfig == NULL) {
        return -1;
     } 
 
     RCConfig(&RC);
return 0;
}

