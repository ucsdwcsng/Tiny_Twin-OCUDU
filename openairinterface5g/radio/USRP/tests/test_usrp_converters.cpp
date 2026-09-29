/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include <gtest/gtest.h>
#include <uhd/convert.hpp>
#include <vector>
#include <cstdint>
#include <cstring>
#include <memory>
#include <utility>
#include "usrp_converters.hpp"

#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
constexpr bool HOST_IS_BIG_ENDIAN = true;
#else
constexpr bool HOST_IS_BIG_ENDIAN = false;
#endif

// Helper to manually compute expected RX conversion
static int16_t compute_expected_rx(int16_t val, int shift, bool swap_bytes)
{
  if (swap_bytes) {
    val = (val << 8) | ((val >> 8) & 0x00FF);
  }
  // Arithmetic right shift
  return val >> shift;
}

// Helper to manually compute expected TX conversion
static int16_t compute_expected_tx(int16_t val, int shift, bool swap_bytes)
{
  val = val << shift;
  if (swap_bytes) {
    val = (val << 8) | ((val >> 8) & 0x00FF);
  }
  return val;
}

// Independent reference model for UHD's item32 wire packing (real << 16) | imag,
// so a regression that drops the I/Q pair swap is actually caught, not just
// echoed back by testing against the converter's own formula.
static void compute_expected_pair(
    int16_t real_in, int16_t imag_in, int shift, bool swap_bytes, bool swap_iq, int16_t& real_out, int16_t& imag_out)
{
  auto process = [&](int16_t v) { return compute_expected_rx(v, shift, swap_bytes); };
  int16_t r = process(real_in);
  int16_t i = process(imag_in);
  if (swap_iq) {
    std::swap(r, i);
  }
  real_out = r;
  imag_out = i;
}

static void compute_expected_tx_pair(
    int16_t real_in, int16_t imag_in, int shift, bool swap_bytes, bool swap_iq, int16_t& out0, int16_t& out1)
{
  int16_t r = compute_expected_tx(real_in, shift, swap_bytes);
  int16_t i = compute_expected_tx(imag_in, shift, swap_bytes);
  if (swap_iq) {
    out0 = i;
    out1 = r;
  } else {
    out0 = r;
    out1 = i;
  }
}

class UsrpConvertersTest : public ::testing::Test {
 protected:
  void SetUp() override
  {
    // Register converters with default shift of 4
    register_oai_converters(4);
  }
};

