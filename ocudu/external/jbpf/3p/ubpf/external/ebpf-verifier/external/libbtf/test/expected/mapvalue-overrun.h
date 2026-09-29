// Generated from mapvalue-overrun.o

#pragma once

typedef unsigned int __uint32_t;

typedef __uint32_t uint32_t;

struct {
  int (*type)[2];
  int *key;
  uint32_t *value;
  int (*max_entries)[1];
} map;

int func(void* ctx);

