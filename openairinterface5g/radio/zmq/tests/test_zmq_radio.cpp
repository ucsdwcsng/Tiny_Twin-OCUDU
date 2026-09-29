/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "common_lib.h"
#include <gtest/gtest.h>
#include "common/config/config_userapi.h"
#include <zmq.h>
extern "C" {
#include "common/config/config_userapi.h"
#include "openair1/SIMULATION/TOOLS/sim.h"
extern int device_init(openair0_device_t *device, openair0_config_t *openair0_cfg);
static softmodem_params_t softmodem_params;
softmodem_params_t *get_softmodem_params(void)
{
  return &softmodem_params;
}
}
#include "common/platform_types.h"
#include <thread>
#include <algorithm>
#include <atomic>
#include <chrono>
#include "zmq_imported.h"
#include "trace_test_utils.h"

configmodule_interface_t *uniqCfg = NULL;

extern "C" void exit_function(const char *file, const char *function, const int line, const char *s, const int assert)
{
  fprintf(stderr, "FATAL: %s at %s:%s:%d\n", s, file, function, line);
  exit(EXIT_FAILURE);
}

class ZMQTest : public ::testing::Test {
 protected:
  configmodule_interface_t *cfg1 = nullptr;
  configmodule_interface_t *cfg2 = nullptr;
  openair0_device_t device1 = {0};
  openair0_device_t device2 = {0};
  openair0_config_t config1 = {0};
  openair0_config_t config2 = {0};
  std::vector<char *> argv;
  virtual const char *tx_taps() const { return "1,0"; }
  virtual const char *rx_taps() const { return "1,0"; }
  virtual std::vector<std::string> extra_config() const { return {}; }

  void SetUp() override
  {
    argv.resize(8);
    argv[0] = strdup("--zmq.[0].tx_channels");
    argv[1] = strdup("tcp://127.0.0.1:5555");
    argv[2] = strdup("--zmq.[0].rx_channels");
    argv[3] = strdup("tcp://127.0.0.1:5556");
    argv[4] = strdup("--zmq.[0].tx_taps");
    argv[5] = strdup(tx_taps());
    argv[6] = strdup("--zmq.[0].rx_taps");
    argv[7] = strdup(rx_taps());

    for (const auto &arg : extra_config())
      argv.push_back(strdup(arg.c_str()));
    cfg1 = load_configmodule(argv.size(), argv.data(), CONFIG_ENABLECMDLINEONLY);
    uniqCfg = cfg1;
    // 2. Initialize the ZMQ device
    config1.tx_num_channels = 1;
    config1.rx_num_channels = 1;
    config1.sample_rate = 30;
    ASSERT_EQ(device_init(&device1, &config1), 0);
    ASSERT_EQ(device1.trx_start_func(&device1), 0);

    // Swap the RX with TX for second device
    char *tmp = argv[0];
    argv[0] = argv[2];
    argv[2] = tmp;
    cfg2 = load_configmodule(4, argv.data(), CONFIG_ENABLECMDLINEONLY);
    uniqCfg = cfg2;
    config2.tx_num_channels = 1;
    config2.rx_num_channels = 1;
    config2.sample_rate = 1500;
    ASSERT_EQ(device_init(&device2, &config2), 0);
    ASSERT_EQ(device2.trx_start_func(&device2), 0);
  }

  void TearDown() override
  {
    if (device1.trx_end_func) {
      device1.trx_end_func(&device1);
    }
    if (device2.trx_end_func) {
      device2.trx_end_func(&device2);
    }
    if (cfg1) {
      end_configmodule(cfg1);
    }
    if (cfg2) {
      end_configmodule(cfg2);
    }
    for (auto i = 0U; i < argv.size(); i++) {
      free(argv[i]);
    }
  }
};

TEST_F(ZMQTest, RXSamples)
{
  std::thread t1([this]() {
    c16_t rx_samples[10];
    openair0_timestamp_t rx_timestamp;
    void *samples[1] = {rx_samples};
    ASSERT_EQ(device1.trx_read_func(&device1, &rx_timestamp, samples, 10, 1), 10);
    for (int i = 0; i < 10; i++) {
      ASSERT_EQ(rx_samples[i].r, 0);
      ASSERT_EQ(rx_samples[i].i, 0);
    }
  });
  std::thread t2([this]() {
    c16_t rx_samples[10];
    openair0_timestamp_t rx_timestamp;
    void *samples[1] = {rx_samples};
    ASSERT_EQ(device2.trx_read_func(&device2, &rx_timestamp, samples, 10, 1), 10);
    for (int i = 0; i < 10; i++) {
      ASSERT_EQ(rx_samples[i].r, 0);
      ASSERT_EQ(rx_samples[i].i, 0);
    }
  });
  t1.join();
  t2.join();
}

