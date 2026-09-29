/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*****************************************************************************
Source      esm_main.h

Version     0.1

Date        2012/12/04

Product     NAS stack

Subsystem   EPS Session Management

Description Defines the EPS Session Management procedure call manager,
        the main entry point for elementary ESM processing.

*****************************************************************************/

#ifndef __ESM_MAIN_H__
#define __ESM_MAIN_H__

#include "networkDef.h"
#include "esm_ebr.h"
#include "esmData.h"
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

void esm_main_initialize(nas_user_t *user, esm_indication_callback_t cb);

void esm_main_cleanup(esm_data_t *esm_data);


/* User's getter for PDN connections and EPS bearer contexts */
int esm_main_get_nb_pdns_max(esm_data_t *esm_data);
int esm_main_get_nb_pdns(esm_data_t *esm_data);
bool esm_main_has_emergency(esm_data_t *esm_data);
bool esm_main_get_pdn_status(nas_user_t *user, int cid, bool *state);
int esm_main_get_pdn(esm_data_t *esm_data, int cid, int *type, const char **apn, bool *is_emergency, bool *is_active);
int esm_main_get_pdn_addr(esm_data_t *esm_data, int cid, const char **ipv4addr, const char **ipv6addr);


#endif /* __ESM_MAIN_H__*/
