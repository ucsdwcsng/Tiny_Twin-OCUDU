/* SPDX-License-Identifier: LicenseRef-CSSL-1.0 */
#include "channel.h"
#include <gtest/gtest.h>
#include <limits>
#include <cstring>
#include <stdexcept>

TEST(Channel, IdentityPreservesSamples)
{
  Channel channel;
  c16_t samples[] = {{-32768, 32767}, {123, -456}};
  channel.process(samples, 2, 100);
  EXPECT_EQ(samples[0].r, -32768);
  EXPECT_EQ(samples[0].i, 32767);
  EXPECT_EQ(samples[1].r, 123);
  EXPECT_EQ(samples[1].i, -456);
}

TEST(Channel, ComplexImpulseCrossesBlockBoundary)
{
  Channel channel;
  channel.set_taps({{1, 0}, {0, 1}, {0.5, -0.5}});
  c16_t first[] = {{0, 0}, {100, 20}};
  channel.process(first, 2, 10);
  EXPECT_EQ(first[1].r, 100);
  EXPECT_EQ(first[1].i, 20);
  c16_t second[] = {{0, 0}, {0, 0}, {0, 0}};
  channel.process(second, 3, 12);
  EXPECT_EQ(second[0].r, -20);
  EXPECT_EQ(second[0].i, 100);
  EXPECT_EQ(second[1].r, 60);
  EXPECT_EQ(second[1].i, -40);
  EXPECT_EQ(second[2].r, 0);
  EXPECT_EQ(second[2].i, 0);
}

TEST(Channel, MaximumDelaySurvivesSingleSampleBlocks)
{
  Channel channel;
  std::vector<std::complex<float>> taps(Channel::MAX_TAPS);
  taps.back() = {1, 0};
  channel.set_taps(taps);
  for (uint64_t n = 0; n < 21; ++n) {
    c16_t sample = {static_cast<int16_t>(n == 0 ? 100 : 0), 0};
    channel.process(&sample, 1, n);
    EXPECT_EQ(sample.r, n == 19 ? 100 : 0);
  }
}

TEST(Channel, SaturatesBothComponents)
{
  Channel channel;
  channel.set_taps({{1, 0}, {1, 0}});
  c16_t samples[] = {{20000, -20000}, {20000, -20000}};
  channel.process(samples, 2, 0);
  EXPECT_EQ(samples[1].r, 32767);
  EXPECT_EQ(samples[1].i, -32768);
}

TEST(Channel, TimestampGapAdvancesHistory)
{
  Channel channel;
  channel.set_taps({{0, 0}, {0, 0}, {1, 0}});
  c16_t sample = {100, 0};
  channel.process(&sample, 1, 0);
  sample = {};
  channel.process(&sample, 1, 2);
  EXPECT_EQ(sample.r, 100);
  sample = {};
  channel.process(&sample, 1, 100);
  EXPECT_EQ(sample.r, 0);
}

TEST(Channel, ResetAndTimestampRewindClearHistory)
{
  Channel channel;
  channel.set_taps({{0, 0}, {1, 0}});
  c16_t sample = {100, 0};
  channel.process(&sample, 1, 10);
  sample = {};
  channel.process(&sample, 1, 0);
  EXPECT_EQ(sample.r, 0);
  sample = {100, 0};
  channel.process(&sample, 1, 1);
  channel.reset();
  sample = {};
  channel.process(&sample, 1, 2);
  EXPECT_EQ(sample.r, 0);
}

TEST(Channel, EmptyBlockDoesNotAdvanceHistory)
{
  Channel channel;
  channel.set_taps({{0, 0}, {1, 0}});
  c16_t sample = {100, 0};
  channel.process(&sample, 1, 10);
  channel.process(nullptr, 0, 100);
  sample = {};
  channel.process(&sample, 1, 11);
  EXPECT_EQ(sample.r, 100);
}

TEST(Channel, ParsesAndValidatesTaps)
{
  auto taps = Channel::parse_taps("1,0; 0.5,-0.25");
  ASSERT_EQ(taps.size(), 2U);
  EXPECT_EQ(taps[1], std::complex<float>(0.5, -0.25));
  for (const char *text : {"", "1", "1,0;", "1,0;;0,1", "nan,0", "inf,0", "1,0junk", "1:0"})
    EXPECT_THROW(Channel::parse_taps(text), std::invalid_argument) << text;
  Channel channel;
  EXPECT_THROW(channel.set_taps({}), std::invalid_argument);
  EXPECT_THROW(channel.set_taps(std::vector<std::complex<float>>(21)), std::invalid_argument);
  EXPECT_THROW(channel.set_taps({{std::numeric_limits<float>::infinity(), 0}}), std::invalid_argument);
  EXPECT_TRUE(channel.is_identity());
}

#include "trace_test_utils.h"

TEST(ChannelTrace, SameFileSuppliesBothComponentsAndAdvancesOncePerBlock)
{
  TapTraceFile file("0.5\n0.25\n");
  Channel channel;
  channel.set_trace(Channel::load_trace(file.path, "", 1));
  c16_t samples[] = {{100, 20}, {100, 20}, {100, 20}};
  channel.process(samples, 3, 0);
  for (const auto &sample : samples) {
    EXPECT_EQ(sample.r, 40);
    EXPECT_EQ(sample.i, 60);
  }
  c16_t next = {100, 20};
  channel.process(&next, 1, 3);
  EXPECT_EQ(next.r, 20);
  EXPECT_EQ(next.i, 30);
  // EOF restores real=[1], imag=[0] rather than retaining or repeating a row.
  next = {100, 20};
  channel.process(&next, 1, 4);
  EXPECT_EQ(next.r, 100);
  EXPECT_EQ(next.i, 20);
  channel.reset();
  channel.process(nullptr, 0, 0);
  next = {100, 20};
  channel.process(&next, 1, 0);
  EXPECT_EQ(next.r, 40);
  EXPECT_EQ(next.i, 60);
}

