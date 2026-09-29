// Generated from nullmapref.o

#pragma once

typedef unsigned int __uint32_t;

typedef __uint32_t uint32_t;

struct {
  int (*type)[1];
  uint32_t *key;
  uint32_t *value;
  int (*max_entries)[1];
} test_map;

int test_repro(void* ctx);

