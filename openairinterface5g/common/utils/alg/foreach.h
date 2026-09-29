/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2022 Mikel Irazabal
 */

#ifndef FOR_EACH_ALGORITHM
#define FOR_EACH_ALGORITHM

#include "../ds/seq_arr.h"

/**
 * @brief Apply function to each element in the sequence container
 * @param arr The sequence container
 * @param value Pointer to the value that will be used by the function applied
 * @param fn_apply Function called by every element in the sequence container
 */
void for_each(seq_arr_t* arr, void* value, void (*fn_apply)(void* value, void* it));

#endif
