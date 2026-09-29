/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include <uhd/convert.hpp>
#include <string>
#include <utility>
#include <simde/x86/avx512.h>
#include "usrp_converters.hpp"

#if defined(__ARM_NEON)
#include <arm_neon.h>
#endif

#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
constexpr bool HOST_IS_BIG_ENDIAN = true;
#else
constexpr bool HOST_IS_BIG_ENDIAN = false;
#endif

const std::string CPU_FORMAT_OAI = "sc16_oai";

namespace uhd {
namespace convert {
converter::~converter(void)
{
  // NOP
}
} // namespace convert
} // namespace uhd

// item32 packs (real << 16) | imag as one 32-bit word. On a little-endian host
// with the LE wire format ("_item32_le", used by B2xx), the byte-order
// conversion is a no-op, so reading the raw bytes back as two int16 lanes
// yields (imag, real) - swapped. BE wire format restores (real, imag) via the
// per-lane byte swap alone. CHDR has no item32 packing, so neither is needed.
template <bool SwapIQ>
UHD_FORCE_INLINE void swap_iq_pairs(int16_t* dest, const size_t total_ints)
{
  if (!SwapIQ) {
    return;
  }
  for (size_t j = 0; j + 1 < total_ints; j += 2) {
    std::swap(dest[j], dest[j + 1]);
  }
}

template <int Shift, bool SwapBytes, bool SwapIQ>
class sc16_oai_rx_converter : public uhd::convert::converter {
 public:
  virtual ~sc16_oai_rx_converter(void) override
  {
  }

  void set_scalar(const double) override
  {
    // No-op
  }

