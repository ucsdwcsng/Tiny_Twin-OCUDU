/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include <gtest/gtest.h>
#include "symbol_reorder.h"

#include <chrono>
#include <future>
#include <thread>

class SymbolReorderTest : public ::testing::Test {
 protected:
  void TearDown() override
  {
    if (reorder)
      symbol_reorder_destroy(reorder);
  }

  // Fetches the current contiguous high-water mark. Only safe to call with an index that is
  // already known to have completed (e.g. the one just passed to a successful advance()) -
  // otherwise this blocks until it does.
  uint64_t latest_after(uint64_t known_completed_index)
  {
    return symbol_reorder_wait_at_least(reorder, known_completed_index, nullptr);
  }

  symbol_reorder_t *reorder = nullptr;
};

TEST_F(SymbolReorderTest, InOrderSingleAdvances)
{
  reorder = symbol_reorder_create(0, nullptr, 0);

  ASSERT_TRUE(symbol_reorder_advance(reorder, 0, 1));
  EXPECT_EQ(latest_after(0), 0u);

  ASSERT_TRUE(symbol_reorder_advance(reorder, 1, 1));
  EXPECT_EQ(latest_after(1), 1u);

  ASSERT_TRUE(symbol_reorder_advance(reorder, 2, 1));
  EXPECT_EQ(latest_after(2), 2u);
}

TEST_F(SymbolReorderTest, OutOfOrderCompletionsDrainOnceGapFills)
{
  reorder = symbol_reorder_create(0, nullptr, 0);

  // 2 and 1 arrive before 0 - neither can advance the frontier yet.
  EXPECT_FALSE(symbol_reorder_advance(reorder, 2, 1));
  EXPECT_FALSE(symbol_reorder_advance(reorder, 1, 1));

  // 0 arrives, filling the gap - the frontier should jump straight to 2.
  ASSERT_TRUE(symbol_reorder_advance(reorder, 0, 1));
  EXPECT_EQ(latest_after(0), 2u);
}

TEST_F(SymbolReorderTest, MultiIndexRangeAdvancesInOneJump)
{
  reorder = symbol_reorder_create(0, nullptr, 0);

  ASSERT_TRUE(symbol_reorder_advance(reorder, 0, 5));
  EXPECT_EQ(latest_after(0), 4u);

  ASSERT_TRUE(symbol_reorder_advance(reorder, 5, 1));
  EXPECT_EQ(latest_after(5), 5u);
}

TEST_F(SymbolReorderTest, ReAdvancingAnAlreadyPassedIndexDoesNotRegress)
{
  reorder = symbol_reorder_create(0, nullptr, 0);

  ASSERT_TRUE(symbol_reorder_advance(reorder, 0, 1));
  ASSERT_TRUE(symbol_reorder_advance(reorder, 1, 1));
  EXPECT_EQ(latest_after(1), 1u);

  // Symbol 0 completing again (e.g. a duplicate/late notification) must not move the frontier.
  EXPECT_FALSE(symbol_reorder_advance(reorder, 0, 1));
  EXPECT_EQ(latest_after(1), 1u);
}

TEST_F(SymbolReorderTest, ValidPositionMaskSkipsOverInvalidPositions)
{
  // Period of 4 indices; only positions 0 and 1 are ever expected to complete (e.g. DL symbols in
  // a repeating DL/DL/UL/UL TDD pattern). Bit 0 and bit 1 set, bits 2 and 3 unset.
  uint8_t mask = 0b00000011;
  reorder = symbol_reorder_create(0, &mask, 4);

  ASSERT_TRUE(symbol_reorder_advance(reorder, 0, 1));
  EXPECT_EQ(latest_after(0), 0u);

  // Completing index 1 should also skip forward automatically over the invalid indices 2 and 3,
  // landing the new high-water mark at 3 even though nothing ever completes there.
  ASSERT_TRUE(symbol_reorder_advance(reorder, 1, 1));
  EXPECT_EQ(latest_after(1), 3u);

  // The next valid position (4, i.e. position 0 of the next period) still needs a real completion.
  ASSERT_TRUE(symbol_reorder_advance(reorder, 4, 1));
  EXPECT_EQ(latest_after(4), 4u);
}

TEST_F(SymbolReorderTest, StartIndexOtherThanZero)
{
  reorder = symbol_reorder_create(100, nullptr, 0);

  EXPECT_FALSE(symbol_reorder_advance(reorder, 101, 1));
  ASSERT_TRUE(symbol_reorder_advance(reorder, 100, 1));
  EXPECT_EQ(latest_after(100), 101u);
}

TEST_F(SymbolReorderTest, WaitAtLeastBlocksUntilTargetCompletes)
{
  reorder = symbol_reorder_create(0, nullptr, 0);

  std::future<uint64_t> waiter =
      std::async(std::launch::async, [this] { return symbol_reorder_wait_at_least(reorder, 2, nullptr); });

  // The waiter should still be blocked - nothing has completed yet.
  EXPECT_EQ(waiter.wait_for(std::chrono::milliseconds(50)), std::future_status::timeout);

  ASSERT_TRUE(symbol_reorder_advance(reorder, 0, 3)); // completes indices 0, 1, 2 in one jump

  ASSERT_EQ(waiter.wait_for(std::chrono::seconds(2)), std::future_status::ready);
  EXPECT_EQ(waiter.get(), 2u);
}

TEST_F(SymbolReorderTest, NotifyAllWakesWaiterOnStopFlag)
{
  reorder = symbol_reorder_create(0, nullptr, 0);
  volatile int stop_flag = 0;

  // Target (100) will never actually complete - only the stop_flag should release the waiter.
  std::future<uint64_t> waiter =
      std::async(std::launch::async, [this, &stop_flag] { return symbol_reorder_wait_at_least(reorder, 100, &stop_flag); });

  EXPECT_EQ(waiter.wait_for(std::chrono::milliseconds(50)), std::future_status::timeout);

  stop_flag = 1;
  symbol_reorder_notify_all(reorder);

  ASSERT_EQ(waiter.wait_for(std::chrono::seconds(2)), std::future_status::ready);
  waiter.get(); // must not hang or throw - the returned value is unspecified since target was never reached
}

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
