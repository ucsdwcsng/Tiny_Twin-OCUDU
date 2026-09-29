/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/**********************************************************************
*
* FILENAME    :  pucch_uci_ue_nr.h
*
* MODULE      :  Packed Uplink Control Channel aka PUCCH
*                PUCCH is used to trasnmit Uplink Control Information UCI
*                which is composed of:
*                - SR Scheduling Request
*                - HARQ ACK/NACK
*                - CSI Channel State Information
*
* DESCRIPTION :  functions related to PUCCH management
*                TS 38.213 9  UE procedure for reporting control information
*
************************************************************************/

#ifndef PUCCH_UCI_UE_NR_H
#define PUCCH_UCI_UE_NR_H

/************** INCLUDE *******************************************/

#include "PHY/defs_nr_UE.h"

/************** DEFINE ********************************************/

#define BITS_PER_SYMBOL_BPSK  (1)     /* 1 bit per symbol for bpsk modulation */
#define BITS_PER_SYMBOL_QPSK  (2)     /* 2 bits per symbol for bpsk modulation */


/*************** FUNCTIONS ****************************************/

void pucch_procedures_ue_nr(PHY_VARS_NR_UE *ue, const UE_nr_rxtx_proc_t *proc, nr_phy_data_tx_t *phy_data, c16_t **txdataF, bool was_symbol_used[14]);

#endif /* PUCCH_UCI_UE_NR_H */
