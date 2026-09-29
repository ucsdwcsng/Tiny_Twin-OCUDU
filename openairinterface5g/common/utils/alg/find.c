/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2022 Mikel Irazabal
 */

#include "find.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

elm_arr_t find_if_arr_it(seq_arr_t* arr, void* start_it, void* end_it, void* value, bool (*f)(const void*, const void*))
{
  assert(arr != NULL);

  while (start_it != end_it) {
    if (f(value, start_it))
      return (elm_arr_t){.found = true, .it = start_it};
    start_it = seq_arr_next(arr, start_it);
  }
  // If I trusted the average developer I should return it=start_it, but I don't.
  return (elm_arr_t){.found = false, .it = NULL};
}

elm_arr_t find_if(seq_arr_t* arr, void* value, bool (*f)(const void*, const void*))
{
  assert(arr != NULL);
  void* start_it = seq_arr_front(arr);
  void* end_it = seq_arr_end(arr);
  return find_if_arr_it(arr, start_it, end_it, value, f);
}