  void operator()(const input_type& in, const output_type& out, const size_t num) override
  {
    for (size_t chan = 0; chan < in.size(); ++chan) {
      const int16_t* src = reinterpret_cast<const int16_t*>(in[chan]);
      int16_t* dest = reinterpret_cast<int16_t*>(out[chan]);

      size_t total_ints = num * 2;
      size_t j = 0;

#if defined(__ARM_NEON)
      // Process 32 elements (16 samples) at a time
      for (; j + 31 < total_ints; j += 32) {
        int16x8_t v0 = vld1q_s16(&src[j + 0]);
        int16x8_t v1 = vld1q_s16(&src[j + 8]);
        int16x8_t v2 = vld1q_s16(&src[j + 16]);
        int16x8_t v3 = vld1q_s16(&src[j + 24]);

        if (SwapBytes) {
          v0 = vreinterpretq_s16_u8(vrev16q_u8(vreinterpretq_u8_s16(v0)));
          v1 = vreinterpretq_s16_u8(vrev16q_u8(vreinterpretq_u8_s16(v1)));
          v2 = vreinterpretq_s16_u8(vrev16q_u8(vreinterpretq_u8_s16(v2)));
          v3 = vreinterpretq_s16_u8(vrev16q_u8(vreinterpretq_u8_s16(v3)));
        }

        v0 = vshrq_n_s16(v0, Shift);
        v1 = vshrq_n_s16(v1, Shift);
        v2 = vshrq_n_s16(v2, Shift);
        v3 = vshrq_n_s16(v3, Shift);

        vst1q_s16(&dest[j + 0], v0);
        vst1q_s16(&dest[j + 8], v1);
        vst1q_s16(&dest[j + 16], v2);
        vst1q_s16(&dest[j + 24], v3);
      }

      // Process remaining elements (8 at a time)
      for (; j + 7 < total_ints; j += 8) {
        int16x8_t v = vld1q_s16(&src[j]);
        if (SwapBytes) {
          v = vreinterpretq_s16_u8(vrev16q_u8(vreinterpretq_u8_s16(v)));
        }
        v = vshrq_n_s16(v, Shift);
        vst1q_s16(&dest[j], v);
      }
#elif defined(__AVX512F__) && defined(__AVX512BW__)
      simde__m512i mask_ff = simde_mm512_set1_epi16(0x00FF);

      for (; j + 31 < total_ints; j += 32) {
        simde__m512i v_in = simde_mm512_loadu_si512(reinterpret_cast<const simde__m512i*>(&src[j]));
        if (SwapBytes) {
          simde__m512i low = simde_mm512_and_si512(simde_mm512_srli_epi16(v_in, 8), mask_ff);
          simde__m512i high = simde_mm512_slli_epi16(v_in, 8);
          v_in = simde_mm512_or_si512(low, high);
        }
        simde__m512i v_out = simde_mm512_srai_epi16(v_in, Shift);
        simde_mm512_storeu_si512(reinterpret_cast<simde__m512i*>(&dest[j]), v_out);
      }
#elif defined(__AVX2__)
      simde__m256i mask_ff = simde_mm256_set1_epi16(0x00FF);

      for (; j + 15 < total_ints; j += 16) {
        simde__m256i v_in = simde_mm256_loadu_si256(reinterpret_cast<const simde__m256i*>(&src[j]));
        if (SwapBytes) {
          simde__m256i low = simde_mm256_and_si256(simde_mm256_srli_epi16(v_in, 8), mask_ff);
          simde__m256i high = simde_mm256_slli_epi16(v_in, 8);
          v_in = simde_mm256_or_si256(low, high);
        }
        simde__m256i v_out = simde_mm256_srai_epi16(v_in, Shift);
        simde_mm256_storeu_si256(reinterpret_cast<simde__m256i*>(&dest[j]), v_out);
      }
#elif defined(__SSE2__)
      simde__m128i mask_ff = simde_mm_set1_epi16(0x00FF);

      for (; j + 7 < total_ints; j += 8) {
        simde__m128i v_in = simde_mm_loadu_si128(reinterpret_cast<const simde__m128i*>(&src[j]));
        if (SwapBytes) {
          simde__m128i low = simde_mm_and_si128(simde_mm_srli_epi16(v_in, 8), mask_ff);
          simde__m128i high = simde_mm_slli_epi16(v_in, 8);
          v_in = simde_mm_or_si128(low, high);
        }
        simde__m128i v_out = simde_mm_srai_epi16(v_in, Shift);
        simde_mm_storeu_si128(reinterpret_cast<simde__m128i*>(&dest[j]), v_out);
      }
#endif
      for (; j < total_ints; ++j) {
        int16_t val = src[j];
        if (SwapBytes) {
          val = (val << 8) | ((val >> 8) & 0x00FF);
        }
        dest[j] = val >> Shift;
      }

      swap_iq_pairs<SwapIQ>(dest, total_ints);
    }
  }
};

template <int Shift, bool SwapBytes, bool SwapIQ>
class sc16_oai_tx_converter : public uhd::convert::converter {
 public:
  virtual ~sc16_oai_tx_converter(void) override
  {
  }

  void set_scalar(const double) override
  {
    // No-op
  }

