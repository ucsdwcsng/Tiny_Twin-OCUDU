/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "actor.h"
#include "thread-pool.h"
#include "task.h"
#include "log.h"
#include <stdio.h>
#include <time.h>

#define NUM_THREADS 10
#define NUM_JOBS 200000

typedef struct {
  struct timespec ts;
  int actor_index;
} actor_task_t;

struct timespec ts_arr[NUM_JOBS];

long long delay_table[NUM_THREADS] = {0};

void calculate_delay(struct timespec* send_ts, int thread_index)
{
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  long long recv_time = ts.tv_sec * 1000000000LL + ts.tv_nsec;
  long long send_time = send_ts->tv_sec * 1000000000LL + send_ts->tv_nsec;
  long long delay = recv_time - send_time;
  delay_table[thread_index] += delay;
}

void tpool_function(void* args)
{
  int worker_id = get_tpool_worker_index();
  calculate_delay((struct timespec*)args, worker_id);
}

void actor_function(void* args)
{
  actor_task_t* actor_args = (actor_task_t*)args;
  calculate_delay(&actor_args->ts, actor_args->actor_index);
}

int main()
{
  logInit();
  tpool_t pool;
  char params[NUM_THREADS * 4];
  memset(params, 0, sizeof(params));
  for (int i = 0; i < NUM_THREADS; i++) {
    char buf[4];
    snprintf(buf, sizeof(buf), "%d,", -1);
    strcat(params, buf);
  }

  initTpool(params, &pool, true);

  // Example task
  task_t task;
  task.func = tpool_function;
  task.args = NULL;

  // Push tasks to the thread pool
  for (int i = 0; i < NUM_JOBS; i++) {
    struct timespec* ts = &ts_arr[i];
    clock_gettime(CLOCK_MONOTONIC, ts);
    task.args = ts;
    pushTpool(&pool, task);
  }

  // Abort the thread pool
  abortTpool(&pool);

  long long sum_delay = 0;
  for (int i = 0; i < NUM_THREADS; i++) {
    sum_delay += delay_table[i];
  }
  float average_delay = sum_delay / (NUM_JOBS * 1.0f);
  printf("Average task delay on tpool: %.2f ns\n", average_delay);

  memset(delay_table, 0, sizeof(delay_table));

  Actor_t actors[NUM_THREADS];
  for (int i = 0; i < NUM_THREADS; i++) {
    init_actor(&actors[i], "example_actor", -1);
  }

  // Push tasks to the actors
  for (int i = 0; i < NUM_JOBS; i++) {
    notifiedFIFO_elt_t* task = newNotifiedFIFO_elt(sizeof(actor_task_t), 0, NULL, actor_function);
    actor_task_t* arg_ts = (actor_task_t*)NotifiedFifoData(task);
    arg_ts->actor_index = i % NUM_THREADS;
    clock_gettime(CLOCK_MONOTONIC, &arg_ts->ts);
    pushNotifiedFIFO(&actors[i % NUM_THREADS].fifo, task);
  }

  for (int i = 0; i < NUM_THREADS; i++) {
    shutdown_actor(&actors[i]);
  }

  sum_delay = 0;
  for (int i = 0; i < NUM_THREADS; i++) {
    sum_delay += delay_table[i];
  }
  average_delay = sum_delay / (NUM_JOBS * 1.0f);
  printf("Average task delay on actors: %.2f ns\n", average_delay);
  return 0;
}
