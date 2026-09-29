/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <stddef.h>
#include <stdint.h>
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
#include "common/platform_constants.h"
//-----------------------------------------------------------------------------

typedef unsigned char uint8_t;

//-----------------------------------------------------------------------------

char* memory_get_path_from_ueid(const char* dirname, const char* filename, int ueid);

int memory_read(const char* datafile, void* data, size_t size);

int memory_write(const char* datafile, const void* data, size_t size);

#ifdef __cplusplus
}
#endif
#endif /* __MEMORY_H__*/
