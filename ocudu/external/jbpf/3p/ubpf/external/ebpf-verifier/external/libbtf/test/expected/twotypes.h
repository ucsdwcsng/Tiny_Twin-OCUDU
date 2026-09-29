// Generated from twotypes.o

#pragma once

struct ctx;

struct {
  int (*type)[2];
  int *key;
  int (*value)[1024];
  int (*max_entries)[1];
} map;

int func(ctx* ctx);

