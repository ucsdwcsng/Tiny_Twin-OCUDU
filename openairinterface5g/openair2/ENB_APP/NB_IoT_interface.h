/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*! \file openair2/ENB_APP/NB_IoT_interface.h
 * \brief: api interface for nb-iot application
 * \date 2018
 * \version 0.1
 * \note
 * \warning
 */
#ifndef NBIOT_INTERFACE_H
#define NBIOT_INTERFACE_H


#define NBIOT_MODULENAME "NB_IoT"
#include "common/ran_context.h"

typedef void(*RCConfig_NbIoT_f_t)(RAN_CONTEXT_t *RC);
#define NBIOT_RCCONFIG_FNAME "RCConfig_NbIoT"

#ifdef NBIOT_INTERFACE_SOURCE

#define NBIOT_INTERFACE_FLIST {\
{NBIOT_RCCONFIG_FNAME,NULL},\
}

#else /* NBIOT_INTERFACE_SOURCE */

extern int load_NB_IoT(void); 

#endif

#endif
