/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright 2017 Cisco Systems, Inc.
 */

#pragma once

#include <string.h>
#include <errno.h>

#define ERR strerror(errno)

/*! The trace levels used by the nfapi libraries */
typedef enum nfapi_trace_level
{
    NFAPI_TRACE_NONE,
    NFAPI_TRACE_ERROR,
    NFAPI_TRACE_WARN,
    NFAPI_TRACE_NOTE,
    NFAPI_TRACE_INFO,
    NFAPI_TRACE_DEBUG,
} nfapi_trace_level_t;

void nfapi_trace(nfapi_trace_level_t, char const *caller, const char *format, ...)
    __attribute__((format(printf, 3, 4)));

nfapi_trace_level_t nfapi_trace_level(void);

#define NFAPI_TRACE(LEVEL, FORMAT, ...) do {                            \
    if (nfapi_trace_level() >= (LEVEL))                                 \
        nfapi_trace(LEVEL, __func__, FORMAT, ##__VA_ARGS__);            \
} while (0)

