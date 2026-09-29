/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief Lttng trace implementaion
 */

//This file is used to define the tracepoints for the lttng tracing.In order to to do linkage with the lttng-tp.h file, we need to define the tracepoints here.
#define TRACEPOINT_CREATE_PROBES
#define TRACEPOINT_DEFINE

#include "lttng-tp.h"