TEST_F(UsrpConvertersTest, LookupConverters)
{
  // Test lookup of LE converters
  uhd::convert::id_type rx_id_le;
  rx_id_le.input_format = "sc16_item32_le";
  rx_id_le.num_inputs = 1;
  rx_id_le.output_format = "sc16_oai";
  rx_id_le.num_outputs = 1;

  uhd::convert::function_type rx_fcn_le = uhd::convert::get_converter(rx_id_le);
  ASSERT_TRUE(rx_fcn_le);
  uhd::convert::converter::sptr rx_conv_le = rx_fcn_le();
  ASSERT_TRUE(rx_conv_le);

  uhd::convert::id_type tx_id_le;
  tx_id_le.input_format = "sc16_oai";
  tx_id_le.num_inputs = 1;
  tx_id_le.output_format = "sc16_item32_le";
  tx_id_le.num_outputs = 1;

  uhd::convert::function_type tx_fcn_le = uhd::convert::get_converter(tx_id_le);
  ASSERT_TRUE(tx_fcn_le);
  uhd::convert::converter::sptr tx_conv_le = tx_fcn_le();
  ASSERT_TRUE(tx_conv_le);

  // Test lookup of BE converters
  uhd::convert::id_type rx_id_be;
  rx_id_be.input_format = "sc16_item32_be";
  rx_id_be.num_inputs = 1;
  rx_id_be.output_format = "sc16_oai";
  rx_id_be.num_outputs = 1;

  uhd::convert::function_type rx_fcn_be = uhd::convert::get_converter(rx_id_be);
  ASSERT_TRUE(rx_fcn_be);
  uhd::convert::converter::sptr rx_conv_be = rx_fcn_be();
  ASSERT_TRUE(rx_conv_be);

  uhd::convert::id_type tx_id_be;
  tx_id_be.input_format = "sc16_oai";
  tx_id_be.num_inputs = 1;
  tx_id_be.output_format = "sc16_item32_be";
  tx_id_be.num_outputs = 1;

  uhd::convert::function_type tx_fcn_be = uhd::convert::get_converter(tx_id_be);
  ASSERT_TRUE(tx_fcn_be);
  uhd::convert::converter::sptr tx_conv_be = tx_fcn_be();
  ASSERT_TRUE(tx_conv_be);

  // Test lookup of CHDR converters (RFNoC devices: X3xx, N3xx, X4xx)
  uhd::convert::id_type rx_id_chdr;
  rx_id_chdr.input_format = "sc16_chdr";
  rx_id_chdr.num_inputs = 1;
  rx_id_chdr.output_format = "sc16_oai";
  rx_id_chdr.num_outputs = 1;

  uhd::convert::function_type rx_fcn_chdr = uhd::convert::get_converter(rx_id_chdr);
  ASSERT_TRUE(rx_fcn_chdr);
  uhd::convert::converter::sptr rx_conv_chdr = rx_fcn_chdr();
  ASSERT_TRUE(rx_conv_chdr);

  uhd::convert::id_type tx_id_chdr;
  tx_id_chdr.input_format = "sc16_oai";
  tx_id_chdr.num_inputs = 1;
  tx_id_chdr.output_format = "sc16_chdr";
  tx_id_chdr.num_outputs = 1;

  uhd::convert::function_type tx_fcn_chdr = uhd::convert::get_converter(tx_id_chdr);
  ASSERT_TRUE(tx_fcn_chdr);
  uhd::convert::converter::sptr tx_conv_chdr = tx_fcn_chdr();
  ASSERT_TRUE(tx_conv_chdr);
}

void run_rx_test(
    const std::string& input_format, int shift, bool swap_bytes, bool swap_iq, int offset_in, int offset_out, size_t num_samples)
{
  uhd::convert::id_type rx_id;
  rx_id.input_format = input_format;
  rx_id.num_inputs = 1;
  rx_id.output_format = "sc16_oai";
  rx_id.num_outputs = 1;

  uhd::convert::converter::sptr rx_conv = uhd::convert::get_converter(rx_id)();
  ASSERT_TRUE(rx_conv);

  // Allocate 64-byte aligned buffers
  alignas(64) int16_t raw_in[2048];
  alignas(64) int16_t raw_out[2048];

  // Initialize input with test pattern
  for (int i = 0; i < 2048; ++i) {
    raw_in[i] = static_cast<int16_t>(i * 3 + 7);
  }
  std::memset(raw_out, 0, sizeof(raw_out));

  int16_t* in_ptr = &raw_in[offset_in];
  int16_t* out_ptr = &raw_out[offset_out];

  // Check alignment
  bool is_in_aligned = (((uintptr_t)in_ptr) & 0x3F) == 0;
  bool is_out_aligned = (((uintptr_t)out_ptr) & 0x3F) == 0;

  uhd::ref_vector<const void*> in_vec(in_ptr);
  uhd::ref_vector<void*> out_vec(out_ptr);

  rx_conv->conv(in_vec, out_vec, num_samples);

  // Verify against the independent item32 reference model, not the converter's own formula.
  for (size_t s = 0; s < num_samples; ++s) {
    int16_t expected_real, expected_imag;
    compute_expected_pair(
        in_ptr[2 * s], in_ptr[2 * s + 1], shift, swap_bytes, swap_iq, expected_real, expected_imag);
    EXPECT_EQ(out_ptr[2 * s], expected_real) << "Real mismatch at sample " << s << " with num_samples=" << num_samples
                                             << ", in_aligned=" << is_in_aligned << ", out_aligned=" << is_out_aligned;
    EXPECT_EQ(out_ptr[2 * s + 1], expected_imag) << "Imag mismatch at sample " << s << " with num_samples=" << num_samples
                                                 << ", in_aligned=" << is_in_aligned << ", out_aligned=" << is_out_aligned;
  }
}

