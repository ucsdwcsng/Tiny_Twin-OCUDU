// ZMQ sample broker: connects one OCUDU gNB to N OAI UEs over the ZMQ radio drivers.
//
// Both drivers speak the same protocol: the receiver sends a 1-byte request, the transmitter replies with whatever
// complex float32 samples it has queued. There is no header and no timestamp; sample n of a stream is time n, and each
// transmitter zero-fills gaps so the stream stays contiguous. The broker keeps that property:
//
//   DL: gNB TX (REP)  <-REQ- broker -REP->  UE_i RX (REQ)   every UE receives an identical copy of the gNB stream.
//   UL: gNB RX (REQ)  <-REP- broker -REQ->  UE_i TX (REP)   the gNB receives the sample-wise sum of all UE streams.
//
// Nothing is pulled from the gNB until every UE has sent its first RX request, so all UE streams start at the same gNB
// sample and summing the UL streams index by index keeps them aligned.
//
// Profiling: service time is measured from the moment a reply becomes possible (the request is in and the needed samples
// are queued) to the moment it is sent. DL: per UE reply. UL: per gNB reply, i.e. the request is in and every UE has UL
// samples queued. SIGUSR1 starts a measurement window and SIGUSR2 prints a "zmq_broker summary:" line for it.

#include <zmq.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <complex>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <string>
#include <vector>

namespace {

using cf_t = std::complex<float>;
using steady = std::chrono::steady_clock;
constexpr size_t sample_size = sizeof(cf_t);

// Largest reply each receiver accepts: OAI's rx_buffer_size (zmq_radio.cpp) and OCUDU's DEFAULT_STREAM_BUFFER_SIZE
// (radio_session_zmq_impl.cpp). Larger replies are truncated or rejected.
constexpr size_t default_ue_max_reply = 300000;
constexpr size_t default_gnb_max_reply = 614400;
// Stop requesting from a transmitter while this many samples are already queued for its slowest consumer.
constexpr size_t default_high_water = 614400;

volatile std::sig_atomic_t stop_requested = 0;
volatile std::sig_atomic_t window_start_requested = 0;
volatile std::sig_atomic_t window_report_requested = 0;
void on_signal(int) { stop_requested = 1; }
void on_window_start(int) { window_start_requested = 1; }
void on_window_report(int) { window_report_requested = 1; }

// Fixed-size log-linear histogram of durations in nanoseconds: 32 sub-buckets per power of two, so a value is binned
// with under 3.2% error. Recording is O(1) and allocation-free, so it can run inside the forwarding loop.
class latency_histogram
{
public:
  void record(uint64_t ns)
  {
    ++counts_[index(ns)];
    ++count_;
    sum_ns_ += ns;
    max_ns_ = std::max(max_ns_, ns);
  }

  void reset() { *this = latency_histogram(); }

  // Non-empty buckets as "mid_ns:count" pairs separated by spaces, for plotting the distribution.
  void print_buckets(std::FILE *out) const
  {
    for (unsigned i = 0; i < counts_.size(); ++i)
      if (counts_[i] != 0)
        std::fprintf(out, " %.0f:%llu", bucket_mid_ns(i), static_cast<unsigned long long>(counts_[i]));
  }

  uint64_t count() const { return count_; }
  double mean_us() const { return count_ ? double(sum_ns_) / count_ / 1e3 : 0.0; }
  double max_us() const { return max_ns_ / 1e3; }

  // Midpoint of the bucket holding the p-quantile (0 < p <= 1), in microseconds.
  double percentile_us(double p) const
  {
    if (count_ == 0)
      return 0.0;
    uint64_t target = std::max<uint64_t>(1, static_cast<uint64_t>(p * count_ + 0.999999));
    uint64_t seen = 0;
    for (unsigned i = 0; i < counts_.size(); ++i) {
      seen += counts_[i];
      if (seen >= target)
        return std::min(bucket_mid_ns(i), double(max_ns_)) / 1e3;
    }
    return max_us();
  }

private:
  static constexpr unsigned sub_bits = 5;
  static constexpr unsigned nof_buckets = (64 - sub_bits + 1) << sub_bits;

  static unsigned index(uint64_t ns)
  {
    if (ns < (1u << sub_bits))
      return static_cast<unsigned>(ns);
    unsigned msb = 63 - __builtin_clzll(ns);
    unsigned shift = msb - sub_bits;
    return ((shift + 1) << sub_bits) + static_cast<unsigned>((ns >> shift) & ((1u << sub_bits) - 1));
  }