TEST(ChannelTrace, ChangingTapsPreservesHistoryAndDoesNotSkipIdentityRows)
{
  TapTraceFile real("1 0\n0 1\n"), imag("0 0\n0 0\n");
  Channel channel;
  channel.set_trace(Channel::load_trace(real.path, imag.path, 2));
  EXPECT_FALSE(channel.is_identity());
  c16_t sample = {100, -50};
  channel.process(&sample, 1, 0);
  sample = {};
  channel.process(&sample, 1, 1);
  EXPECT_EQ(sample.r, 100);
  EXPECT_EQ(sample.i, -50);
}

TEST(ChannelTrace, ShortRowsAndIndependentEofUseTinyTwinDefaults)
{
  TapTraceFile real("0.5\n"), imag("0 0.25\n0.5\n");
  Channel channel;
  channel.set_trace(Channel::load_trace(real.path, imag.path, 2));
  c16_t samples[] = {{100, 0}, {0, 0}};
  channel.process(samples, 2, 0);
  EXPECT_EQ(samples[0].r, 50);
  EXPECT_EQ(samples[1].i, 25);
  c16_t next = {100, 0};
  channel.process(&next, 1, 2);
  EXPECT_EQ(next.r, 100);
  EXPECT_EQ(next.i, 50);
  next = {100, 0};
  channel.process(&next, 1, 3);
  EXPECT_EQ(next.r, 100);
  EXPECT_EQ(next.i, 0);
}

TEST(ChannelTrace, TraceUsesNearestIntegerRoundingAndSaturation)
{
  TapTraceFile real("0.5\n2\n"), imag("0\n0\n");
  Channel channel;
  channel.set_trace(Channel::load_trace(real.path, imag.path, 1));
  c16_t sample = {1, -1};
  channel.process(&sample, 1, 0);
  EXPECT_EQ(sample.r, 1);
  EXPECT_EQ(sample.i, -1);
  sample = {20000, -20000};
  channel.process(&sample, 1, 1);
  EXPECT_EQ(sample.r, 32767);
  EXPECT_EQ(sample.i, -32768);
}

TEST(ChannelTrace, SharedDataHasIndependentReplayCursors)
{
  TapTraceFile file("0.5\n0.25\n");
  auto trace = Channel::load_trace(file.path, "", 1);
  Channel a, b;
  a.set_trace(trace);
  b.set_trace(trace);
  c16_t sample = {100, 0};
  a.process(&sample, 1, 0);
  sample = {100, 0};
  a.process(&sample, 1, 1);
  EXPECT_EQ(sample.r, 25);
  sample = {100, 0};
  b.process(&sample, 1, 0);
  EXPECT_EQ(sample.r, 50);
}

TEST(ChannelTrace, ValidatesFilesAndTapCount)
{
  TapTraceFile bad("nan\n"), valid("1\n"), empty("");
  EXPECT_THROW(Channel::load_trace(valid.path + ".missing", "", 1), std::invalid_argument);
  EXPECT_THROW(Channel::load_trace(bad.path, "", 1), std::invalid_argument);
  EXPECT_THROW(Channel::load_trace(valid.path, "", 0), std::invalid_argument);
  EXPECT_THROW(Channel::load_trace(valid.path, "", 21), std::invalid_argument);
  EXPECT_TRUE(Channel::load_trace(empty.path, "", 1)->rows.empty());
}

TEST(ChannelImpairments, PathGainIsAppliedAfterConvolution)
{
  Channel channel;
  channel.set_taps({{1, 0}, {1, 0}});
  channel.set_impairments(-6.020599913279624, -std::numeric_limits<double>::infinity());
  c16_t samples[] = {{20000, -20000}, {20000, -20000}};
  channel.process(samples, 2, 0);
  EXPECT_EQ(samples[0].r, 10000);
  EXPECT_EQ(samples[1].r, 20000); // Gain is applied before saturation.
  EXPECT_EQ(samples[1].i, -20000);
}

TEST(ChannelImpairments, TinyTwinNoiseScaleAndSeedAreReproducible)
{
  Channel channel;
  channel.set_taps({{0, 0}});
  channel.set_impairments(0, -10, 1234);
  std::vector<c16_t> samples(100000);
  channel.process(samples.data(), samples.size(), 0);
  double mean_r = 0, mean_i = 0, energy_r = 0, energy_i = 0;
  for (const auto &s : samples) {
    mean_r += s.r;
    mean_i += s.i;
    energy_r += double(s.r) * s.r;
    energy_i += double(s.i) * s.i;
  }
  EXPECT_NEAR(mean_r / samples.size(), 0, 0.3);
  EXPECT_NEAR(mean_i / samples.size(), 0, 0.3);
  // Tiny_Twin uses sigma = 256 * 10^(-10/10) = 25.6 per IQ component.
  EXPECT_NEAR(energy_r / samples.size(), 655.36, 15);
  EXPECT_NEAR(energy_i / samples.size(), 655.36, 15);
  channel.reset();
  std::vector<c16_t> replay(samples.size());
  channel.process(replay.data(), replay.size(), 0);
  EXPECT_EQ(memcmp(samples.data(), replay.data(), samples.size() * sizeof(c16_t)), 0);
  Channel attenuated;
  attenuated.set_taps({{0, 0}});
  attenuated.set_impairments(-20, -10, 1234);
  std::fill(replay.begin(), replay.end(), c16_t{});
  attenuated.process(replay.data(), replay.size(), 0);
  EXPECT_EQ(memcmp(samples.data(), replay.data(), samples.size() * sizeof(c16_t)), 0);
}
