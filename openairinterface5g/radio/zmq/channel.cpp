/* SPDX-License-Identifier: LicenseRef-CSSL-1.0 */
#include "channel.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <fstream>
#include <sstream>
#include <stdexcept>

Channel::Channel()
{
  taps_[0] = {1.0F, 0.0F};
}

std::shared_ptr<const Channel::Trace> Channel::load_trace(const std::string &real_file,
                                                          const std::string &imag_file,
                                                          std::size_t tap_count)
{
  if (tap_count == 0 || tap_count > MAX_TAPS)
    throw std::invalid_argument("trace requires between 1 and 20 taps");
  std::ifstream real(real_file);
  const auto &imag_path = imag_file.empty() ? real_file : imag_file;
  std::ifstream imag(imag_path);
  if (!real.is_open() || !imag.is_open())
    throw std::invalid_argument("cannot open tap trace files: " + real_file + " / " + imag_path);
  auto trace = std::make_shared<Trace>();
  trace->tap_count = tap_count;
  std::string real_line, imag_line;
  while (true) {
    const bool have_real = static_cast<bool>(std::getline(real, real_line));
    const bool have_imag = static_cast<bool>(std::getline(imag, imag_line));
    if (!have_real && !have_imag)
      break;
    // Tiny_Twin resets each row to real=[1,0,...], imag=[0,0,...].
    std::array<std::complex<float>, MAX_TAPS> row{};
    row[0] = {1.0F, 0.0F};
    const auto parse = [&](const std::string &line, bool imaginary) {
      std::istringstream values(line);
      for (std::size_t k = 0; k < tap_count; ++k) {
        values >> std::ws;
        if (values.eof())
          break;
        float value;
        if (!(values >> value) || !std::isfinite(value))
          throw std::invalid_argument("invalid tap trace value at row " + std::to_string(trace->rows.size() + 1));
        if (imaginary)
          row[k].imag(value);
        else
          row[k].real(value);
      }
    };
    if (have_real)
      parse(real_line, false);
    if (have_imag)
      parse(imag_line, true);
    trace->rows.push_back(row);
  }
  if (real.bad() || imag.bad())
    throw std::invalid_argument("failed to read tap trace files");
  return trace;
}

void Channel::set_trace(std::shared_ptr<const Trace> trace)
{
  if (!trace || trace->tap_count == 0 || trace->tap_count > MAX_TAPS)
    throw std::invalid_argument("invalid channel trace");
  trace_ = std::move(trace);
  tap_count_ = trace_->tap_count;
  round_output_ = true;
  reset();
}

void Channel::set_impairments(double path_gain_db, double noise_power_db, uint64_t seed)
{
  const double gain = std::pow(10.0, path_gain_db / 20.0);
  // Match Tiny_Twin literally: this is an amplitude multiplier, despite the parameter name.
  const double noise = 256.0 * std::pow(10.0, noise_power_db / 10.0);
  if (!std::isfinite(path_gain_db) || !std::isfinite(gain) || !std::isfinite(noise)
      || (noise_power_db != -std::numeric_limits<double>::infinity() && !std::isfinite(noise_power_db)))
    throw std::invalid_argument("invalid channel path gain or noise power");
  path_gain_ = gain;
  noise_scale_ = noise;
  noise_seed_ = seed;
  noise_rng_.seed(seed);
  gaussian_.reset();
  round_output_ = trace_ || gain != 1.0 || noise != 0.0;
}

std::vector<std::complex<float>> Channel::parse_taps(const std::string &text)
{
  std::vector<std::complex<float>> taps;
  std::istringstream stream(text);
  std::string entry;
  while (std::getline(stream, entry, ';')) {
    std::istringstream pair(entry);
    float real, imag;
    char comma;
    if (!(pair >> real >> comma >> imag) || comma != ',' || !(pair >> std::ws).eof()
        || !std::isfinite(real) || !std::isfinite(imag))
      throw std::invalid_argument("taps must be finite real,imag pairs separated by semicolons");
    taps.emplace_back(real, imag);
  }
  if (taps.empty() || taps.size() > MAX_TAPS || text.back() == ';')
    throw std::invalid_argument("channel requires between 1 and 20 taps");
  return taps;
}