void run_tx_test(
    const std::string& output_format, int shift, bool swap_bytes, bool swap_iq, int offset_in, int offset_out, size_t num_samples)
{
  uhd::convert::id_type tx_id;
  tx_id.input_format = "sc16_oai";
  tx_id.num_inputs = 1;
  tx_id.output_format = output_format;
  tx_id.num_outputs = 1;

  uhd::convert::converter::sptr tx_conv = uhd::convert::get_converter(tx_id)();
  ASSERT_TRUE(tx_conv);

  // Allocate 64-byte aligned buffers
  alignas(64) int16_t raw_in[2048];
  alignas(64) int16_t raw_out[2048];

  // Initialize input with test pattern
  for (int i = 0; i < 2048; ++i) {
    raw_in[i] = static_cast<int16_t>(i * 5 - 11);
  }
  std::memset(raw_out, 0, sizeof(raw_out));

  int16_t* in_ptr = &raw_in[offset_in];
  int16_t* out_ptr = &raw_out[offset_out];

  // Check alignment
  bool is_in_aligned = (((uintptr_t)in_ptr) & 0x3F) == 0;
  bool is_out_aligned = (((uintptr_t)out_ptr) & 0x3F) == 0;

  uhd::ref_vector<const void*> in_vec(in_ptr);
  uhd::ref_vector<void*> out_vec(out_ptr);

  tx_conv->conv(in_vec, out_vec, num_samples);

  // Verify against the independent item32 reference model, not the converter's own formula.
  for (size_t s = 0; s < num_samples; ++s) {
    int16_t expected0, expected1;
    compute_expected_tx_pair(
        in_ptr[2 * s], in_ptr[2 * s + 1], shift, swap_bytes, swap_iq, expected0, expected1);
    EXPECT_EQ(out_ptr[2 * s], expected0) << "Mismatch at wire word 0 of sample " << s << " with num_samples=" << num_samples
                                         << ", in_aligned=" << is_in_aligned << ", out_aligned=" << is_out_aligned;
    EXPECT_EQ(out_ptr[2 * s + 1], expected1) << "Mismatch at wire word 1 of sample " << s << " with num_samples=" << num_samples
                                             << ", in_aligned=" << is_in_aligned << ", out_aligned=" << is_out_aligned;
  }
}

TEST_F(UsrpConvertersTest, RxConverterLE_Shift4)
{
  constexpr bool swap_bytes = HOST_IS_BIG_ENDIAN;

  // Test combinations of alignments and sizes
  std::vector<int> offsets = {0, 1, 2, 4, 8, 15, 16, 32};
  std::vector<size_t> sample_counts = {1, 3, 7, 8, 15, 16, 31, 32, 63, 64, 128, 256};

  for (int off_in : offsets) {
    for (int off_out : offsets) {
      for (size_t num_s : sample_counts) {
        run_rx_test("sc16_item32_le", 4, swap_bytes, /*swap_iq=*/true, off_in, off_out, num_s);
      }
    }
  }
}

TEST_F(UsrpConvertersTest, RxConverterBE_Shift4)
{
  constexpr bool swap_bytes = !HOST_IS_BIG_ENDIAN;

  // Test combinations of alignments and sizes
  std::vector<int> offsets = {0, 1, 2, 4, 8, 15, 16, 32};
  std::vector<size_t> sample_counts = {1, 3, 7, 8, 15, 16, 31, 32, 63, 64, 128, 256};

  for (int off_in : offsets) {
    for (int off_out : offsets) {
      for (size_t num_s : sample_counts) {
        run_rx_test("sc16_item32_be", 4, swap_bytes, /*swap_iq=*/false, off_in, off_out, num_s);
      }
    }
  }
}