TEST_F(ZMQTest, TxRxSamples)
{
  std::thread t1([this]() {
    c16_t rx_samples[10];
    openair0_timestamp_t rx_timestamp;
    void *samples[1] = {rx_samples};
    ASSERT_EQ(device1.trx_read_func(&device1, &rx_timestamp, samples, 10, 1), 10);
    for (int i = 0; i < 10; i++) {
      ASSERT_EQ(rx_samples[i].r, 0);
      ASSERT_EQ(rx_samples[i].i, 0);
    }
    c16_t tx_samples[10];
    openair0_timestamp_t tx_timestamp = rx_timestamp + 10;
    for (int i = 0; i < 10; i++) {
      tx_samples[i].r = i;
      tx_samples[i].i = i + 1;
    }
    samples[0] = tx_samples;
    ASSERT_EQ(device1.trx_write_func(&device1, tx_timestamp, samples, 10, 1, 0), 10);
  });
  std::thread t2([this]() {
    c16_t rx_samples[10];
    openair0_timestamp_t rx_timestamp;
    void *samples[1] = {rx_samples};
    ASSERT_EQ(device2.trx_read_func(&device2, &rx_timestamp, samples, 10, 1), 10);
    for (int i = 0; i < 10; i++) {
      ASSERT_EQ(rx_samples[i].r, 0);
      ASSERT_EQ(rx_samples[i].i, 0);
    }
    openair0_timestamp_t rx_timestamp2;
    ASSERT_EQ(device2.trx_read_func(&device2, &rx_timestamp2, samples, 10, 1), 10);
    for (int i = 0; i < 10; i++) {
      ASSERT_EQ(rx_samples[i].r, i);
      ASSERT_EQ(rx_samples[i].i, i + 1);
    }
    ASSERT_EQ(rx_timestamp + 10, rx_timestamp2);
  });
  t1.join();
  t2.join();
}

TEST_F(ZMQTest, TxRxSamplesSIMD)
{
  const int size = 100;
  std::thread t1([this, size]() {
    c16_t rx_samples[size];
    openair0_timestamp_t rx_timestamp;
    void *samples[1] = {rx_samples};
    ASSERT_EQ(device1.trx_read_func(&device1, &rx_timestamp, samples, size, 1), size);
    for (int i = 0; i < size; i++) {
      ASSERT_EQ(rx_samples[i].r, 0);
      ASSERT_EQ(rx_samples[i].i, 0);
    }
    c16_t tx_samples[size];
    openair0_timestamp_t tx_timestamp = rx_timestamp + size;
    for (int i = 0; i < size; i++) {
      tx_samples[i].r = (int16_t)i;
      tx_samples[i].i = (int16_t)(i + 1);
    }
    samples[0] = tx_samples;
    ASSERT_EQ(device1.trx_write_func(&device1, tx_timestamp, samples, size, 1, 0), size);
  });
  std::thread t2([this, size]() {
    c16_t rx_samples[size];
    openair0_timestamp_t rx_timestamp;
    void *samples[1] = {rx_samples};
    ASSERT_EQ(device2.trx_read_func(&device2, &rx_timestamp, samples, size, 1), size);
    for (int i = 0; i < size; i++) {
      ASSERT_EQ(rx_samples[i].r, 0);
      ASSERT_EQ(rx_samples[i].i, 0);
    }
    openair0_timestamp_t rx_timestamp2;
    ASSERT_EQ(device2.trx_read_func(&device2, &rx_timestamp2, samples, size, 1), size);
    for (int i = 0; i < size; i++) {
      ASSERT_EQ(rx_samples[i].r, (int16_t)i);
      ASSERT_EQ(rx_samples[i].i, (int16_t)(i + 1));
    }
    ASSERT_EQ(rx_timestamp + size, rx_timestamp2);
  });
  t1.join();
  t2.join();
}

