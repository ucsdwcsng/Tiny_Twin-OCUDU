/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*****************************************************************************

Source      emm_fsm.h

Version     0.1

Date        2012/10/03

Product     NAS stack

Subsystem   EPS Mobility Management

Description Defines the EPS Mobility Management procedures executed at
        the EMMREG Service Access Point.

*****************************************************************************/
#ifndef __EMM_FSM_H__
#define __EMM_FSM_H__

#include "emm_regDef.h"
#include "emm_fsm_defs.h"
#include "user_defs.h"

/****************************************************************************/
/*********************  G L O B A L    C O N S T A N T S  *******************/
/****************************************************************************/

/****************************************************************************/
/********************  G L O B A L    V A R I A B L E S  ********************/
/****************************************************************************/

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

emm_fsm_state_t emm_fsm_initialize(void);

int emm_fsm_set_status(nas_user_t *user, emm_fsm_state_t status);
emm_fsm_state_t emm_fsm_get_status(nas_user_t *user);


int emm_fsm_process(nas_user_t *user, const emm_reg_t *evt);

#endif /* __EMM_FSM_H__*/
