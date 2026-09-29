// Generated from D:\libbtf\external\ebpf-samples\build\map_in_map_anonymous.o

#pragma once

typedef unsigned int uint32_t;

struct {
  int (*type)[12];
  int (*max_entries)[1];
  uint32_t *key;
  struct {
    int (*type)[2];
    uint32_t *key;
    uint32_t *value;
    int (*max_entries)[1];
  } * values[0];
} outer_map;

int func(void* ctx);

