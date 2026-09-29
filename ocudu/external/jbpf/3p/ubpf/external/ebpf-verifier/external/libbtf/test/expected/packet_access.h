// Generated from packet_access.o

#pragma once

typedef unsigned int __u32;

struct xdp_md {
  __u32 data;
  __u32 data_end;
  __u32 data_meta;
  __u32 ingress_ifindex;
  __u32 rx_queue_index;
  __u32 egress_ifindex;
};

int test_packet_access(xdp_md* ctx);

