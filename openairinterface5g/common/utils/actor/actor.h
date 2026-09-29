/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef ACTOR_H
#define ACTOR_H
#include "common/utils/threadPool/notified_fifo.h"

#define INIT_ACTOR(ptr, name, core_affinity) init_actor((Actor_t *)ptr, name, core_affinity);

#define DESTROY_ACTOR(ptr) destroy_actor((Actor_t *)ptr);

#define SHUTDOWN_ACTOR(ptr) shutdown_actor((Actor_t *)ptr);

typedef struct {
  notifiedFIFO_t fifo;
  bool terminate;
  pthread_t thread;
} Actor_t;

/// @brief Initialize the actor. Starts actor thread
/// @param actor
/// @param name Actor name. Thread name will be derived from it
/// @param core_affinity Core affinity. Specify -1 for no affinity
void init_actor(Actor_t *actor, const char *name, int core_affinity);

/// @brief Destroy the actor. Free the memory, stop the thread.
/// @param actor
void destroy_actor(Actor_t *actor);

/// @brief Gracefully shutdown the actor, letting all tasks previously started finish
/// @param actor
void shutdown_actor(Actor_t *actor);

/// @brief This function will return when all current jobs in the queue are finished.
/// The caller should make sure no new jobs are added to the queue between this function call and return.
/// @param actor
void flush_actor(Actor_t *actor);

#endif
