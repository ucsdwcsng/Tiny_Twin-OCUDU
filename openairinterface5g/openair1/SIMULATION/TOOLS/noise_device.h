/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef NOISE_DEVICE_H
#define NOISE_DEVICE_H

#include "SIMULATION/TOOLS/sim.h"

void init_noise_device(float noise_power);
void free_noise_device(void);
void get_noise_vector(float *noise_vector, int length);

#endif
