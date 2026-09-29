/* SPDX-License-Identifier: LicenseRef-CSSL-1.0 */
#ifndef OAI_ZMQ_CHANNEL_H
#define OAI_ZMQ_CHANNEL_H

#include "common/platform_types.h"
#include <array>
#include <complex>
#include <cstddef>
#include <cstdint>
#include <string>
#include <memory>
#include <random>
#include <limits>
#include <vector>

class Channel {
 public:
  static constexpr std::size_t MAX_TAPS = 20;
  static constexpr std::size_t HISTORY_SIZE = MAX_TAPS - 1;

  struct Trace {
    std::size_t tap_count;
    std::vector<std::array<std::complex<float>, MAX_TAPS>> rows;
  };

  Channel();
  static std::shared_ptr<const Trace> load_trace(const std::string &real_file,
                                                const std::string &imag_file,
                                                std::size_t tap_count);
  void set_trace(std::shared_ptr<const Trace> trace);
  void set_impairments(double path_gain_db, double noise_power_db, uint64_t seed = 1);
  // Coefficients are ordered by increasing delay, in samples.
  static std::vector<std::complex<float>> parse_taps(const std::string &text);
  void set_taps(const std::vector<std::complex<float>> &taps);
  // Alignment silence advances sample history but does not consume a trace row.
  void process(c16_t *samples, std::size_t num_samples, uint64_t timestamp, bool advance_trace = true);
  void reset();
  std::size_t tap_count() const { return tap_count_; }
  bool is_identity() const
  {
    return !trace_ && path_gain_ == 1.0 && noise_scale_ == 0.0
           && tap_count_ == 1 && taps_[0] == std::complex<float>(1.0F, 0.0F);
  }

 private:
  std::shared_ptr<const Trace> trace_;
  std::size_t trace_row_ = 0;
  double path_gain_ = 1.0;
  double noise_scale_ = 0.0;
  bool round_output_ = false;
  uint64_t noise_seed_ = 1;
  std::mt19937_64 noise_rng_{noise_seed_};
  std::normal_distribution<double> gaussian_{0.0, 1.0};
  std::array<std::complex<float>, MAX_TAPS> taps_{};
  std::size_t tap_count_ = 1;
  std::array<c16_t, HISTORY_SIZE> history_{};
  std::vector<c16_t> input_;
  uint64_t next_timestamp_ = 0;
  bool have_timestamp_ = false;
};

#endif