  static double bucket_mid_ns(unsigned i)
  {
    if (i < (1u << sub_bits))
      return i;
    unsigned shift = (i >> sub_bits) - 1;
    double lower = double(((1u << sub_bits) + (i & ((1u << sub_bits) - 1)))) * double(1ull << shift);
    return lower + double(1ull << shift) / 2;
  }

  std::array<uint64_t, nof_buckets> counts_{};
  uint64_t count_ = 0;
  uint64_t sum_ns_ = 0;
  uint64_t max_ns_ = 0;
};

uint64_t elapsed_ns(steady::time_point from, steady::time_point to)
{
  return to > from ? static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(to - from).count()) : 0;
}

// Everything the broker measures over one interval (a stats period or a SIGUSR1..SIGUSR2 window).
struct broker_stats {
  steady::time_point start = steady::now();
  uint64_t gnb_dl_samples = 0; // received from the gNB TX
  uint64_t gnb_ul_samples = 0; // sent to the gNB RX
  uint64_t ue_dl_samples = 0; // sent to all UEs together
  uint64_t ue_ul_samples = 0; // received from all UEs together
  uint64_t poll_ns = 0; // time blocked in zmq_poll (idle)
  latency_histogram dl_service;
  latency_histogram ul_service;

  double seconds(steady::time_point now) const { return std::chrono::duration<double>(now - start).count(); }
  double busy_pct(steady::time_point now) const
  {
    double total = seconds(now);
    return total > 0 ? 100.0 * (1.0 - poll_ns / 1e9 / total) : 0.0;
  }
};

// One-line machine-readable report of a measurement window, followed by the service time histograms.
void print_summary(const broker_stats &st, size_t nof_ues, steady::time_point now)
{
  double sec = st.seconds(now);
  auto msps = [sec](uint64_t samples) { return sec > 0 ? samples / sec / 1e6 : 0.0; };
  std::printf("zmq_broker summary: window_s=%.3f ues=%zu gnb_dl_msps=%.3f gnb_ul_msps=%.3f ue_dl_msps=%.3f "
              "ue_ul_msps=%.3f busy_pct=%.2f gnb_dl_samples=%llu gnb_ul_samples=%llu ue_dl_samples=%llu ue_ul_samples=%llu",
              sec,
              nof_ues,
              msps(st.gnb_dl_samples),
              msps(st.gnb_ul_samples),
              msps(st.ue_dl_samples),
              msps(st.ue_ul_samples),
              st.busy_pct(now),
              static_cast<unsigned long long>(st.gnb_dl_samples),
              static_cast<unsigned long long>(st.gnb_ul_samples),
              static_cast<unsigned long long>(st.ue_dl_samples),
              static_cast<unsigned long long>(st.ue_ul_samples));
  for (auto [name, h] : {std::pair{"dl", &st.dl_service}, std::pair{"ul", &st.ul_service}})
    std::printf(" %s_n=%llu %s_mean_us=%.3f %s_p50_us=%.3f %s_p90_us=%.3f %s_p99_us=%.3f %s_p999_us=%.3f %s_max_us=%.3f",
                name,
                static_cast<unsigned long long>(h->count()),
                name,
                h->mean_us(),
                name,
                h->percentile_us(0.5),
                name,
                h->percentile_us(0.9),
                name,
                h->percentile_us(0.99),
                name,
                h->percentile_us(0.999),
                name,
                h->max_us());
  std::printf("\nzmq_broker hist dl:");
  st.dl_service.print_buckets(stdout);
  std::printf("\nzmq_broker hist ul:");
  st.ul_service.print_buckets(stdout);
  std::printf("\n");
  std::fflush(stdout);
}

[[noreturn]] void fatal(const std::string &what)
{
  std::fprintf(stderr, "zmq_broker: %s: %s\n", what.c_str(), zmq_strerror(zmq_errno()));
  std::exit(1);
}

// FIFO of received sample messages. Messages are kept as zmq_msg_t so DL fan-out and pass-through share the payload
// instead of copying it.
class sample_queue
{
public:
  sample_queue() = default;
  sample_queue(const sample_queue &) = delete;
  sample_queue &operator=(const sample_queue &) = delete;
  ~sample_queue()
  {
    for (auto &s : segments_)
      zmq_msg_close(&s.msg);
  }

  size_t available() const { return available_; }

  // Arrival time of the oldest queued samples. Only valid while available() > 0.
  steady::time_point front_time() const { return segments_.front().arrival; }

