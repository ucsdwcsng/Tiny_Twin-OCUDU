/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "spsc_q.h"

bool spsc_q_alloc(spsc_q_t *rbn, size_t cnt, size_t elsiz)
{
  /* internally, use one element more: the ringbuffer is full if
   * (write_idx + 1) % cnt == read_idx, so it's full if one element is still
   * free (because otherwise write_idx == read_idx, but this is the empty
   * condition) */
  cnt += 1;
  spsc_q_t rb = {.cnt = cnt, .elsiz = elsiz};
  rb.buf = calloc(cnt, elsiz);
  *rbn = rb;
  return rb.buf != NULL;
}

void spsc_q_free(spsc_q_t *rb)
{
  free(rb->buf);
  memset(rb, 0, sizeof(*rb));
}

bool spsc_q_put(spsc_q_t *rb, const void *src, size_t elsiz)
{
  assert(elsiz == rb->elsiz);
  size_t w = atomic_load_explicit(&rb->write_idx, memory_order_relaxed);
  size_t r = atomic_load_explicit(&rb->read_idx, memory_order_acquire);
  if ((w + 1) % rb->cnt == r) {
    return false;
  }

  uint8_t *bufpos = &rb->buf[w * rb->elsiz];
  memcpy(bufpos, src, elsiz);
  atomic_store_explicit(&rb->write_idx, (w + 1) % rb->cnt, memory_order_release);
  return true;
}

bool spsc_q_get_if(spsc_q_t *rb, pred p, void *user, void *dest, size_t elsiz)
{
  assert(elsiz == rb->elsiz);
  size_t w = atomic_load_explicit(&rb->write_idx, memory_order_acquire);
  size_t r = atomic_load_explicit(&rb->read_idx, memory_order_relaxed);
  if (w == r) {
    return false;
  }

  uint8_t *bufpos = &rb->buf[rb->read_idx * rb->elsiz];
  if (!p(bufpos, user)) {
    return false;
  }

  if (dest)
    memcpy(dest, bufpos, elsiz);
  atomic_store_explicit(&rb->read_idx, (r + 1) % rb->cnt, memory_order_release);
  return true;
}

static bool always(const void *data, void *user)
{
  (void) data;
  (void) user;
  return true;
}
bool spsc_q_get(spsc_q_t *rb, void *dest, size_t elsiz)
{
  return spsc_q_get_if(rb, always, NULL, dest, elsiz);
}

int spsc_q_get_while(spsc_q_t *rb, pred p, void *user, void *dest, size_t elsiz, size_t max_len)
{
  assert(elsiz == rb->elsiz);
  size_t count = 0;
  while (count < max_len && spsc_q_get_if(rb, p, user, dest + elsiz * count, elsiz))
    count++;
  return count;
}

int spsc_q_drop_while(spsc_q_t *rb, pred p, void *user)
{
  size_t dropped = 0;
  while (spsc_q_get_if(rb, p, user, NULL, rb->elsiz))
    dropped++;
  return dropped;
}
