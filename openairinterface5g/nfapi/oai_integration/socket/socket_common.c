/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */
#include "socket_common.h"
#include "LOG/log.h"

int socket_send_p5_msg(const int sctp,
                       const int socket,
                       const void* remote_addr,
                       const void* msg,
                       const uint32_t len,
                       const uint16_t stream)
{
  if (sctp == 1) {
    struct sockaddr* to = (struct sockaddr*)remote_addr;
    const int retval = sctp_sendmsg(socket, msg, len, to, 0, 42, 0, stream, 0, 0);
    if (retval < 0) {
      LOG_E(NR_PHY, "sctp_sendmsg failed errno: %d\n", errno);
      return -1;
    }
  } else {
    if (send(socket, msg, len, 0) != len) {
      LOG_E(NR_PHY, "write failed errno: %d\n", errno);
      return -1;
    }
  }

  return len;
}

int socket_send_p7_msg(const int socket, const void* remote_addr, const void* msg, const uint32_t len)
{
  const struct sockaddr* to = (struct sockaddr*)remote_addr;
  const long sendto_result = sendto(socket, msg, len, 0, to, sizeof(struct sockaddr_in));
  if (sendto_result != len) {
    LOG_E(NR_PHY, "%s() sendto_result %ld %d\n", __FUNCTION__, sendto_result, errno);
    return -1;
  }

  return 0;
}
