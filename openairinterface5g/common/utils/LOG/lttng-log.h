/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief LTTng Log interface
 */

#ifndef __LTTNG_LOG_H__
#define __LTTNG_LOG_H__
#if ENABLE_LTTNG
#include "lttng-tp.h"

#define LOG_FC(component, func, line, log)                        \
  do {                                                            \
    tracepoint(OAI, gNB, component, -1, -1, -1, func, line, log); \
  } while (0)
#else
#define LOG_FC(component, func, line, log)
#endif

#endif /** __LTTNG_LOG_H__ */