TEST_F(UsrpConvertersTest, TxConverterLE)
{
  constexpr bool swap_bytes = HOST_IS_BIG_ENDIAN;

  // Test combinations of alignments and sizes
  std::vector<int> offsets = {0, 1, 2, 4, 8, 15, 16, 32};
  std::vector<size_t> sample_counts = {1, 3, 7, 8, 15, 16, 31, 32, 63, 64, 128, 256};

  for (int off_in : offsets) {
    for (int off_out : offsets) {
      for (size_t num_s : sample_counts) {
        run_tx_test("sc16_item32_le", 4, swap_bytes, /*swap_iq=*/true, off_in, off_out, num_s);
      }
    }
  }
}

TEST_F(UsrpConvertersTest, TxConverterBE)
{
  constexpr bool swap_bytes = !HOST_IS_BIG_ENDIAN;

  // Test combinations of alignments and sizes
  std::vector<int> offsets = {0, 1, 2, 4, 8, 15, 16, 32};
  std::vector<size_t> sample_counts = {1, 3, 7, 8, 15, 16, 31, 32, 63, 64, 128, 256};

  for (int off_in : offsets) {
    for (int off_out : offsets) {
      for (size_t num_s : sample_counts) {
        run_tx_test("sc16_item32_be", 4, swap_bytes, /*swap_iq=*/false, off_in, off_out, num_s);
      }
    }
  }
}

TEST_F(UsrpConvertersTest, RxConverterChdr_Shift4)
{
  // CHDR carries samples in host-native order, so there is never a byte-swap.
  std::vector<int> offsets = {0, 1, 2, 4, 8, 15, 16, 32};
  std::vector<size_t> sample_counts = {1, 3, 7, 8, 15, 16, 31, 32, 63, 64, 128, 256};

  for (int off_in : offsets) {
    for (int off_out : offsets) {
      for (size_t num_s : sample_counts) {
        run_rx_test("sc16_chdr", 4, /*swap_bytes=*/false, /*swap_iq=*/false, off_in, off_out, num_s);
      }
    }
  }
}

TEST_F(UsrpConvertersTest, TxConverterChdr)
{
  std::vector<int> offsets = {0, 1, 2, 4, 8, 15, 16, 32};
  std::vector<size_t> sample_counts = {1, 3, 7, 8, 15, 16, 31, 32, 63, 64, 128, 256};

  for (int off_in : offsets) {
    for (int off_out : offsets) {
      for (size_t num_s : sample_counts) {
        run_tx_test("sc16_chdr", 4, /*swap_bytes=*/false, /*swap_iq=*/false, off_in, off_out, num_s);
      }
    }
  }
}

TEST_F(UsrpConvertersTest, RxConverterShift2)
{
  // Register with shift 2
  register_oai_converters(2);

  constexpr bool swap_bytes_le = HOST_IS_BIG_ENDIAN;
  constexpr bool swap_bytes_be = !HOST_IS_BIG_ENDIAN;

  std::vector<int> offsets = {0, 1, 2, 8};
  std::vector<size_t> sample_counts = {1, 7, 8, 31, 32, 64};

  for (int off_in : offsets) {
    for (int off_out : offsets) {
      for (size_t num_s : sample_counts) {
        run_rx_test("sc16_item32_le", 2, swap_bytes_le, /*swap_iq=*/true, off_in, off_out, num_s);
        run_rx_test("sc16_item32_be", 2, swap_bytes_be, /*swap_iq=*/false, off_in, off_out, num_s);
      }
    }
  }
}

#include <benchmark/benchmark.h>