  // Takes ownership of msg (which is left empty); arrival is when it was received.
  void push(zmq_msg_t *msg, steady::time_point arrival)
  {
    size_t n = zmq_msg_size(msg) / sample_size;
    if (n == 0) {
      zmq_msg_close(msg);
      zmq_msg_init(msg);
      return;
    }
    segments_.emplace_back();
    segments_.back().arrival = arrival;
    zmq_msg_init(&segments_.back().msg);
    zmq_msg_move(&segments_.back().msg, msg);
    available_ += n;
  }

  // Fills out (uninitialised) with up to max_samples from the front segment. A whole segment that fits is handed over
  // without copying.
  size_t pop_msg(zmq_msg_t *out, size_t max_samples)
  {
    segment &s = segments_.front();
    size_t left = zmq_msg_size(&s.msg) / sample_size - s.offset;
    size_t n = std::min(left, max_samples);
    if (s.offset == 0 && n == left) {
      zmq_msg_init(out);
      zmq_msg_move(out, &s.msg);
    } else {
      zmq_msg_init_size(out, n * sample_size);
      std::memcpy(zmq_msg_data(out), static_cast<const cf_t *>(zmq_msg_data(&s.msg)) + s.offset, n * sample_size);
      s.offset += n;
    }
    consume_front(n, left);
    return n;
  }

  // Consumes n samples, writing them to dst (accumulate = false) or adding them to dst (accumulate = true).
  void read(cf_t *dst, size_t n, bool accumulate)
  {
    while (n > 0) {
      segment &s = segments_.front();
      size_t left = zmq_msg_size(&s.msg) / sample_size - s.offset;
      size_t k = std::min(left, n);
      const cf_t *src = static_cast<const cf_t *>(zmq_msg_data(&s.msg)) + s.offset;
      if (accumulate) {
        // Add as interleaved floats: plain float loops vectorise, std::complex operator+= often does not.
        const float *sf = reinterpret_cast<const float *>(src);
        float *df = reinterpret_cast<float *>(dst);
        for (size_t i = 0; i < 2 * k; ++i)
          df[i] += sf[i];
      } else {
        std::memcpy(dst, src, k * sample_size);
      }
      s.offset += k;
      consume_front(k, left);
      dst += k;
      n -= k;
    }
  }

private:
  struct segment {
    zmq_msg_t msg;
    size_t offset = 0; // samples already consumed
    steady::time_point arrival;
  };

  void consume_front(size_t n, size_t left)
  {
    available_ -= n;
    if (n == left) {
      zmq_msg_close(&segments_.front().msg);
      segments_.pop_front();
    }
  }

  std::deque<segment> segments_;
  size_t available_ = 0;
};

// Receives one message. Returns false on a would-block/interrupted receive.
bool recv_msg(void *sock, zmq_msg_t *msg, const char *who)
{
  zmq_msg_init(msg);
  if (zmq_msg_recv(msg, sock, ZMQ_DONTWAIT) >= 0)
    return true;
  zmq_msg_close(msg);
  if (zmq_errno() == EAGAIN || zmq_errno() == EINTR)
    return false;
  fatal(std::string("receive from ") + who);
}

void send_msg(void *sock, zmq_msg_t *msg, const char *who)
{
  while (zmq_msg_send(msg, sock, 0) < 0) {
    if (zmq_errno() == EINTR && !stop_requested)
      continue;
    zmq_msg_close(msg);
    if (stop_requested)
      return;
    fatal(std::string("send to ") + who);
  }
}

void send_request(void *sock, const char *who)
{
  uint8_t dummy = 0;
  while (zmq_send(sock, &dummy, 1, 0) < 0) {
    if (zmq_errno() == EINTR && !stop_requested)
      continue;
    if (stop_requested)
      return;
    fatal(std::string("request to ") + who);
  }
}

void *make_socket(void *ctx, int type, const std::string &endpoint, bool bind)
{
  void *sock = zmq_socket(ctx, type);
  if (sock == nullptr)
    fatal("zmq_socket");
  int linger = 0;
  zmq_setsockopt(sock, ZMQ_LINGER, &linger, sizeof(linger));
  int rc = bind ? zmq_bind(sock, endpoint.c_str()) : zmq_connect(sock, endpoint.c_str());
  if (rc != 0)
    fatal((bind ? "bind " : "connect ") + endpoint);
  return sock;
}

