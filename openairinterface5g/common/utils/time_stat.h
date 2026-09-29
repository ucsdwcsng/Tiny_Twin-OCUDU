/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef _COMMON_UTIL_TIME_STAT_H_
#define _COMMON_UTIL_TIME_STAT_H_

#include <stdint.h>

typedef struct {
  uint64_t time;     /* unit: microsecond */
  uint64_t value;    /* unit: microsecond */
} time_value_t;

typedef struct {
  int head;
  int tail;
  time_value_t *buffer;
  int maxsize;
  int size;
} time_ring_t;

typedef struct {
  int duration;     /* unit: microsecond */
  time_ring_t r;
  uint64_t accumulated_value;
} time_average_t;

/* 'duration' is in unit microsecond */
time_average_t *time_average_new(int duration, int initial_size);
void time_average_free(time_average_t *t);
void time_average_reset(time_average_t *t);

/* add a value tagged with time 'time' unit microsecond (it also modifies 't',
 * removing all data with time < 'time' - t->duration)
 */
void time_average_add(time_average_t *t, uint64_t time, uint64_t value);

/* get the average for time 'time' (it also modifies 't', removing all
 * data with time < 'time' - t->duration)
 */
double time_average_get_average(time_average_t *t, uint64_t time);

/* unit microsecond */
uint64_t time_average_now(void);

#endif /* _COMMON_UTIL_TIME_STAT_H_ */