TEST_F(ZMQTest, BenchmarkThroughput)
{
  int level = g_log->log_component[HW].level;
  g_log->log_component[HW].level = OAILOG_ERR;
  const size_t nsamps = 10000;
  const size_t num_iters = 100000;
  c16_t tx_samples[nsamps];
  for (size_t i = 0; i < nsamps; i++) {
    tx_samples[i].r = i;
    tx_samples[i].i = i + 1;
  }

  void *samples[1] = {tx_samples};
  openair0_timestamp_t tx_timestamp = 0;

  auto start = std::chrono::high_resolution_clock::now();

  for (size_t i = 0; i < num_iters; i++) {
    ASSERT_EQ(device1.trx_write_func(&device1, tx_timestamp, samples, nsamps, 1, 0), nsamps);
    tx_timestamp += nsamps;
  }

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> diff = end - start;

  std::cout << "Benchmark:" << std::endl;
  std::cout << "Time taken: " << diff.count() << " s\n";
  std::cout << "Throughput: " << (nsamps * num_iters) / diff.count() / 1e6 << " MSamples/s\n";

  // No need to drain messages as device shutdown handles cleanup
  g_log->log_component[HW].level = level;
}

TEST_F(ZMQTest, TxReplyIsSentAsSoonAsSamplesAreQueued)
{
  // device2 requests samples from device1 before device1 has any to send. The reply must go out as soon as
  // device1 queues samples, not when the TX poll thread's 10 ms poll timeout expires.
  const int iterations = 20;
  std::vector<double> latency_ms;
  c16_t tx_sample = {100, 200};
  for (int n = 0; n < iterations; n++) {
    // Let the pending request reach device1's TX poll thread with its queue empty.
    std::this_thread::sleep_for(std::chrono::milliseconds(3));
    void *tx[] = {&tx_sample};
    auto start = std::chrono::steady_clock::now();
    ASSERT_EQ(device1.trx_write_func(&device1, n, tx, 1, 1, 0), 1);
    c16_t received;
    void *rx[] = {&received};
    openair0_timestamp_t timestamp;
    ASSERT_EQ(device2.trx_read_func(&device2, &timestamp, rx, 1, 1), 1);
    latency_ms.push_back(std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count());
    EXPECT_EQ(received.r, 100);
    EXPECT_EQ(received.i, 200);
  }
  std::sort(latency_ms.begin(), latency_ms.end());
  // Waiting out the poll timeout would take ~7 ms here (10 ms minus the 3 ms sleep).
  EXPECT_LT(latency_ms[iterations / 2], 2.0) << "median write-to-read latency";
}

class ZMQMultiTapTest : public ZMQTest {
 protected:
  const char *tx_taps() const override { return "1,0;0.5,0"; }
  const char *rx_taps() const override { return "1,0;0,0.5"; }
};

TEST_F(ZMQMultiTapTest, ConfiguredTapsAffectBothDirections)
{
  c16_t impulse[] = {{1000, 0}, {0, 0}, {0, 0}};
  void *tx[] = {impulse};
  ASSERT_EQ(device1.trx_write_func(&device1, 0, tx, 3, 1, 0), 3);
  ASSERT_EQ(device2.trx_write_func(&device2, 15, tx, 3, 1, 0), 3);
  // TX processing must not modify the caller's buffer.
  EXPECT_EQ(impulse[1].r, 0);
  c16_t received[3]{};
  void *rx[] = {received};
  openair0_timestamp_t timestamp;
  ASSERT_EQ(device2.trx_read_func(&device2, &timestamp, rx, 3, 1), 3);
  EXPECT_EQ(received[0].r, 1000);
  EXPECT_EQ(received[1].r, 500);
  EXPECT_EQ(received[1].i, 0);
  ASSERT_EQ(device1.trx_read_func(&device1, &timestamp, rx, 3, 1), 3);
  EXPECT_EQ(received[0].r, 1000);
  EXPECT_EQ(received[1].r, 0);
  EXPECT_EQ(received[1].i, 500);
}

class ZMQDisabledMultiTapTest : public ZMQMultiTapTest {
 protected:
  std::vector<std::string> extra_config() const override
  {
    return {"--zmq.[0].channel_effects_enabled", "0"};
  }
};

