/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief security mode complete procedures for gNB
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "OctetString.h"

#ifndef FGC_NAS_MESSAGE_CONTAINER_H_
#define FGC_NAS_MESSAGE_CONTAINER_H_

#define FGC_NAS_MESSAGE_CONTAINER_MINIMUM_LENGTH 4

typedef struct FGCNasMessageContainer_tag {
  OctetString nasmessagecontainercontents;
} FGCNasMessageContainer;

int encode_fgc_nas_message_container(const FGCNasMessageContainer *nasmessagecontainer, uint8_t iei, uint8_t *buffer, uint32_t len);

int decode_fgc_nas_message_container(FGCNasMessageContainer *nasmessagecontainer, uint8_t iei, uint8_t *buffer, uint32_t len);

void dump_fgc_nas_message_container_xml(FGCNasMessageContainer *nasmessagecontainer, uint8_t iei);

#endif /* FGC NAS MESSAGE CONTAINER_H_ */
