/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief shared memory to store data captured by T-Tracer services to be processed by data recording application
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>

#ifndef _SHARED_MEMORY_CONFIG_H_
#define _SHARED_MEMORY_CONFIG_H_

#define NUM_MESSAGES_PER_SLOT 5
#define SHMSIZE ((122.88e6 / (100 * 20)) * 100 * NUM_MESSAGES_PER_SLOT * 8) // ~229 MiB
//  122.88e6: sample rate at 100 MHz BW of 5G NR
//  100 * 20: 2000 slots/s (20 slots/frame, 100 frames/s) at SCS 30 kHz
//  100: number of slots to capture
//  NUM_MESSAGES_PER_SLOT: messages captured per slot
//  8: bytes per complex I+Q sample

// for gNB T Tracer App
#define GETKEYDIR1_gNB ("/tmp/gnb_app1")
#define GETKEYDIR2_gNB ("/tmp/gnb_app2")
#define PROJECTID_gNB (2335)

// for UET Tracer App
#define GETKEYDIR1_UE ("/tmp/ue_app1")
#define GETKEYDIR2_UE ("/tmp/ue_app2")
#define PROJECTID_UE (2336)

void err_exit(char *buf);
int create_shm(char **addrN, const char *pathname, int projectId);
void del_shm(char *addr, int shm_id);

#endif /* _SHARED_MEMORY_CONFIG_H_ */
