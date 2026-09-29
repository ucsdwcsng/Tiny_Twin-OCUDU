// Generated from badrelo.o

#pragma once

typedef unsigned long long __uint64_t;

struct bpf_map;

struct ctx;

typedef __uint64_t uint64_t;

struct {
  int (*type)[2];
  int *key;
  int (*value)[0];
  int (*max_entries)[1];
} map;

extern int ebpf_map_update_elem(bpf_map*, const void*, const void*, uint64_t);

int func(ctx* ctx);

