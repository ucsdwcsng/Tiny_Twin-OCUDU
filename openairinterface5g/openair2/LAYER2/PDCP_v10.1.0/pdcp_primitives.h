/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
* \brief pdcp primitives  
*/

/** \addtogroup _pdcp
*  @{
 */
#ifndef PDCP_PRIMITIVES_H
#define PDCP_PRIMITIVES_H

/*
 * 3GPP TS 36.323 V10.1.0 (2011-03)
 */

/*! \brief Data or control (1-bit, see 6.3.7) */
#define PDCP_CONTROL_PDU_BIT_SET 0x00
#define PDCP_DATA_PDU_BIT_SET    0x01

/*
 * 6.1 Protocol Data Units
 * 6.2.2 Control Plane PDCP Data PDU
 */
#define PDCP_CONTROL_PLANE_DATA_PDU_SN_SIZE 1
#define PDCP_CONTROL_PLANE_DATA_PDU_MAC_I_SIZE 4
typedef struct {
  uint8_t sn;      // PDCP sequence number will wrap around 2^5-1 so
  // reserved field is unnecessary here
  uint8_t mac_i[PDCP_CONTROL_PLANE_DATA_PDU_MAC_I_SIZE];  // Integration protection is not implemented (pad with 0)
} pdcp_control_plane_data_pdu_header;

/*
 * 6.2.3 User Plane PDCP Data PDU with long PDCP SN (12-bit)
 */
typedef struct {
  uint8_t dc;      // Data or control (see 6.3.7)
  uint16_t sn;     // 12-bit sequence number
} pdcp_user_plane_data_pdu_header_with_long_sn;
#define PDCP_USER_PLANE_DATA_PDU_LONG_SN_HEADER_SIZE 2

/*
 * 6.2.4 User Plane PDCP Data PDU with short PDCP SN (7-bit)
 */
typedef struct {
  uint8_t dc;
  uint8_t sn;      // 7-bit sequence number
} pdcp_user_plane_data_pdu_header_with_short_sn;
#define PDCP_USER_PLANE_DATA_PDU_SHORT_SN_HEADER_SIZE 1

/*
 * 6.2.5 PDCP Control PDU for interspersed ROHC feedback packet
 */
typedef struct {
  uint8_t dc;
  uint8_t pdu_type; // PDU type (see 6.3.8)
} pdcp_control_pdu_for_interspersed_rohc_feedback_packet_header;

/*
 * 6.2.6 PDCP Control PDU for PDCP status report
 */
typedef struct {
  uint8_t dc;
  uint8_t pdu_type; // PDU type (see 6.3.8)
  uint16_t first_missing_sn; // First missing PDCP SN
  unsigned char* window_bitmap; // Ack/Nack information coded as a bitmap
  uint16_t window_bitmap_size;
} pdcp_control_pdu_for_pdcp_status_report;

/*
 * Parses sequence number out of buffer of User Plane PDCP Data PDU with
 * long PDCP SN (12-bit)
 *
 * @param pdu_buffer PDCP PDU buffer
 * @return 12-bit sequence number
 */
uint16_t pdcp_get_sequence_number_of_pdu_with_long_sn(unsigned char* pdu_buffer);

/*
 * Parses sequence number out of buffer of User Plane PDCP Data PDU with
 * short PDCP SN (7-bit)
 *
 * @param pdu_buffer PDCP PDU buffer
 * @return 7-bit sequence number
 */
uint8_t pdcp_get_sequence_number_of_pdu_with_short_sn(unsigned char* pdu_buffer);

/*
 * Parses sequence number out of buffer of Control Plane PDCP Data PDU with
 * short PDCP SN (5-bit)
 *
 * @param pdu_buffer PDCP PDU buffer
 * @return 5-bit sequence number
 */
uint8_t pdcp_get_sequence_number_of_pdu_with_SRB_sn(unsigned char* pdu_buffer);

/*
 * Fills the incoming buffer with the fields of the header for SRB1
 *
 * @param pdu_buffer PDCP PDU buffer
 * @return true on success, false otherwise
 */
bool pdcp_serialize_control_plane_data_pdu_with_SRB_sn_buffer(unsigned char* pdu_buffer,
                                                              pdcp_control_plane_data_pdu_header* pdu);
/*
 * Fills the incoming buffer with the fields of the header for long SN (RLC UM and AM)
 *
 * @param pdu_buffer PDCP PDU buffer
 * @return true on success, false otherwise
 */
bool pdcp_serialize_user_plane_data_pdu_with_long_sn_buffer(unsigned char* pdu_buffer,
                                                            pdcp_user_plane_data_pdu_header_with_long_sn* pdu);

/*
 * Fills the incoming status report header with given value of bitmap
 * and 'first missing pdu' sequence number
 *
 * @param FMS First Missing PDCP SN
 * @param bitmap Received/Missing sequence number bitmap
 * @param pdu A status report header
 * @return true on success, false otherwise
 */
bool pdcp_serialize_control_pdu_for_pdcp_status_report(unsigned char* pdu_buffer,
                                                       uint8_t bitmap[512],
                                                       pdcp_control_pdu_for_pdcp_status_report* pdu);

int pdcp_netlink_dequeue_element(const protocol_ctxt_t* const  ctxt_pP,
                                 struct pdcp_netlink_element_s **data_ppP);

void pdcp_config_set_security(const protocol_ctxt_t* const ctxt_pP,
                              pdcp_t *pdcp_pP,uint8_t security_modeP,
                              uint8_t *kRRCenc_pP,
                              uint8_t *kRRCint_pP,
                              uint8_t *kUPenc_pP);

int pdcp_apply_security(const protocol_ctxt_t* const  ctxt_pP,
                        pdcp_t     *pdcp_entity,
                        srb_flag_t  srb_flagP,
                        rb_id_t     rb_id,
                        uint8_t     pdcp_header_len,
                        uint16_t    current_sn,
                        uint8_t    *pdcp_pdu_buffer,
                        uint16_t    sdu_buffer_size);

int pdcp_validate_security(const protocol_ctxt_t* const  ctxt_pP,
                           pdcp_t *pdcp_entity,
                           srb_flag_t srb_flagP,
                           rb_id_t rb_id,
                           uint8_t pdcp_header_len,
                           uint32_t hfn,
                           int sn,
                           uint8_t *pdcp_pdu_buffer,
                           uint16_t sdu_buffer_size);

#endif
/** @}*/
