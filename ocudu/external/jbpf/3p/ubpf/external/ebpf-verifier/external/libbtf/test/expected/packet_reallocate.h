// Generated from packet_reallocate.o

#pragma once

struct bpf_sock;

struct bpf_flow_keys;

typedef unsigned int __u32;

typedef unsigned long long __u64;

struct __sk_buff {
  __u32 len;
  __u32 pkt_type;
  __u32 mark;
  __u32 queue_mapping;
  __u32 protocol;
  __u32 vlan_present;
  __u32 vlan_tci;
  __u32 vlan_proto;
  __u32 priority;
  __u32 ingress_ifindex;
  __u32 ifindex;
  __u32 tc_index;
  __u32 cb[5];
  __u32 hash;
  __u32 tc_classid;
  __u32 data;
  __u32 data_end;
  __u32 napi_id;
  __u32 family;
  __u32 remote_ip4;
  __u32 local_ip4;
  __u32 remote_ip6[4];
  __u32 local_ip6[4];
  __u32 remote_port;
  __u32 local_port;
  __u32 data_meta;
  union {
    bpf_flow_keys *flow_keys;
  };
  __u64 tstamp;
  __u32 wire_len;
  __u32 gso_segs;
  union {
    bpf_sock *sk;
  };
  __u32 gso_size;
};

int reallocate_invalidates(__sk_buff* ctx);

