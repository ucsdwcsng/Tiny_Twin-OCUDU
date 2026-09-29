/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */
#include "dci_payload_utils.h"
#include "nr_fapi.h"

void test_pack_payload(uint8_t payloadSizeBits, uint8_t payload[])
{
  uint8_t msg_buf[8192] = {0};
  uint8_t payloadSizeBytes = (payloadSizeBits + 7) / 8;
  uint8_t *pWritePackedMessage = msg_buf;
  uint8_t *pPackMessageEnd = msg_buf + sizeof(msg_buf);

  pack_dci_payload(payload, payloadSizeBits, &pWritePackedMessage, pPackMessageEnd);

  uint8_t unpack_buf[payloadSizeBytes];
  pWritePackedMessage = msg_buf;
  unpack_dci_payload(unpack_buf, payloadSizeBits, &pWritePackedMessage, pPackMessageEnd);

  printf("\nOriginal:\t");
  for (int j = payloadSizeBytes - 1; j >= 0; j--) {
    printbits(payload[j], 1);
    printf(" ");
  }
  printf("\n");

  printf("Unpacked:\t");
  for (int j = payloadSizeBytes - 1; j >= 0; j--) {
    printbits(unpack_buf[j], 1);
    printf(" ");
  }
  printf("\n");

  DevAssert(memcmp(payload, unpack_buf, payloadSizeBytes) == 0);
  // All tests successful!
}

int main()
{
  fapi_test_init();
  uint8_t upper = 8 * DCI_PAYLOAD_BYTE_LEN;
  uint8_t lower = 1;
  uint8_t payloadSizeBits = (rand() % (upper - lower + 1)) + lower; // from 1 bit to DCI_PAYLOAD_BYTE_LEN, in bits
  printf("payloadSizeBits:%d\n", payloadSizeBits);
  uint8_t payload[(payloadSizeBits + 7) / 8];

  generate_payload(payloadSizeBits, payload);
  test_pack_payload(payloadSizeBits, payload);
  return 0;
}
