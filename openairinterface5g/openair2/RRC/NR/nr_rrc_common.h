/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef __NR_RRC_COMMON_H__
#define __NR_RRC_COMMON_H__

#include <stdint.h> 
#include "NR_BWP-Downlink.h"

#define NR_NUM_SRB 4 /* Number of Signalling Radio Bearers according to clause 4.2.2 of 3GPP TS 38.331 */
#define NR_RRC_BUF_SIZE 4096

typedef enum UE_STATE_NR_e {
  NR_RRC_INACTIVE=0,
  NR_RRC_IDLE,
  NR_RRC_SI_RECEIVED,
  NR_RRC_CONNECTED,
  NR_RRC_RECONFIGURED,
  NR_RRC_HO_EXECUTION
} NR_UE_STATE_t;

typedef struct {
  unsigned short transport_block_size; /*!< \brief Minimum PDU size in bytes provided by RLC to MAC layer interface */
  unsigned short max_transport_blocks; /*!< \brief Maximum PDU size in bytes provided by RLC to MAC layer interface */
  unsigned long Guaranteed_bit_rate; /*!< \brief Guaranteed Bit Rate (average) to be offered by MAC layer scheduling*/
  unsigned long Max_bit_rate; /*!< \brief Maximum Bit Rate that can be offered by MAC layer scheduling*/
  uint8_t Delay_class; /*!< \brief Delay class offered by MAC layer scheduling*/
  uint8_t Target_bler; /*!< \brief Target Average Transport Block Error rate*/
  uint8_t Lchan_t; /*!< \brief Logical Channel Type (BCCH,CCCH,DCCH,DTCH_B,DTCH,MRBCH)*/
} __attribute__ ((__packed__))  NR_LCHAN_DESC;

typedef struct RB_INFO_NR_s {
  uint16_t Rb_id; //=Lchan_id
  NR_LCHAN_DESC Lchan_desc[2];
  //MAC_MEAS_REQ_ENTRY *Meas_entry; //may not needed for NB-IoT
} NR_RB_INFO;

typedef struct SRB_INFO_TABLE_ENTRY_NR_s {
  uint8_t Active;
  uint8_t status;
} NR_SRB_INFO_TABLE_ENTRY;

#endif
