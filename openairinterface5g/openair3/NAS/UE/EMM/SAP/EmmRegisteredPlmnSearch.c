/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*****************************************************************************
Source      EmmRegisteredPlmnSearch.c

Version     0.1

Date        2012/10/03

Product     NAS stack

Subsystem   EPS Mobility Management

Description Implements the EPS Mobility Management procedures executed
        when the EMM-SAP is in EMM-REGISTERED.PLMN-SEARCH state.

        In EMM-REGISTERED.PLMN-SEARCH state, the UE is searching
        for PLMNs.

*****************************************************************************/


#include "emm_fsm.h"
#include "commonDef.h"
#include "nas_log.h"

#include <assert.h>

/****************************************************************************/
/****************  E X T E R N A L    D E F I N I T I O N S  ****************/
/****************************************************************************/

/****************************************************************************/
/*******************  L O C A L    D E F I N I T I O N S  *******************/
/****************************************************************************/

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

/****************************************************************************
 **                                                                        **
 ** Name:    EmmRegisteredPlmnSearch()                                 **
 **                                                                        **
 ** Description: Handles the behaviour of the UE while the EMM-SAP is in   **
 **      EMM-REGISTERED.PLMN-SEARCH state.                         **
 **                                                                        **
 ** Inputs:  evt:       The received EMM-SAP event                 **
 **      Others:    emm_fsm_status                             **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    emm_fsm_status                             **
 **                                                                        **
 ***************************************************************************/
int EmmRegisteredPlmnSearch(nas_user_t *user, const emm_reg_t *evt)
{
  LOG_FUNC_IN;

  assert(emm_fsm_get_status(user) == EMM_REGISTERED_PLMN_SEARCH);

  /* TODO */

  LOG_FUNC_RETURN (RETURNok);
}
