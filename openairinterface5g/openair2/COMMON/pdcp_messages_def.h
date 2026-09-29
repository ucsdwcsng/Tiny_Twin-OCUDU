/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*
 * pdcp_messages_def.h
 */

//-------------------------------------------------------------------------------------------//
// Messages between RRC and PDCP layers
MESSAGE_DEF(RRC_DCCH_DATA_REQ,          MESSAGE_PRIORITY_MED_PLUS, RrcDcchDataReq,              rrc_dcch_data_req)
MESSAGE_DEF(RRC_DCCH_DATA_IND,          MESSAGE_PRIORITY_MED_PLUS, RrcDcchDataInd,              rrc_dcch_data_ind)
MESSAGE_DEF(RRC_PCCH_DATA_REQ,          MESSAGE_PRIORITY_MED_PLUS, RrcPcchDataReq,              rrc_pcch_data_req)
MESSAGE_DEF(RRC_DCCH_DATA_COPY_IND,     MESSAGE_PRIORITY_MED_PLUS, RrcDcchDataInd,         rrc_dcch_data_copy_ind)

// gNB
MESSAGE_DEF(NR_RRC_DCCH_DATA_REQ,       MESSAGE_PRIORITY_MED_PLUS, NRRrcDcchDataReq,           nr_rrc_dcch_data_req)
MESSAGE_DEF(NR_RRC_DCCH_DATA_IND,       MESSAGE_PRIORITY_MED_PLUS, NRRrcDcchDataInd,           nr_rrc_dcch_data_ind)
