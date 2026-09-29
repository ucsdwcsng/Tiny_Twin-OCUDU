// Generated from exposeptr2.o

#pragma once

typedef unsigned int __uint32_t;

typedef unsigned long long __uint64_t;

typedef __uint32_t uint32_t;

typedef __uint64_t uint64_t;

struct ctx;

struct {
  int (*type)[2];
  uint64_t *key;
  uint32_t *value;
  int (*max_entries)[1];
} map;

int func(ctx* ctx);

