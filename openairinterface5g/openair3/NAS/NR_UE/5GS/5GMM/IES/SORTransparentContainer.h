/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "OctetString.h"

#ifndef SOR_TRANSPARENT_CONTAINER_H_
#define SOR_TRANSPARENT_CONTAINER_H_

#define SOR_TRANSPARENT_CONTAINER_MINIMUM_LENGTH 2 // [length]+[length]
#define SOR_TRANSPARENT_CONTAINER_MAXIMUM_LENGTH 65538 // [IEI]+[length]+[length]+[ESM msg]

typedef struct SORTransparentContainer_tag {
  OctetString sortransparentcontainercontents;
} SORTransparentContainer;

int encode_sor_transparent_container(const SORTransparentContainer *sortransparentcontainer,
                                     uint8_t iei,
                                     uint8_t *buffer,
                                     uint32_t len);

int decode_sor_transparent_container(SORTransparentContainer *sortransparentcontainer,
                                     uint8_t iei,
                                     const uint8_t *buffer,
                                     uint32_t len);

void dump_sor_transparent_container_xml(SORTransparentContainer *sortransparentcontainer, uint8_t iei);

#endif /* ESM MESSAGE CONTAINER_H_ */