struct ue_link {
  std::string rx_bind; // UE RX connects here (broker REP)
  std::string tx_endpoint; // UE TX binds here (broker REQ)
  void *rx_sock = nullptr;
  void *tx_sock = nullptr;
  bool rx_request_pending = false; // UE is waiting for DL samples
  steady::time_point rx_request_time; // when the pending DL request arrived
  bool tx_request_outstanding = false; // broker is waiting for UL samples
  bool connected = false; // first RX request seen
  sample_queue dl;
  sample_queue ul;
};

struct options {
  std::string gnb_tx;
  std::string gnb_rx;
  std::vector<std::pair<std::string, std::string>> ues;
  size_t ue_max_reply = default_ue_max_reply;
  size_t gnb_max_reply = default_gnb_max_reply;
  size_t high_water = default_high_water;
  double stats_period = 5.0;
};

[[noreturn]] void usage(const char *argv0, int code)
{
  std::fprintf(code == 0 ? stdout : stderr,
               "Usage: %s --gnb-tx ENDPOINT --gnb-rx ENDPOINT --ue RX_BIND,TX_ENDPOINT [--ue ...]\n"
               "          [--ue-max-reply N] [--gnb-max-reply N] [--high-water N] [--stats-period SEC]\n"
               "\n"
               "  --gnb-tx   gNB TX endpoint to connect to (gNB ru_sdr tx_port), e.g. tcp://10.53.1.3:4556\n"
               "  --gnb-rx   endpoint to bind for the gNB RX (gNB ru_sdr rx_port connects here), e.g. tcp://*:4557\n"
               "  --ue       per UE: endpoint to bind for its RX (UE zmq rx_channels connects here) and its TX endpoint\n"
               "             to connect to (UE zmq tx_channels), e.g. tcp://*:5000,tcp://10.53.1.4:4557\n"
               "  --ue-max-reply   max samples per DL reply to a UE (default %zu)\n"
               "  --gnb-max-reply  max samples per UL reply to the gNB (default %zu)\n"
               "  --high-water     stop requesting from a transmitter while this many samples are queued (default %zu)\n"
               "  --stats-period   seconds between stats lines, 0 disables (default 5)\n",
               argv0,
               default_ue_max_reply,
               default_gnb_max_reply,
               default_high_water);
  std::exit(code);
}

options parse_args(int argc, char **argv)
{
  options opt;
  for (int i = 1; i < argc; ++i) {
    std::string a = argv[i];
    auto value = [&]() -> std::string {
      if (i + 1 >= argc)
        usage(argv[0], 1);
      return argv[++i];
    };
    if (a == "--gnb-tx") {
      opt.gnb_tx = value();
    } else if (a == "--gnb-rx") {
      opt.gnb_rx = value();
    } else if (a == "--ue") {
      std::string v = value();
      auto comma = v.find(',');
      if (comma == std::string::npos)
        usage(argv[0], 1);
      opt.ues.emplace_back(v.substr(0, comma), v.substr(comma + 1));
    } else if (a == "--ue-max-reply") {
      opt.ue_max_reply = std::stoul(value());
    } else if (a == "--gnb-max-reply") {
      opt.gnb_max_reply = std::stoul(value());
    } else if (a == "--high-water") {
      opt.high_water = std::stoul(value());
    } else if (a == "--stats-period") {
      opt.stats_period = std::stod(value());
    } else if (a == "-h" || a == "--help") {
      usage(argv[0], 0);
    } else {
      usage(argv[0], 1);
    }
  }
  if (opt.gnb_tx.empty() || opt.gnb_rx.empty() || opt.ues.empty() || opt.ue_max_reply == 0 || opt.gnb_max_reply == 0)
    usage(argv[0], 1);
  return opt;
}

} // namespace

