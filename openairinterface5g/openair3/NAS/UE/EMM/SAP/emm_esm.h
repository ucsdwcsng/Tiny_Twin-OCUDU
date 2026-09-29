/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*****************************************************************************

Source      emm_esm.h

Version     0.1

Date        2012/10/16

Product     NAS stack

Subsystem   EPS Mobility Management

Description Defines the EMMESM Service Access Point that provides
        interlayer services to the EPS Session Management sublayer
        for service registration and activate/deactivate PDP context.

*****************************************************************************/
#ifndef __EMM_ESM_H__
#define __EMM_ESM_H__

#include "emm_esmDef.h"
#include "user_defs.h"

/****************************************************************************/
/*********************  G L O B A L    C O N S T A N T S  *******************/
/****************************************************************************/

/****************************************************************************/
/************************  G L O B A L    T Y P E S  ************************/
/****************************************************************************/

/****************************************************************************/
/********************  G L O B A L    V A R I A B L E S  ********************/
/****************************************************************************/

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

void emm_esm_initialize(void);

int emm_esm_send(nas_user_t *user, const emm_esm_t *msg);

#endif /* __EMM_ESM_H__*/
