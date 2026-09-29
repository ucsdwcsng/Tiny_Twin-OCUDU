/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2022 Mikel Irazabal
 */

#ifndef FIND_ALGORITHM
#define FIND_ALGORITHM

#include <stdbool.h>
#include "../ds/seq_arr.h"

typedef struct {
  void* it;
  bool found;
} elm_arr_t;

// Sequencial containers

/**
 * @brief Find elements in an array if the predicate is true
 * @param arr The sequence container
 * @param value Pointer to the value that will be used by the predicate
 * @param f Function representing the predicate
 * @return Whether the predicate was fullfilled and the iterator to the element if true
 */
elm_arr_t find_if(seq_arr_t* arr, void* value, bool (*f)(const void* value, const void* it));

/**
 * @brief Find elements in an array in the semi-open range [start_it, end_it)
 * @param arr The sequence container
 * @param start_it Iterator to the first element
 * @param end_it Iterator to the one past last element
 * @param value Pointer to the value usied in the predicate
 * @param f Function representing the predicate
 * @return Whether the predicate was fullfilled and the iterator to the element if true
 */
elm_arr_t find_if_arr_it(seq_arr_t* arr, void* start_it, void* end_it, void* value, bool (*f)(const void* value, const void* it));

#endif
