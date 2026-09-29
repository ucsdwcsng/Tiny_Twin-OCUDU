// Generated from tail_call_bad.o

#pragma once

typedef unsigned int __uint32_t;

typedef unsigned int __u32;

typedef __uint32_t uint32_t;

struct xdp_md {
  __u32 data;
  __u32 data_end;
  __u32 data_meta;
  __u32 ingress_ifindex;
  __u32 rx_queue_index;
  __u32 egress_ifindex;
};

struct {
  int (*type)[2];
  uint32_t *key;
  uint32_t *value;
  int (*max_entries)[1];
} map;

int callee(xdp_md* ctx);

int caller(xdp_md* ctx);

