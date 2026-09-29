// Generated from map_in_map.o

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
} array_of_maps;

struct {
  int (*type)[2];
  uint32_t *key;
  uint32_t *value;
  int (*max_entries)[1];
} inner_map;

int func(void* ctx);