int main(int argc, char **argv)
{
  options opt = parse_args(argc, argv);
  std::signal(SIGINT, on_signal);
  std::signal(SIGTERM, on_signal);

  void *ctx = zmq_ctx_new();
  void *gnb_tx = make_socket(ctx, ZMQ_REQ, opt.gnb_tx, false);
  void *gnb_rx = make_socket(ctx, ZMQ_REP, opt.gnb_rx, true);
  std::vector<ue_link> ues(opt.ues.size());
  for (size_t i = 0; i < ues.size(); ++i) {
    ues[i].rx_bind = opt.ues[i].first;
    ues[i].tx_endpoint = opt.ues[i].second;
    ues[i].rx_sock = make_socket(ctx, ZMQ_REP, ues[i].rx_bind, true);
    ues[i].tx_sock = make_socket(ctx, ZMQ_REQ, ues[i].tx_endpoint, false);
  }

  std::printf("zmq_broker: gNB tx %s, gNB rx %s, %zu UE(s)\n", opt.gnb_tx.c_str(), opt.gnb_rx.c_str(), ues.size());
  for (size_t i = 0; i < ues.size(); ++i)
    std::printf("zmq_broker: UE %zu rx %s, tx %s\n", i, ues[i].rx_bind.c_str(), ues[i].tx_endpoint.c_str());
  std::fflush(stdout);

  bool started = false;
  bool gnb_tx_outstanding = false;
  bool gnb_rx_request_pending = false;
  steady::time_point gnb_rx_request_time;

  std::signal(SIGUSR1, on_window_start);
  std::signal(SIGUSR2, on_window_report);
  broker_stats period; // reset every --stats-period
  broker_stats window; // reset by SIGUSR1, reported by SIGUSR2
  bool window_active = false;

  std::vector<zmq_pollitem_t> items(2 + 2 * ues.size());
  while (!stop_requested) {
    if (window_start_requested) {
      window_start_requested = 0;
      window = broker_stats();
      window_active = true;
      std::printf("zmq_broker: measurement window started\n");
      std::fflush(stdout);
    }
    if (window_report_requested) {
      window_report_requested = 0;
      if (window_active)
        print_summary(window, ues.size(), steady::now());
      else
        std::printf("zmq_broker: no measurement window (send SIGUSR1 first)\n");
      std::fflush(stdout);
    }

    // Issue requests that are allowed now.
    if (started && !gnb_tx_outstanding) {
      size_t deepest = 0;
      for (auto &ue : ues)
        deepest = std::max(deepest, ue.dl.available());
      if (deepest < opt.high_water) {
        send_request(gnb_tx, "gNB TX");
        gnb_tx_outstanding = true;
      }
    }
    for (auto &ue : ues) {
      if (!ue.tx_request_outstanding && ue.ul.available() < opt.high_water) {
        send_request(ue.tx_sock, "UE TX");
        ue.tx_request_outstanding = true;
      }
    }

    // Answer requests that can be served now. Service time runs from the moment the reply became possible (request in
    // and samples queued) to the moment it is sent.
    for (auto &ue : ues) {
      if (ue.rx_request_pending && ue.dl.available() > 0) {
        steady::time_point ready = std::max(ue.rx_request_time, ue.dl.front_time());
        zmq_msg_t msg;
        size_t n = ue.dl.pop_msg(&msg, opt.ue_max_reply);
        send_msg(ue.rx_sock, &msg, "UE RX");
        uint64_t service = elapsed_ns(ready, steady::now());
        ue.rx_request_pending = false;
        for (broker_stats *st : {&period, &window}) {
          st->ue_dl_samples += n;
          st->dl_service.record(service);
        }
      }
    }
    if (gnb_rx_request_pending) {
      size_t n = opt.gnb_max_reply;
      for (auto &ue : ues)
        n = std::min(n, ue.ul.available());
      if (n > 0) {
        steady::time_point ready = gnb_rx_request_time;
        for (auto &ue : ues)
          ready = std::max(ready, ue.ul.front_time());
        zmq_msg_t msg;
        if (ues.size() == 1) {
          n = ues[0].ul.pop_msg(&msg, n);
        } else {
          zmq_msg_init_size(&msg, n * sample_size);
          cf_t *dst = static_cast<cf_t *>(zmq_msg_data(&msg));
          for (size_t i = 0; i < ues.size(); ++i)
            ues[i].ul.read(dst, n, i != 0);
        }
        send_msg(gnb_rx, &msg, "gNB RX");
        uint64_t service = elapsed_ns(ready, steady::now());
        gnb_rx_request_pending = false;
        for (broker_stats *st : {&period, &window}) {
          st->gnb_ul_samples += n;
          st->ul_service.record(service);
        }
      }
    }

    // Poll only the sockets we expect something on.
    size_t nitems = 0;
    auto watch = [&](void *sock, bool want) { items[nitems++] = {sock, 0, static_cast<short>(want ? ZMQ_POLLIN : 0), 0}; };
    watch(gnb_tx, gnb_tx_outstanding);
    watch(gnb_rx, !gnb_rx_request_pending);
    for (auto &ue : ues) {
      watch(ue.rx_sock, !ue.rx_request_pending);
      watch(ue.tx_sock, ue.tx_request_outstanding);
    }
    long timeout_ms = opt.stats_period > 0 ? 200 : -1;
    steady::time_point poll_start = steady::now();
    int rc = zmq_poll(items.data(), static_cast<int>(nitems), timeout_ms);
    uint64_t polled = elapsed_ns(poll_start, steady::now());
    period.poll_ns += polled;
    window.poll_ns += polled;
    if (rc < 0) {
      if (zmq_errno() == EINTR)
        continue;
      fatal("zmq_poll");
    }

    size_t k = 0;
    zmq_msg_t msg;
    if ((items[k++].revents & ZMQ_POLLIN) && recv_msg(gnb_tx, &msg, "gNB TX")) {
      steady::time_point arrival = steady::now();
      gnb_tx_outstanding = false;
      size_t n = zmq_msg_size(&msg) / sample_size;
      if (zmq_msg_size(&msg) % sample_size != 0)
        std::fprintf(stderr, "zmq_broker: gNB TX sent %zu bytes, not a whole number of samples\n", zmq_msg_size(&msg));
      period.gnb_dl_samples += n;
      window.gnb_dl_samples += n;
      // Every UE but the last gets a refcounted copy; the last takes the original.
      for (size_t i = 0; i + 1 < ues.size(); ++i) {
        zmq_msg_t copy;
        zmq_msg_init(&copy);
        zmq_msg_copy(&copy, &msg);
        ues[i].dl.push(&copy, arrival);
        zmq_msg_close(&copy);
      }
      ues.back().dl.push(&msg, arrival);
      zmq_msg_close(&msg);
    }
    if ((items[k++].revents & ZMQ_POLLIN) && recv_msg(gnb_rx, &msg, "gNB RX")) {
      zmq_msg_close(&msg);
      gnb_rx_request_pending = true;
      gnb_rx_request_time = steady::now();
    }
    for (size_t i = 0; i < ues.size(); ++i) {
      auto &ue = ues[i];
      if ((items[k++].revents & ZMQ_POLLIN) && recv_msg(ue.rx_sock, &msg, "UE RX")) {
        zmq_msg_close(&msg);
        ue.rx_request_pending = true;
        ue.rx_request_time = steady::now();
        if (!ue.connected) {
          ue.connected = true;
          std::printf("zmq_broker: UE %zu connected\n", i);
          std::fflush(stdout);
        }
      }
      if ((items[k++].revents & ZMQ_POLLIN) && recv_msg(ue.tx_sock, &msg, "UE TX")) {
        steady::time_point arrival = steady::now();
        ue.tx_request_outstanding = false;
        if (zmq_msg_size(&msg) % sample_size != 0)
          std::fprintf(stderr, "zmq_broker: UE %zu TX sent %zu bytes, not a whole number of samples\n", i,
                       zmq_msg_size(&msg));
        period.ue_ul_samples += zmq_msg_size(&msg) / sample_size;
        window.ue_ul_samples += zmq_msg_size(&msg) / sample_size;
        ue.ul.push(&msg, arrival);
        zmq_msg_close(&msg);
      }
    }

    if (!started && std::all_of(ues.begin(), ues.end(), [](const ue_link &ue) { return ue.connected; })) {
      started = true;
      std::printf("zmq_broker: all %zu UE(s) connected, forwarding\n", ues.size());
      std::fflush(stdout);
    }

    if (opt.stats_period > 0) {
      steady::time_point now = steady::now();
      double sec = period.seconds(now);
      if (sec >= opt.stats_period) {
        std::printf("zmq_broker: DL %.2f Msps, UL %.2f Msps, service p50/p99/max us DL %.1f/%.1f/%.1f UL %.1f/%.1f/%.1f, "
                    "busy %.1f%%, queues DL/UL",
                    period.gnb_dl_samples / sec / 1e6,
                    period.gnb_ul_samples / sec / 1e6,
                    period.dl_service.percentile_us(0.5),
                    period.dl_service.percentile_us(0.99),
                    period.dl_service.max_us(),
                    period.ul_service.percentile_us(0.5),
                    period.ul_service.percentile_us(0.99),
                    period.ul_service.max_us(),
                    period.busy_pct(now));
        for (auto &ue : ues)
          std::printf(" %zu/%zu", ue.dl.available(), ue.ul.available());
        std::printf("\n");
        std::fflush(stdout);
        period = broker_stats();
      }
    }
  }

  std::printf("zmq_broker: stopping\n");
  zmq_close(gnb_tx);
  zmq_close(gnb_rx);
  for (auto &ue : ues) {
    zmq_close(ue.rx_sock);
    zmq_close(ue.tx_sock);
  }
  zmq_ctx_term(ctx);
  return 0;
}