  void operator()(const input_type& in, const output_type& out, const size_t num) override
  {
    for (size_t chan = 0; chan < in.size(); ++chan) {
      const int16_t* src = reinterpret_cast<const int16_t*>(in[chan]);
      int16_t* dest = reinterpret_cast<int16_t*>(out[chan]);

      size_t total_ints = num * 2;
      size_t j = 0;

#if defined(__ARM_NEON)
      // Process 32 elements (16 samples) at a time
      for (; j + 31 < total_ints; j += 32) {
        int16x8_t v0 = vld1q_s16(&src[j + 0]);
        int16x8_t v1 = vld1q_s16(&src[j + 8]);
        int16x8_t v2 = vld1q_s16(&src[j + 16]);
        int16x8_t v3 = vld1q_s16(&src[j + 24]);

        v0 = vshlq_n_s16(v0, Shift);
        v1 = vshlq_n_s16(v1, Shift);
        v2 = vshlq_n_s16(v2, Shift);
        v3 = vshlq_n_s16(v3, Shift);

        if (SwapBytes) {
          v0 = vreinterpretq_s16_u8(vrev16q_u8(vreinterpretq_u8_s16(v0)));
          v1 = vreinterpretq_s16_u8(vrev16q_u8(vreinterpretq_u8_s16(v1)));
          v2 = vreinterpretq_s16_u8(vrev16q_u8(vreinterpretq_u8_s16(v2)));
          v3 = vreinterpretq_s16_u8(vrev16q_u8(vreinterpretq_u8_s16(v3)));
        }

        vst1q_s16(&dest[j + 0], v0);
        vst1q_s16(&dest[j + 8], v1);
        vst1q_s16(&dest[j + 16], v2);
        vst1q_s16(&dest[j + 24], v3);
      }

      // Process remaining elements (8 at a time)
      for (; j + 7 < total_ints; j += 8) {
        int16x8_t v = vld1q_s16(&src[j]);
        v = vshlq_n_s16(v, Shift);
        if (SwapBytes) {
          v = vreinterpretq_s16_u8(vrev16q_u8(vreinterpretq_u8_s16(v)));
        }
        vst1q_s16(&dest[j], v);
      }
#elif defined(__AVX512F__) && defined(__AVX512BW__)
      simde__m512i mask_ff = simde_mm512_set1_epi16(0x00FF);

      for (; j + 31 < total_ints; j += 32) {
        simde__m512i v_in = simde_mm512_loadu_si512(reinterpret_cast<const simde__m512i*>(&src[j]));
        simde__m512i v_out = simde_mm512_slli_epi16(v_in, Shift);
        if (SwapBytes) {
          simde__m512i low = simde_mm512_and_si512(simde_mm512_srli_epi16(v_out, 8), mask_ff);
          simde__m512i high = simde_mm512_slli_epi16(v_out, 8);
          v_out = simde_mm512_or_si512(low, high);
        }
        simde_mm512_storeu_si512(reinterpret_cast<simde__m512i*>(&dest[j]), v_out);
      }
#elif defined(__AVX2__)
      simde__m256i mask_ff = simde_mm256_set1_epi16(0x00FF);

      for (; j + 15 < total_ints; j += 16) {
        simde__m256i v_in = simde_mm256_loadu_si256(reinterpret_cast<const simde__m256i*>(&src[j]));
        simde__m256i v_out = simde_mm256_slli_epi16(v_in, Shift);
        if (SwapBytes) {
          simde__m256i low = simde_mm256_and_si256(simde_mm256_srli_epi16(v_out, 8), mask_ff);
          simde__m256i high = simde_mm256_slli_epi16(v_out, 8);
          v_out = simde_mm256_or_si256(low, high);
        }
        simde_mm256_storeu_si256(reinterpret_cast<simde__m256i*>(&dest[j]), v_out);
      }
#elif defined(__SSE2__)
      simde__m128i mask_ff = simde_mm_set1_epi16(0x00FF);

      for (; j + 7 < total_ints; j += 8) {
        simde__m128i v_in = simde_mm_loadu_si128(reinterpret_cast<const simde__m128i*>(&src[j]));
        simde__m128i v_out = simde_mm_slli_epi16(v_in, Shift);
        if (SwapBytes) {
          simde__m128i low = simde_mm_and_si128(simde_mm_srli_epi16(v_out, 8), mask_ff);
          simde__m128i high = simde_mm_slli_epi16(v_out, 8);
          v_out = simde_mm_or_si128(low, high);
        }
        simde_mm_storeu_si128(reinterpret_cast<simde__m128i*>(&dest[j]), v_out);
      }
#endif
      for (; j < total_ints; ++j) {
        int16_t val = src[j] << Shift;
        if (SwapBytes) {
          val = (val << 8) | ((val >> 8) & 0x00FF);
        }
        dest[j] = val;
      }

      swap_iq_pairs<SwapIQ>(dest, total_ints);
    }
  }
};