void Channel::set_taps(const std::vector<std::complex<float>> &taps)
{
  if (taps.empty() || taps.size() > MAX_TAPS)
    throw std::invalid_argument("channel requires between 1 and 20 taps");
  for (const auto &tap : taps) {
    if (!std::isfinite(tap.real()) || !std::isfinite(tap.imag()))
      throw std::invalid_argument("channel taps must be finite");
  }
  trace_.reset();
  round_output_ = path_gain_ != 1.0 || noise_scale_ != 0.0;
  taps_.fill({0.0F, 0.0F});
  std::copy(taps.begin(), taps.end(), taps_.begin());
  tap_count_ = taps.size();
}

void Channel::process(c16_t *samples, std::size_t num_samples, uint64_t timestamp, bool advance_trace)
{
  if (num_samples == 0)
    return;
  if (trace_ && advance_trace) {
    if (trace_row_ < trace_->rows.size()) {
      taps_ = trace_->rows[trace_row_++];
    } else {
      // At EOF Tiny_Twin uses identity, not the last trace row and not a loop.
      taps_.fill({0.0F, 0.0F});
      taps_[0] = {1.0F, 0.0F};
    }
  }
  if (have_timestamp_ && timestamp != next_timestamp_) {
    if (timestamp < next_timestamp_ || timestamp - next_timestamp_ >= HISTORY_SIZE) {
      history_.fill(c16_t{});
    } else {
      const auto gap = static_cast<std::size_t>(timestamp - next_timestamp_);
      std::move(history_.begin() + gap, history_.end(), history_.begin());
      std::fill(history_.end() - gap, history_.end(), c16_t{});
    }
  }
  next_timestamp_ = timestamp + num_samples;
  have_timestamp_ = true;

  // Preserve original samples for in-place convolution and the next block.
  input_.resize(HISTORY_SIZE + num_samples);
  std::copy(history_.begin(), history_.end(), input_.begin());
  std::copy_n(samples, num_samples, input_.begin() + HISTORY_SIZE);
  if (!is_identity()) {
    for (std::size_t n = 0; n < num_samples; ++n) {
      double real = 0.0, imag = 0.0;
      for (std::size_t k = 0; k < tap_count_; ++k) {
        const c16_t x = input_[HISTORY_SIZE + n - k];
        const auto h = taps_[k];
        real += double(x.r) * h.real() - double(x.i) * h.imag();
        imag += double(x.i) * h.real() + double(x.r) * h.imag();
      }
      real *= path_gain_;
      imag *= path_gain_;
      if (noise_scale_ != 0.0) {
        real += noise_scale_ * gaussian_(noise_rng_);
        imag += noise_scale_ * gaussian_(noise_rng_);
      }
      const auto saturate = [this](double value) {
        constexpr double minimum = std::numeric_limits<int16_t>::min();
        constexpr double maximum = std::numeric_limits<int16_t>::max();
        value = std::clamp(value, minimum, maximum);
        return static_cast<int16_t>(round_output_ ? std::round(value) : value);
      };
      samples[n] = {saturate(real), saturate(imag)};
    }
  }
  std::copy(input_.end() - HISTORY_SIZE, input_.end(), history_.begin());
}

void Channel::reset()
{
  history_.fill(c16_t{});
  trace_row_ = 0;
  if (trace_) {
    taps_.fill({0.0F, 0.0F});
    taps_[0] = {1.0F, 0.0F};
  }
  noise_rng_.seed(noise_seed_);
  gaussian_.reset();
  have_timestamp_ = false;
  next_timestamp_ = 0;
}
