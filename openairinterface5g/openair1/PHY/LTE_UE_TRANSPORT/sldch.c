/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief Functions to Generate and Receive PSDCH
 */
#include "PHY/defs_UE.h"

void generate_sldch(PHY_VARS_UE *ue,SLDCH_t *sldch,int frame_tx,int subframe_tx) {

  UE_tport_t pdu;
  size_t sldch_header_len = sizeof(UE_tport_header_t);

  pdu.header.packet_type = SLDCH;
  pdu.header.absSF = (frame_tx*10)+subframe_tx;


  AssertFatal((sldch->payload_length <=1500-sldch_header_len - sizeof(SLDCH_t) + sizeof(uint8_t*)),
                "SLDCH payload length > %zd\n",
                1500-sldch_header_len - sizeof(SLDCH_t) + sizeof(uint8_t*));
  memcpy((void*)&pdu.sldch,
         (void*)sldch,
         sizeof(SLDCH_t));

  LOG_I(PHY,"SLDCH configuration %zd bytes, TBS payload %d bytes => %zd bytes\n",
        sizeof(SLDCH_t)-sizeof(uint8_t*),
        sldch->payload_length,
        sldch_header_len+sizeof(SLDCH_t)-sizeof(uint8_t*)+sldch->payload_length);

  AssertFatal(false, "multicast_link_write_sock() not implemented\n");
}
