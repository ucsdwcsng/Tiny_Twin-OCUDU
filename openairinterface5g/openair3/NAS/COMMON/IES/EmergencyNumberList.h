/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "OctetString.h"

#ifndef EMERGENCY_NUMBER_LIST_H_
#define EMERGENCY_NUMBER_LIST_H_

#define EMERGENCY_NUMBER_LIST_MINIMUM_LENGTH 5
#define EMERGENCY_NUMBER_LIST_MAXIMUM_LENGTH 50

typedef struct EmergencyNumberList_tag {
  uint8_t  num_emergency_elements;
  struct {
    uint8_t  lengthofemergency;
    uint8_t  emergencyservicecategoryvalue:5;
#define EMERGENCY_NUMBER_LIST_MAX_2DIGITS 6
    uint8_t  bcd_digits[EMERGENCY_NUMBER_LIST_MAX_2DIGITS]; // length to be fixed
#define EMERGENCY_NUMBER_LIST_MAX_NUMBER_INFORMATION 2
  } emergency_number_information[EMERGENCY_NUMBER_LIST_MAX_NUMBER_INFORMATION];
} EmergencyNumberList;

int encode_emergency_number_list(EmergencyNumberList *emergencynumberlist, uint8_t iei, uint8_t *buffer, uint32_t len);

int decode_emergency_number_list(EmergencyNumberList *emergencynumberlist, uint8_t iei, uint8_t *buffer, uint32_t len);

void dump_emergency_number_list_xml(EmergencyNumberList *emergencynumberlist, uint8_t iei);

#endif /* EMERGENCY NUMBER LIST_H_ */