TEST_F(ZMQDisabledMultiTapTest, ChannelEffectsAreBypassedInBothDirections)
{
  c16_t impulse[] = {{1000, 0}, {0, 0}, {0, 0}};
  void *tx[] = {impulse};
  ASSERT_EQ(device1.trx_write_func(&device1, 0, tx, 3, 1, 0), 3);
  ASSERT_EQ(device2.trx_write_func(&device2, 15, tx, 3, 1, 0), 3);

  c16_t received[3]{};
  void *rx[] = {received};
  openair0_timestamp_t timestamp;
  ASSERT_EQ(device2.trx_read_func(&device2, &timestamp, rx, 3, 1), 3);
  EXPECT_EQ(received[0].r, 1000);
  EXPECT_EQ(received[1].r, 0);
  EXPECT_EQ(received[1].i, 0);

  ASSERT_EQ(device1.trx_read_func(&device1, &timestamp, rx, 3, 1), 3);
  EXPECT_EQ(received[0].r, 1000);
  EXPECT_EQ(received[1].r, 0);
  EXPECT_EQ(received[1].i, 0);
}

class ZMQTraceTest : public ZMQTest {
 protected:
  TapTraceFile trace{"1\n0.5\n"};
  std::vector<std::string> extra_config() const override
  {
    return {"--zmq.[0].tx_tap_file", trace.path, "--zmq.[0].rx_tap_file", trace.path,
            "--zmq.[0].tx_path_gain_db", "-6.020599913279624"};
  }
};

TEST_F(ZMQTraceTest, ConfiguredTraceChangesOnEachRadioCallback)
{
  c16_t impulse = {1000, 0};
  void *tx[] = {&impulse};
  ASSERT_EQ(device1.trx_write_func(&device1, 0, tx, 1, 1, 0), 1);
  ASSERT_EQ(device1.trx_write_func(&device1, 1, tx, 1, 1, 0), 1);
  ASSERT_EQ(device1.trx_write_func(&device1, 2, tx, 1, 1, 0), 1);
  c16_t received[3]{};
  void *rx[] = {received};
  openair0_timestamp_t timestamp;
  ASSERT_EQ(device2.trx_read_func(&device2, &timestamp, rx, 3, 1), 3);
  EXPECT_EQ(received[0].r, 500);
  EXPECT_EQ(received[0].i, 500);
  EXPECT_EQ(received[1].r, 250);
  EXPECT_EQ(received[1].i, 250);
  EXPECT_EQ(received[2].r, 500);
  EXPECT_EQ(received[2].i, 0); // EOF returns to identity, retaining path gain.

  c16_t downlink[] = {{1000, 0}, {1000, 0}, {1000, 0}};
  tx[0] = downlink;
  ASSERT_EQ(device2.trx_write_func(&device2, 18, tx, 3, 1, 0), 3);
  // The previous read aligned peer TX from 15 to 18 with silence.
  ASSERT_EQ(device1.trx_read_func(&device1, &timestamp, rx, 3, 1), 3);
  for (int i = 0; i < 3; ++i) {
    EXPECT_EQ(received[i].r, 0);
    EXPECT_EQ(received[i].i, 0);
  }
  ASSERT_EQ(device1.trx_read_func(&device1, &timestamp, rx, 1, 1), 1);
  EXPECT_EQ(received[0].r, 500);
  EXPECT_EQ(received[0].i, 500);
  ASSERT_EQ(device1.trx_read_func(&device1, &timestamp, rx, 1, 1), 1);
  EXPECT_EQ(received[0].r, 1000);
  EXPECT_EQ(received[0].i, 0);
}

static std::vector<cf_t> drain_samples(zmq_tx_channel &channel)
{
  std::vector<cf_t> samples;
  zmq_msg_t msg;
  while (channel.pop_message(&msg)) {
    const auto *data = static_cast<const cf_t *>(zmq_msg_data(&msg));
    samples.insert(samples.end(), data, data + zmq_msg_size(&msg) / sizeof(cf_t));
    zmq_msg_close(&msg);
  }
  return samples;
}

