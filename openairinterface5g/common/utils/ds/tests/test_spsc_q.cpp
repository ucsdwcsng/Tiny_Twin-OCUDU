/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "gtest/gtest.h"
extern "C" {
#include "spsc_q.h"
}

TEST(spsc_q, basic_test) {
  spsc_q_t rb;
  EXPECT_TRUE(spsc_q_alloc(&rb, 2, sizeof(int)));

  int a = 1;
  EXPECT_TRUE(spsc_q_put(&rb, &a, sizeof(a)));
  EXPECT_TRUE(spsc_q_put(&rb, &a, sizeof(a)));
  EXPECT_FALSE(spsc_q_put(&rb, &a, sizeof(a)));

  int b;
  EXPECT_TRUE(spsc_q_get(&rb, &b, sizeof(b)));
  EXPECT_EQ(a, b);
  EXPECT_TRUE(spsc_q_get(&rb, &b, sizeof(b)));
  EXPECT_EQ(a, b);
  EXPECT_FALSE(spsc_q_get(&rb, &b, sizeof(b)));

  spsc_q_free(&rb);
}

TEST(spsc_q, cont_write) {
  spsc_q_t rb;
  EXPECT_TRUE(spsc_q_alloc(&rb, 10, sizeof(int)));

  int a = 1;
  for (int i = 0; i < 1111; ++i) {
    EXPECT_TRUE(spsc_q_put(&rb, &a, sizeof(a)));
    a++;
    EXPECT_TRUE(spsc_q_put(&rb, &a, sizeof(a)));
    a++;
    EXPECT_TRUE(spsc_q_put(&rb, &a, sizeof(a)));
    a++;

    int b;
    EXPECT_TRUE(spsc_q_get(&rb, &b, sizeof(b)));
    EXPECT_EQ(b, a - 3);
    EXPECT_TRUE(spsc_q_get(&rb, &b, sizeof(b)));
    EXPECT_EQ(b, a - 2);
    EXPECT_TRUE(spsc_q_get(&rb, &b, sizeof(b)));
    EXPECT_EQ(b, a - 1);
  }

  spsc_q_free(&rb);
}

bool count(const void *data, void *user)
{
  EXPECT_TRUE(data != NULL);
  int *c = (int *)user;
  (*c)++;
  return true; /* count all */
}
TEST(spsc_q, iterator) {
  spsc_q_t rb;
  EXPECT_TRUE(spsc_q_alloc(&rb, 10, sizeof(int)));
  int n = 8;
  for (int i = 0; i < n; ++i)
    EXPECT_TRUE(spsc_q_put(&rb, &i, sizeof(i)));

  int c = 0;
  int results[n];
  int ret = spsc_q_get_while(&rb, count, &c, results, sizeof(*results), n);
  EXPECT_EQ(c, n);
  EXPECT_EQ(ret, n);
  for (int i = 0; i < n; ++i)
    EXPECT_EQ(results[i], i);

  int b;
  EXPECT_FALSE(spsc_q_get(&rb, &b, sizeof(b))); /* empty! */

  spsc_q_free(&rb);
}

bool drop(const void *data, void *user)
{
  const int *e = (const int *)data;
  int *c = (int *)user;
  EXPECT_EQ(*e, *c);
  (*c)++;
  return true; /* drop all */
}
TEST(spsc_q, drop) {
  spsc_q_t rb;
  EXPECT_TRUE(spsc_q_alloc(&rb, 10, sizeof(int)));
  int n = 8;
  for (int i = 0; i < n; ++i)
    EXPECT_TRUE(spsc_q_put(&rb, &i, sizeof(i)));

  int c = 0;
  int ret = spsc_q_drop_while(&rb, drop, &c);
  EXPECT_EQ(c, n);
  EXPECT_EQ(ret, n);

  int b;
  EXPECT_FALSE(spsc_q_get(&rb, &b, sizeof(b))); /* empty! */

  spsc_q_free(&rb);
}


TEST(spsc_q, full) {
  int n = 3;
  spsc_q_t rb;
  EXPECT_TRUE(spsc_q_alloc(&rb, n, sizeof(int)));
  int a = 1;
  EXPECT_TRUE(spsc_q_put(&rb, &a, sizeof(a)));
  a++;
  EXPECT_TRUE(spsc_q_put(&rb, &a, sizeof(a)));
  a++;
  EXPECT_TRUE(spsc_q_put(&rb, &a, sizeof(a)));
  a++;
  EXPECT_FALSE(spsc_q_put(&rb, &a, sizeof(a))); /* full! */

  int b;
  EXPECT_TRUE(spsc_q_get(&rb, &b, sizeof(b)));
  EXPECT_EQ(b, a - n);

  EXPECT_TRUE(spsc_q_put(&rb, &a, sizeof(a)));
  a++;
  EXPECT_FALSE(spsc_q_put(&rb, &a, sizeof(a))); /* full! */

  EXPECT_TRUE(spsc_q_get(&rb, &b, sizeof(b)));
  EXPECT_EQ(b, a - n);

  spsc_q_free(&rb);
}

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
