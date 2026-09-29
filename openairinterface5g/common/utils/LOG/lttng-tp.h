/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief Lttng tracer implementaion
 */

#ifndef _LTTNG_TP_H
#define _LTTNG_TP_H

#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER OAI

#undef TRACEPOINT_INCLUDE
#define TRACEPOINT_INCLUDE "./common/utils/LOG/lttng-tp.h"

#include <stdbool.h>
#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(
    OAI,
    gNB,
    TP_ARGS(const char*, log_modName, int, event_id, int, sfn, int, slot, const char*, funcName, int, lineNo, const char*, msg),
    TP_FIELDS(ctf_string(MODNAME, log_modName) ctf_integer(int32_t, EVENTID, event_id) ctf_integer(int32_t, SFN, sfn)
                  ctf_integer(int32_t, SLOT, slot) ctf_string(FUNCTION, funcName) ctf_integer(int32_t, LINE, lineNo)
                      ctf_string(MSG, msg)))
TRACEPOINT_LOGLEVEL(OAI, gNB, TRACE_DEBUG_FUNCTION)

#endif /* _LTTNG_TP_H */

#include <lttng/tracepoint-event.h>
