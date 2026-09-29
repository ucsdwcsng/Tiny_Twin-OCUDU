/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include <gtest/gtest.h>
extern "C" {
#include "fsn.h"
}

TEST(fsn, test_mu0_fsn_add) {
  fsn_t current = {123, 5, 0};
  fsn_t next = current;

  next = fsn_add_delta(next, 12);
  EXPECT_EQ(next.f, 124);
  EXPECT_EQ(next.s, 7);
  EXPECT_EQ(next.mu, 0);

  next = fsn_add_delta(next, 10);
  EXPECT_EQ(next.f, 125);
  EXPECT_EQ(next.s, 7);
  EXPECT_EQ(next.mu, 0);

  next = fsn_add_delta(next, 10 * 1024);
  EXPECT_EQ(next.f, 125);
  EXPECT_EQ(next.s, 7);
  EXPECT_EQ(next.mu, 0);
}

TEST(fsn, test_mu0_get_diff) {
  fsn_t current = {123, 5, 0};
  fsn_t next = fsn_add_delta(current, 12);
  int diff;

  diff = fsn_get_diff(current, next);
  EXPECT_EQ(diff, -12);

  diff = fsn_get_diff(next, current);
  EXPECT_EQ(diff, 12);
}

TEST(fsn, test_mu0_get_max) {
  fsn_t current = {123, 5, 0};
  fsn_t next = {125, 7, 0};

  fsn_t max = fsn_get_max(current, next);
  EXPECT_EQ(max.f, next.f);
  EXPECT_EQ(max.s, next.s);
  EXPECT_EQ(max.mu, next.mu);
}

TEST(fsn, test_mu0_equal) {
  fsn_t a = {123, 5, 0};
  fsn_t b = a;
  fsn_t c = {123, 6, 0};

  EXPECT_TRUE(fsn_equal(a, b));
  EXPECT_FALSE(fsn_equal(a, c));
}

TEST(fsn, test_mu0_in_the_past) {
  fsn_t a = {0, 0, 0};
  fsn_t n = {100, 9, 0};
  fsn_t b_bef = {511, 9, 0};
  fsn_t b_ex = {512, 0, 0};
  fsn_t b_aft = {512, 1, 0};

  // a is in the past of n
  EXPECT_TRUE(fsn_in_the_past(a, n));
  // similarly, a is in the past of b_bef
  EXPECT_TRUE(fsn_in_the_past(a, b_bef));
  // difference "to both sides" is 512 frames, so we consider a still in the
  // past of b_ex
  EXPECT_TRUE(fsn_in_the_past(a, b_ex));
  // (b_aft - a) < (a - b_aft) => b_aft is in the past of a because of
  // wrap-around
  EXPECT_TRUE(fsn_in_the_past(b_aft, a));
}

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
