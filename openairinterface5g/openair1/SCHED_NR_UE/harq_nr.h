/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/***********************************************************************
*
* FILENAME    :  harq_nr.h
*
* MODULE      :  HARQ
*
* DESCRIPTION :  header related to Hybrid Automatic Repeat Request Acknowledgment
*                This feature allows to acknowledge downlink and uplink transport blocks
*
************************************************************************/

#ifndef HARQ_NR_H
#define HARQ_NR_H

/************** DEFINE ********************************************/

#define NR_DEFAULT_DLSCH_HARQ_PROCESSES (8) /* TS 38.214 5.1 */

/************** INCLUDE *******************************************/

#include "PHY/defs_nr_UE.h"

/************* TYPE ***********************************************/


/************** VARIABLES *****************************************/


/*************** FUNCTIONS ****************************************/

/** \brief This function initialises downlink HARQ status
    @param pointer to downlink harq status
    @returns none */

void init_downlink_harq_status(NR_DL_UE_HARQ_t *dl_harq);


/** \brief This function update downlink harq context and return reception type (new transmission or retransmission)
    @param dlsch downlink harq context
    @param harq process identifier harq_pid
    @param rnti_type type of rnti
    @returns retransmission or new transmission */

void downlink_harq_process(NR_DL_UE_HARQ_t *dlsch, int harq_pid, int ndi, int rv, uint8_t rnti_type);

#undef EXTERN
#undef INIT_VARIABLES_HARQ_NR_H

#endif /* HARQ_NR_H */