static void BM_RxConverterLE_Shift4_Aligned(benchmark::State& state) {
  uhd::convert::id_type rx_id;
  rx_id.input_format = "sc16_item32_le";
  rx_id.num_inputs = 1;
  rx_id.output_format = "sc16_oai";
  rx_id.num_outputs = 1;

  uhd::convert::converter::sptr rx_conv = uhd::convert::get_converter(rx_id)();

  alignas(64) int16_t raw_in[4096];
  alignas(64) int16_t raw_out[4096];

  for (int i = 0; i < 4096; ++i) {
    raw_in[i] = static_cast<int16_t>(i);
  }

  const void* in_vec_ptr = raw_in;
  void* out_vec_ptr = raw_out;
  uhd::ref_vector<const void*> in_vec(in_vec_ptr);
  uhd::ref_vector<void*> out_vec(out_vec_ptr);

  size_t num_samples = state.range(0);

  for (auto _ : state) {
    rx_conv->conv(in_vec, out_vec, num_samples);
    benchmark::ClobberMemory();
  }
  state.SetItemsProcessed(state.iterations() * num_samples);
}
BENCHMARK(BM_RxConverterLE_Shift4_Aligned)->RangeMultiplier(2)->Range(8, 2048);

static void BM_RxConverterLE_Shift4_Unaligned(benchmark::State& state) {
  uhd::convert::id_type rx_id;
  rx_id.input_format = "sc16_item32_le";
  rx_id.num_inputs = 1;
  rx_id.output_format = "sc16_oai";
  rx_id.num_outputs = 1;

  uhd::convert::converter::sptr rx_conv = uhd::convert::get_converter(rx_id)();

  alignas(64) int16_t raw_in[4096];
  alignas(64) int16_t raw_out[4096];

  for (int i = 0; i < 4096; ++i) {
    raw_in[i] = static_cast<int16_t>(i);
  }

  const void* in_vec_ptr = raw_in + 1; // 2-byte aligned
  void* out_vec_ptr = raw_out + 1;
  uhd::ref_vector<const void*> in_vec(in_vec_ptr);
  uhd::ref_vector<void*> out_vec(out_vec_ptr);

  size_t num_samples = state.range(0);

  for (auto _ : state) {
    rx_conv->conv(in_vec, out_vec, num_samples);
    benchmark::ClobberMemory();
  }
  state.SetItemsProcessed(state.iterations() * num_samples);
}
BENCHMARK(BM_RxConverterLE_Shift4_Unaligned)->RangeMultiplier(2)->Range(8, 2048);

static void BM_TxConverterLE_Aligned(benchmark::State& state) {
  uhd::convert::id_type tx_id;
  tx_id.input_format = "sc16_oai";
  tx_id.num_inputs = 1;
  tx_id.output_format = "sc16_item32_le";
  tx_id.num_outputs = 1;

  uhd::convert::converter::sptr tx_conv = uhd::convert::get_converter(tx_id)();

  alignas(64) int16_t raw_in[4096];
  alignas(64) int16_t raw_out[4096];

  for (int i = 0; i < 4096; ++i) {
    raw_in[i] = static_cast<int16_t>(i);
  }

  const void* in_vec_ptr = raw_in;
  void* out_vec_ptr = raw_out;
  uhd::ref_vector<const void*> in_vec(in_vec_ptr);
  uhd::ref_vector<void*> out_vec(out_vec_ptr);

  size_t num_samples = state.range(0);

  for (auto _ : state) {
    tx_conv->conv(in_vec, out_vec, num_samples);
    benchmark::ClobberMemory();
  }
  state.SetItemsProcessed(state.iterations() * num_samples);
}
BENCHMARK(BM_TxConverterLE_Aligned)->RangeMultiplier(2)->Range(8, 2048);

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  int gtest_ret = RUN_ALL_TESTS();

  bool run_benchmarks = false;
  for (int i = 1; i < argc; ++i) {
    if (std::strcmp(argv[i], "--benchmark") == 0 || std::strstr(argv[i], "--benchmark_") != nullptr) {
      run_benchmarks = true;
      break;
    }
  }

  if (run_benchmarks) {
    // Re-register converters with shift 4 for benchmark
    register_oai_converters(4);
    ::benchmark::Initialize(&argc, argv);
    ::benchmark::RunSpecifiedBenchmarks();
  }

  return gtest_ret;
}
