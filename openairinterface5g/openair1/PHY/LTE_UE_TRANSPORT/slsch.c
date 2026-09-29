/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief Functions to Generate and Receive PSSCH
 */
#include "PHY/defs_UE.h"

void generate_slsch(PHY_VARS_UE *ue,SLSCH_t *slsch,int frame_tx,int subframe_tx) {

  UE_tport_t pdu;
  size_t slsch_header_len = sizeof(UE_tport_header_t);

  if (slsch->rvidx==0) {
    pdu.header.packet_type = SLSCH;
    pdu.header.absSF = (frame_tx*10)+subframe_tx;
    
    memcpy((void*)&pdu.slsch,(void*)slsch,sizeof(SLSCH_t)-sizeof(uint8_t*));
    
    AssertFatal(slsch->payload_length <=1500-slsch_header_len - sizeof(SLSCH_t) + sizeof(uint8_t*),
		"SLSCH payload length > %zd\n",
		1500-slsch_header_len - sizeof(SLSCH_t) + sizeof(uint8_t*));
    memcpy((void*)&pdu.payload[0],
	   (void*)slsch->payload,
	   slsch->payload_length);
    
    LOG_I(PHY,"SLSCH configuration %zd bytes, TBS payload %d bytes => %zd bytes\n",
	  sizeof(SLSCH_t)-sizeof(uint8_t*),
	  slsch->payload_length,
	  slsch_header_len+sizeof(SLSCH_t)-sizeof(uint8_t*)+slsch->payload_length);
    
    AssertFatal(false, "multicast_link_write_sock() not implemented\n");
  }
}
