#include "benchmark/benchmark.h"
#include "stdio.h"
#include "assert.h"
#include "pthread.h"

extern "C" {
#include "spsc_q.h"
}

typedef struct q_args {
  spsc_q_t q;
  int range;
} q_args_t;

static void *producer(void *arg)
{
  q_args_t *a = (q_args_t *) arg;
  int i = 0;
  while (i <= a->range) {
    if (spsc_q_put(&a->q, &i, sizeof(i)))
      i++;
  }
  return NULL;
}

static void *consumer(void *arg)
{
  q_args_t *a = (q_args_t *) arg;
  int i = 0;
  while (i != a->range) {
    int r;
    if (spsc_q_get(&a->q, &r, sizeof(r))) {
      assert(r == i);
      i++;
    }
  }
  return NULL;
}

static void run_test(int range, int q_size)
{
  q_args_t args = {
    .range = range,
  };
  spsc_q_alloc(&args.q, q_size, sizeof(int));

  pthread_t p, c;
  int ret;
  // start
  ret = pthread_create(&p, NULL, producer, &args);
  assert(ret == 0);
  ret = pthread_create(&c, NULL, consumer, &args);
  assert(ret == 0);

  ret = pthread_join(p, NULL);
  assert(ret == 0);
  ret = pthread_join(c, NULL);
  assert(ret == 0);
  // stop
  // data/time => throughput

  spsc_q_free(&args.q);
}

static void BM_spsc_q(benchmark::State &state)
{
  int range = 500000;
  int q_size = state.range(0);
  for (auto _ : state)
    run_test(range, q_size);
}

BENCHMARK(BM_spsc_q)->RangeMultiplier(2)->Range(10, 160);

BENCHMARK_MAIN();
