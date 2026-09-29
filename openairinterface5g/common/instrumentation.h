/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef INSTRUMENTATION_H
#define INSTRUMENTATION_H

#ifdef TRACY_ENABLE
#include "tracy/TracyC.h"
#endif

#ifndef TracyCZone
#define TracyCZone(...)
#endif
#ifndef TracyCZoneEnd
#define TracyCZoneEnd(...)
#endif
#ifndef TracyCFrameMark
#define TracyCFrameMark
#endif
#ifndef TracyCPlot
#define TracyCPlot(...)
#endif

#endif
