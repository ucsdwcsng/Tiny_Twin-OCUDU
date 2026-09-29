/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "PHY/types.h"
#include "PHY/defs.h"

void gpib_send(unsigned int gpib_card, unsigned int gpib_device, char *command_string );
void analyzer(unsigned int gpib_card,unsigned int gpib_device,unsigned int freqband,LTE_DL_FRAME_PARMS *frame_parms,DCI_ALLOC_t *dci_alloc);
