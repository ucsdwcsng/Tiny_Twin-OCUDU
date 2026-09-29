/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef EQ_CHECK_H_
#define EQ_CHECK_H_

#include <stdio.h>
#include <string.h>

/*
 * Layers may override PRINT_ERROR() before including this header to preserve
 * custom logging style (for example by mapping it to a local PRINT_ERROR()).
 */

#ifndef PRINT_ERROR
#ifdef ENABLE_TESTS
#define PRINT_ERROR(fmt, ...) fprintf(stderr, "%s:%d: " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define PRINT_ERROR(...) \
  do {                   \
  } while (0)
#endif
#endif

#ifndef EQ_CHECK_FAIL
#define EQ_CHECK_FAIL() return false
#endif

#define EQ_CHECK_GENERIC(condition, fmt, ...)                                                         \
  do {                                                                                                \
    if (!(condition)) {                                                                               \
      PRINT_ERROR("Equality condition '%s' failed: " fmt " != " fmt "\n", #condition, ##__VA_ARGS__); \
      EQ_CHECK_FAIL();                                                                                \
    }                                                                                                 \
  } while (0)

#define _EQ_CHECK_LONG(A, B) EQ_CHECK_GENERIC((A) == (B), "%ld", (A), (B))
#define _EQ_CHECK_INT(A, B) EQ_CHECK_GENERIC((A) == (B), "%d", (A), (B))
#define _EQ_CHECK_UINT32(A, B) EQ_CHECK_GENERIC((A) == (B), "%u", (A), (B))
#define _EQ_CHECK_UINT64(A, B) EQ_CHECK_GENERIC((A) == (B), "%" PRIu64, (uint64_t)(A), (uint64_t)(B))
#define _EQ_CHECK_STR(A, B) EQ_CHECK_GENERIC(strcmp((A), (B)) == 0, "'%s'", (A), (B))

#define _EQ_CHECK_OPTIONAL_IE(A, B, FIELD, EQ_MACRO)                                                                      \
  do {                                                                                                                    \
    EQ_CHECK_GENERIC((((A)->FIELD) != NULL) == (((B)->FIELD) != NULL), "%d", ((A)->FIELD) != NULL, ((B)->FIELD) != NULL); \
    if ((A)->FIELD && (B)->FIELD)                                                                                         \
      EQ_MACRO(*(A)->FIELD, *(B)->FIELD);                                                                                 \
  } while (0)

#define _EQ_CHECK_OPTIONAL_PTR(A, B, FIELD)                          \
  do {                                                               \
    if ((((A)->FIELD) != NULL) != (((B)->FIELD) != NULL)) {          \
      PRINT_ERROR("%s:%d: optional IE '%s' not allocated: %p, %p\n", \
                  __FILE__,                                          \
                  __LINE__,                                          \
                  #FIELD,                                            \
                  (void *)(A)->FIELD,                                \
                  (void *)(B)->FIELD);                               \
      EQ_CHECK_FAIL();                                               \
    }                                                                \
  } while (0)

#endif /* EQ_CHECK_H_ */
