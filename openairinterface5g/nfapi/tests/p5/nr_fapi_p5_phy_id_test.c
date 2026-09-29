/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */
#include "nfapi/tests/nr_fapi_test.h"
#include "nr_fapi_p5.h"
#include "nr_fapi_p5_utils.h"

// The phy_id is carried on the wire in the opaque handle byte of the FAPI
// message header (SCF 222 §3.3.1.1), so only values 0..255 survive the round trip.
static void test_phy_id_roundtrip(uint16_t phy_id)
{
  nfapi_nr_start_request_scf_t req = {.header.message_id = NFAPI_NR_PHY_MSG_TYPE_START_REQUEST, .header.phy_id = phy_id};
  uint8_t msg_buf[65535];
  int pack_result = fapi_nr_p5_message_pack(&req, sizeof(req), msg_buf, sizeof(msg_buf), NULL);
  // START.request message body length is 0
  DevAssert(pack_result == 0 + NFAPI_HEADER_LENGTH);
  // the opaque handle is the second byte of the packed header
  DevAssert(msg_buf[1] == phy_id);

  // test the header-peek unpack
  nfapi_nr_p4_p5_message_header_t peek_hdr = {0};
  int unpack_header_result = fapi_nr_message_header_unpack(msg_buf, NFAPI_HEADER_LENGTH, &peek_hdr, sizeof(peek_hdr), 0);
  DevAssert(unpack_header_result >= 0);
  DevAssert(peek_hdr.phy_id == phy_id);
  DevAssert(peek_hdr.message_id == req.header.message_id);

  // test the full message unpack
  nfapi_nr_start_request_scf_t unpacked_req = {0};
  int unpack_result =
      fapi_nr_p5_message_unpack(msg_buf, peek_hdr.message_length + NFAPI_HEADER_LENGTH, &unpacked_req, sizeof(unpacked_req), NULL);
  DevAssert(unpack_result >= 0);
  DevAssert(unpacked_req.header.phy_id == phy_id);
  DevAssert(eq_start_request(&unpacked_req, &req));
  free_start_request(&unpacked_req);
  free_start_request(&req);
}

int main()
{
  fapi_test_init();

  for (uint16_t phy_id = 0; phy_id <= UINT8_MAX; phy_id++) {
    test_phy_id_roundtrip(phy_id);
  }
  // All tests successful!
  return 0;
}