void register_oai_converters(int rxshift)
{
  uhd::convert::register_bytes_per_item(CPU_FORMAT_OAI, sizeof(int16_t) * 2);

  constexpr bool swap_le = (false != HOST_IS_BIG_ENDIAN);
  constexpr bool swap_be = (true != HOST_IS_BIG_ENDIAN);

  for (size_t num_chans = 1; num_chans <= 4; ++num_chans) {
    // LE wire format (used by B2xx, regardless of host endianness): needs the I/Q pair swap.
    {
      uhd::convert::id_type rx_id;
      rx_id.input_format = "sc16_item32_le";
      rx_id.num_inputs = num_chans;
      rx_id.output_format = CPU_FORMAT_OAI;
      rx_id.num_outputs = num_chans;

      switch (rxshift) {
        case 2:
          uhd::convert::register_converter(
              rx_id,
              []() { return uhd::convert::converter::sptr(new sc16_oai_rx_converter<2, swap_le, true>()); },
              100);
          break;
        case 4:
          uhd::convert::register_converter(
              rx_id,
              []() { return uhd::convert::converter::sptr(new sc16_oai_rx_converter<4, swap_le, true>()); },
              100);
          break;
        default:
          break;
      }

      uhd::convert::id_type tx_id;
      tx_id.input_format = CPU_FORMAT_OAI;
      tx_id.num_inputs = num_chans;
      tx_id.output_format = rx_id.input_format;
      tx_id.num_outputs = num_chans;

      uhd::convert::register_converter(
          tx_id,
          []() { return uhd::convert::converter::sptr(new sc16_oai_tx_converter<4, swap_le, true>()); },
          100);
    }

    // BE wire format: (real, imag) order is already correct, no I/Q pair swap needed.
    {
      uhd::convert::id_type rx_id;
      rx_id.input_format = "sc16_item32_be";
      rx_id.num_inputs = num_chans;
      rx_id.output_format = CPU_FORMAT_OAI;
      rx_id.num_outputs = num_chans;

      switch (rxshift) {
        case 2:
          uhd::convert::register_converter(
              rx_id,
              []() { return uhd::convert::converter::sptr(new sc16_oai_rx_converter<2, swap_be, false>()); },
              100);
          break;
        case 4:
          uhd::convert::register_converter(
              rx_id,
              []() { return uhd::convert::converter::sptr(new sc16_oai_rx_converter<4, swap_be, false>()); },
              100);
          break;
        default:
          break;
      }

      uhd::convert::id_type tx_id;
      tx_id.input_format = CPU_FORMAT_OAI;
      tx_id.num_inputs = num_chans;
      tx_id.output_format = rx_id.input_format;
      tx_id.num_outputs = num_chans;

      uhd::convert::register_converter(
          tx_id,
          []() { return uhd::convert::converter::sptr(new sc16_oai_tx_converter<4, swap_be, false>()); },
          100);
    }

    // CHDR wire format (RFNoC devices: X3xx, N3xx, X4xx): no item32 packing, no swaps needed.
    {
      uhd::convert::id_type rx_id;
      rx_id.input_format = "sc16_chdr";
      rx_id.num_inputs = num_chans;
      rx_id.output_format = CPU_FORMAT_OAI;
      rx_id.num_outputs = num_chans;

      switch (rxshift) {
        case 2:
          uhd::convert::register_converter(
              rx_id,
              []() { return uhd::convert::converter::sptr(new sc16_oai_rx_converter<2, false, false>()); },
              100);
          break;
        case 4:
          uhd::convert::register_converter(
              rx_id,
              []() { return uhd::convert::converter::sptr(new sc16_oai_rx_converter<4, false, false>()); },
              100);
          break;
        default:
          break;
      }

      uhd::convert::id_type tx_id;
      tx_id.input_format = CPU_FORMAT_OAI;
      tx_id.num_inputs = num_chans;
      tx_id.output_format = rx_id.input_format;
      tx_id.num_outputs = num_chans;

      uhd::convert::register_converter(
          tx_id,
          []() { return uhd::convert::converter::sptr(new sc16_oai_tx_converter<4, false, false>()); },
          100);
    }
  }
}
