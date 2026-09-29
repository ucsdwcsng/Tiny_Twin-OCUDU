/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2022 Mikel Irazabal
 */

#include "foreach.h"

#include <assert.h>
#include <stdlib.h>

void for_each(seq_arr_t* arr, void* value, void (*f)(void* value, void* it))
{
  assert(arr != NULL);

  void* it = seq_arr_front(arr);
  void* end = seq_arr_end(arr);
  while (it != end) {
    f(value, it);
    it = seq_arr_next(arr, it);
  }
}