TEST(ZMQChannel, WaitMessageWakesAsSoonAsSamplesAreQueued)
{
  zmq_tx_channel channel(nullptr, 0);
  channel.start(0);
  std::atomic<bool> running = true;
  std::thread producer([&channel]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    c16_t sample = {1000, 0};
    channel.transmit(&sample, 1, 0);
  });
  zmq_msg_t msg;
  auto start = std::chrono::steady_clock::now();
  EXPECT_TRUE(channel.wait_message(&msg, running, std::chrono::seconds(5)));
  auto elapsed = std::chrono::steady_clock::now() - start;
  producer.join();
  EXPECT_LT(elapsed, std::chrono::seconds(1));
  EXPECT_EQ(zmq_msg_size(&msg), sizeof(cf_t));
  zmq_msg_close(&msg);
}

TEST(ZMQChannel, WaitMessageReturnsWhenStopped)
{
  zmq_tx_channel channel(nullptr, 0);
  std::atomic<bool> running = true;
  std::thread stopper([&channel, &running]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    running = false;
    channel.wake_waiters();
  });
  zmq_msg_t msg;
  auto start = std::chrono::steady_clock::now();
  EXPECT_FALSE(channel.wait_message(&msg, running, std::chrono::seconds(5)));
  auto elapsed = std::chrono::steady_clock::now() - start;
  stopper.join();
  EXPECT_LT(elapsed, std::chrono::seconds(1));
}

TEST(ZMQChannel, BurstGapContainsTailAndNextBurstHasNoStaleHistory)
{
  for (bool use_stream : {false, true}) {
    zmq_tx_channel channel(nullptr, 0);
    channel.channel_processor_.set_taps({{1, 0}, {0.5, 0}});
    zmq_tx_stream stream;
    stream.channels_.push_back(&channel);
    stream.start(0);
    c16_t first = {1000, 0}, second = {2000, 0};
    if (use_stream) {
      c16_t *buffers[] = {&first};
      stream.transmit(buffers, 1, 0);
      buffers[0] = &second;
      stream.transmit(buffers, 1, 4);
    } else {
      channel.transmit(&first, 1, 0);
      channel.transmit(&second, 1, 4);
    }
    stream.align(6, std::chrono::milliseconds(0));
    const auto samples = drain_samples(channel);
    ASSERT_EQ(samples.size(), 6U);
    const float expected[] = {1000, 500, 0, 0, 2000, 1000};
    for (size_t n = 0; n < samples.size(); ++n) {
      EXPECT_NEAR(samples[n].r * 32767.0F, expected[n], 0.001F);
      EXPECT_EQ(samples[n].i, 0);
    }
  }
}

TEST(ZMQChannel, AlignmentSilenceFlushesTailAndLateWriteDoesNotChangeHistory)
{
  zmq_tx_channel channel(nullptr, 0);
  channel.channel_processor_.set_taps({{0, 0}, {0, 0}, {1, 0}});
  channel.start(0);
  c16_t impulse = {1000, 0};
  channel.transmit(&impulse, 1, 0);
  channel.align(2, std::chrono::milliseconds(0));
  c16_t late = {9999, 0};
  channel.transmit(&late, 1, 1);
  channel.align(4, std::chrono::milliseconds(0));
  const auto samples = drain_samples(channel);
  ASSERT_EQ(samples.size(), 4U);
  EXPECT_EQ(samples[0].r, 0);
  EXPECT_EQ(samples[1].r, 0);
  EXPECT_NEAR(samples[2].r * 32767.0F, 1000, 0.001F);
  EXPECT_EQ(samples[3].r, 0);
}

TEST(ZMQChannel, PaddingAndRejectedWritesDoNotConsumeTraceRows)
{
  TapTraceFile real("0.5\n0.25\n"), imag("0\n0\n");
  zmq_tx_channel channel(nullptr, 0);
  channel.channel_processor_.set_trace(Channel::load_trace(real.path, imag.path, 1));
  channel.start(0);
  channel.align(2, std::chrono::milliseconds(0));
  c16_t sample = {1000, 0};
  channel.transmit(&sample, 1, 2);
  channel.transmit(&sample, 1, 1); // Late write must not consume row two.
  channel.align(4, std::chrono::milliseconds(0));
  channel.transmit(&sample, 1, 4);
  auto result = drain_samples(channel);
  ASSERT_EQ(result.size(), 5U);
  EXPECT_NEAR(result[2].r * 32767.0F, 500, 0.001);
  EXPECT_NEAR(result[4].r * 32767.0F, 250, 0.001);
}

int main(int argc, char **argv)
{
  logInit();
  g_log->log_component[HW].level = OAILOG_DEBUG;
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
