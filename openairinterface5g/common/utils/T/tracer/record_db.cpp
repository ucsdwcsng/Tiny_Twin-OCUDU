/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include <clickhouse/client.h>
#include <memory>
#include <string>
#include <iostream>
#include <chrono>
#include <cstdint>
#include <ctime>
#include <cmath>
#include <unordered_map>
#include "fmtlog.h"

namespace ch = clickhouse;

extern "C" { // For connect_to(), socket_send(), parse_database(), etc.
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include "database.h"
#include "utils.h"
#include "../T_defs.h"
#include "configuration.h"
}

const int MAX_QUERY_SIZE = 65536;
const char *DEFAULT_CLICKHOUSE_HOST = "localhost";
const int DEFAULT_CLICKHOUSE_PORT = 9000;
const char *DEFAULT_CLICKHOUSE_DB = "oai";

// Buffer multiple rows per event type and insert when:
// 1. Buffer reaches BATCH_SIZE rows, OR
// 2. INSERT_INTERVAL_MS milliseconds have elapsed since last insert
// This reduces insertion time from ~30ms/event to <1ms/event amortized
const size_t BATCH_SIZE = 500; // Number of rows to buffer before inserting
const uint64_t INSERT_INTERVAL_MS = 500; // Insert every 500ms
const size_t BUFFER_SIZE = 10 * 1024 * 1024; // 10MB
const size_t BUFFER_MARGIN = 183'456; // UE_PHY_PDSCH_IQ size

// 5G NR timing constants
const uint64_t NS_PER_SEC = 1000000000ULL;
const uint64_t FRAME_PERIOD_NS = 10000000ULL; // 10ms per frame
const int SFN_MAX = 1024;
// Duration of one full SFN cycle (1024 frames = 10.24 seconds)
const uint64_t SFN_CYCLE_NS = FRAME_PERIOD_NS * SFN_MAX;

// TAI to UTC offset in seconds (37 leap seconds as of 2017)
// This may need to be updated when new leap seconds are added
const int TAI_UTC_OFFSET_SEC = 37;

// GPS epoch is  January 6, 1980 00:00:00 UTC
// Unix epoch is January 1, 1970 00:00:00 UTC
// Offset = 315964800 seconds
const uint64_t UNIX_TO_GPS_OFFSET_SEC = 315964800ULL;
const uint64_t UNIX_TO_GPS_OFFSET_NS = UNIX_TO_GPS_OFFSET_SEC * NS_PER_SEC;
const uint8_t UTC_TO_GPS_OFFSET_SEC = 18; // As of 2017, GPS time is 18 seconds ahead of UTC time.
const uint64_t UTC_TO_GPS_OFFSET_NS = UTC_TO_GPS_OFFSET_SEC * NS_PER_SEC;

/**
 * Calculate duration between two timestamps in nanoseconds
 * Works with both std::chrono::high_resolution_clock::time_point and uint64_t nanosecond timestamps
 */
#define DURATION_NS(start, end) (std::chrono::duration_cast<std::chrono::nanoseconds>((end) - (start)).count())
#define DURATION_US(start, end) (std::chrono::duration_cast<std::chrono::microseconds>((end) - (start)).count())
#define DURATION_MS(start, end) (std::chrono::duration_cast<std::chrono::milliseconds>((end) - (start)).count())

/**
 * Calculate duration between two uint64_t nanosecond timestamps
 */
#define DURATION_NS_U64(end_ns, start_ns) ((int64_t)(end_ns) - (int64_t)(start_ns))
/**
 * Get slot duration in nanoseconds for a given numerology
 * mu=0 (15kHz): 1ms, mu=1 (30kHz): 0.5ms, mu=2 (60kHz): 0.25ms, etc.
 */
inline uint64_t get_slot_duration_ns(int mu)
{
  static const uint64_t slot_durations_ns[] = {
      1000000, // mu=0: 1ms
      500000, // mu=1: 0.5ms
      250000, // mu=2: 0.25ms
      125000, // mu=3: 0.125ms
      62500 // mu=4: 0.0625ms
  };
  if (mu < 0 || mu > 4) {
    loge("Invalid numerology mu={}, using mu=0", mu);
    return slot_durations_ns[0];
  }
  return slot_durations_ns[mu];
}

/**
 * Get number of slots per frame for a given numerology
 */
inline int get_slots_per_frame(int mu)
{
  static const int slots_per_frame[] = {10, 20, 40, 80, 160};
  if (mu < 0 || mu > 4) {
    loge("Invalid numerology mu={}, using mu=0", mu);
    return slots_per_frame[0];
  }
  return slots_per_frame[mu];
}

/**
 * Convert Unix/Linux time to GPS time
 *
 * @param unix_time_ns Unix time in nanoseconds since Unix epoch (Jan 1, 1970)
 * @return GPS time in nanoseconds since GPS epoch (Jan 6, 1980)
 */
inline uint64_t unix_to_gps_ns(uint64_t unix_time_ns)
{
  return unix_time_ns - UNIX_TO_GPS_OFFSET_NS + UTC_TO_GPS_OFFSET_NS;
}

/**
 * Convert GPS time to Unix/Linux time
 *
 * @param gps_time_ns GPS time in nanoseconds since GPS epoch
 * @return GPS time in nanoseconds since Unix epoch
 */
inline uint64_t gps_to_posix_ns(uint64_t gps_time_ns)
{
  return gps_time_ns + UNIX_TO_GPS_OFFSET_NS;
}

/**
 * Convert GPS time to Unix/Linux time
 *
 * @param gps_time_ns GPS time in nanoseconds since GPS epoch
 * @return UTC time in nanoseconds since Unix epoch
 */
inline uint64_t gps_to_utc_ns(uint64_t gps_time_ns)
{
  return gps_to_posix_ns(gps_time_ns) - UTC_TO_GPS_OFFSET_NS;
}

/**
 * Calculate the absolute GPS timestamp for a given SFN and slot, given a close time reference in Unix time.
 *
 * @param sfn System Frame Number (0-1023)
 * @param slot Slot number within the frame (depends on numerology)
 * @param mu Numerology (0-4): 0=15kHz, 1=30kHz, 2=60kHz, 3=120kHz, 4=240kHz
 * @param reference_utc_ns Reference UTC time in nanoseconds since Unix epoch
 * @return UTC timestamp in nanoseconds when the SFN/slot was transmitted
 */
uint64_t calculate_sfn_slot_gps_time(int sfn, int slot, int mu, uint64_t reference_unix_ns)
{
  // Validate inputs
  if (sfn < 0 || sfn >= SFN_MAX) {
    loge("Invalid SFN={} (must be 0-1023)", sfn);
    sfn = sfn % SFN_MAX;
    return 0;
  }

  int slots_per_frame = get_slots_per_frame(mu);
  if (slot < 0 || slot >= slots_per_frame) {
    loge("Invalid slot={} for mu={} (must be 0-{})", slot, mu, slots_per_frame - 1);
    slot = slot % slots_per_frame;
  }
  // Nanoseconds since GPS epoch start, should be slightly larger than actual transmit time.
  uint64_t reference_gps_ns = unix_to_gps_ns(reference_unix_ns);

  // Calculate the offset within one SFN cycle for the given SFN/slot
  uint64_t slot_duration_ns = get_slot_duration_ns(mu);
  uint64_t offset_in_cycle_ns = (sfn * FRAME_PERIOD_NS) + (slot * slot_duration_ns);

  // Find the number of complete SFN cycles since GPS epoch
  // We'll use the reference time to determine which SFN cycle we're in
  uint64_t cycles_since_epoch = reference_gps_ns / SFN_CYCLE_NS;

  // Calculate the base time for the current SFN cycle
  uint64_t current_cycle_base_ns = cycles_since_epoch * SFN_CYCLE_NS;

  // Calculate the target time within this cycle
  uint64_t gps_time_ns = current_cycle_base_ns + offset_in_cycle_ns;

  // Check if the GPS time is too far in the future (more than 1 second)
  // If so, it probably occurred in the previous cycle
  if (gps_time_ns > reference_gps_ns + NS_PER_SEC) {
    gps_time_ns -= SFN_CYCLE_NS;
  }
  // Check if the target is too far in the past (more than 9.24 seconds)
  // If so, it probably occurs in the next cycle
  if (gps_time_ns < reference_gps_ns - (SFN_CYCLE_NS - NS_PER_SEC)) {
    gps_time_ns += SFN_CYCLE_NS;
  }
  return gps_time_ns;
}

void calculate_sfn_slot_from_gps(uint64_t gps_time_ns, int mu, int *sfn, int *slot)
{
  uint64_t phase_ns = gps_time_ns % SFN_CYCLE_NS;

  *sfn = phase_ns / FRAME_PERIOD_NS; // Truncate to integer
  uint64_t position_in_frame_ns = phase_ns % FRAME_PERIOD_NS; // Remainder

  uint64_t slot_duration_ns = get_slot_duration_ns(mu);
  *slot = position_in_frame_ns / slot_duration_ns;

  if (*sfn >= SFN_MAX)
    *sfn = SFN_MAX - 1;
  int max_slot = get_slots_per_frame(mu) - 1;
  if (*slot > max_slot)
    *slot = max_slot;
}

void calculate_sfn_slot_from_unix(uint64_t unix_time_ns, int mu, int *sfn, int *slot)
{
  uint64_t gps_time_ns = unix_to_gps_ns(unix_time_ns);
  calculate_sfn_slot_from_gps(gps_time_ns, mu, sfn, slot);
}

void usage(void)
{
  printf(
      "options:\n"
      "    -d <database file>        this option is mandatory\n"
      "    -on <GROUP or ID>         turn log ON for given GROUP or ID\n"
      "    -off <GROUP or ID>        turn log OFF for given GROUP or ID\n"
      "    -ON                       turn all logs ON\n"
      "    -OFF                      turn all logs OFF\n"
      "                              note: you may pass several -on/-off/-ON/-OFF,\n"
      "                                    they will be processed in order\n"
      "                                    by default, all is off\n"
      "    -ip <host>                connect to given IP address (default %s)\n"
      "    -p <port>                 connect to given port (default %d)\n"
      "    -ch-host <host>           ClickHouse host (default %s)\n"
      "    -ch-port <port>           ClickHouse native port (default %d)\n"
      "    -ch-db <database>         ClickHouse database name (default %s)\n"
      "    -ch-user <user>           ClickHouse user (optional)\n"
      "    -ch-pass <password>       ClickHouse password (optional)\n"
      "    -ch-droptables            drop the ClickHouse database before run\n"
      "    -debug                    enable debug logging\n",
      DEFAULT_REMOTE_IP,
      DEFAULT_REMOTE_PORT,
      DEFAULT_CLICKHOUSE_HOST,
      DEFAULT_CLICKHOUSE_PORT,
      DEFAULT_CLICKHOUSE_DB);
  exit(1);
}

volatile int run = 1;

static int socket_fd = -1;

void force_stop(int x)
{
  logi("gently quit %d", x);
  close(socket_fd);
  socket_fd = -1;
  run = 0;
}

struct clickhouse_config_t {
  std::string host;
  int port;
  std::string database;
  std::string user;
  std::string password;
  std::unique_ptr<ch::Client> client;
  int drop_tables;
};

// Buffer management structures
struct DataBuffer {
  uint8_t *buffer; // Large preallocated buffer
  size_t buffer_size; // Total size of buffer
  size_t current_offset; // Current write position in buffer
  std::shared_ptr<ch::ColumnUInt8> inner_col; // Reusable inner column for ClickHouse
  std::shared_ptr<ch::ColumnUInt64> offsets_col; // Reusable offsets column for ClickHouse (cumulative end offsets)
  bool insert_now;

  DataBuffer(size_t size) : buffer_size(size - BUFFER_MARGIN), current_offset(0), insert_now(false)
  {
    buffer = (uint8_t *)malloc(size);
    if (!buffer) {
      throw std::bad_alloc();
    }

    // Create and reserve ClickHouse columns for reuse
    inner_col = std::make_shared<ch::ColumnUInt8>();
    auto &inner_data = inner_col->GetWritableData();
    inner_data.reserve(size);

    offsets_col = std::make_shared<ch::ColumnUInt64>();
    offsets_col->Reserve(BATCH_SIZE);
  }

  ~DataBuffer()
  {
    if (buffer) {
      free(buffer);
      buffer = nullptr;
    }
  }

  // Disable copy constructor and assignment operator
  DataBuffer(const DataBuffer &) = delete;
  DataBuffer &operator=(const DataBuffer &) = delete;

  // Copy data into buffer and return offset
  size_t append(const void *data, size_t length)
  {
    if (current_offset + length > buffer_size) {
      insert_now = true; // If this puts the buffer into the margin, insert it immediately
      if (length > BUFFER_MARGIN) {
        loge("DataBuffer full: current_offset={} length={} buffer_size={}", current_offset, length, buffer_size);
        return (size_t)-1;
      } // Else we have margin
    }

    size_t offset = current_offset;
    memcpy(buffer + current_offset, data, length);
    current_offset += length;

    offsets_col->Append(current_offset);

    return offset;
  }

  // Get pointer to data at a specific offset
  const uint8_t *get_data(size_t offset) const
  {
    if (offset >= current_offset) {
      return nullptr;
    }
    return buffer + offset;
  }

  // Clear buffer for reuse
  void clear()
  {
    current_offset = 0;
    insert_now = false;
    inner_col->Clear();
    offsets_col->Clear();
  }

  // Get number of entries
  size_t entry_count() const
  {
    return offsets_col->Size();
  }

  // Get current usage
  size_t usage() const
  {
    return current_offset;
  }
};

struct EventBuffer {
  std::vector<ch::ColumnRef> columns; // Ordered list of columns
  std::vector<std::string> column_names; // Column names in same order
  std::string event_table_name;
  size_t row_count;
  bool insert_now;
  std::chrono::steady_clock::time_point last_insert_time;
  database_event_format format;
  std::string event_name;
  bool has_frame_slot;
  std::unordered_map<int, std::unique_ptr<DataBuffer>> data_buffers;

  // Cached typed column pointers to avoid expensive As<> casts on every insert
  std::shared_ptr<ch::ColumnDateTime64> ts_sw_col;
  std::shared_ptr<ch::ColumnDateTime64> ts_tx_col;
  std::vector<std::shared_ptr<ch::ColumnInt32>> int_cols;
  std::vector<std::shared_ptr<ch::ColumnUInt64>> ulong_cols;
  std::vector<std::shared_ptr<ch::ColumnString>> string_cols;
  std::unordered_map<int, int> field_idx_to_typed_col_idx; // Maps format field index to typed column index

  // Performance metrics
  uint64_t last_column_build_duration_ns;
  uint64_t last_block_append_duration_ns;
  uint64_t last_insertion_duration_ns;

  // Statistics tracking
  uint64_t total_rows; // Total rows inserted since start
  uint64_t rows_since_print; // Rows since last print

  EventBuffer()
      : row_count(0),
        insert_now(false),
        last_insert_time(std::chrono::steady_clock::now()),
        has_frame_slot(false),
        last_column_build_duration_ns(0),
        last_block_append_duration_ns(0),
        last_insertion_duration_ns(0),
        total_rows(0),
        rows_since_print(0)
  {
  }
};

// Global buffer map: event_id -> EventBuffer
std::unordered_map<int, EventBuffer> g_event_buffers;

/* Convert T format type to ClickHouse type */
const char *get_clickhouse_type(const char *t_type)
{
  if (strcmp(t_type, "int") == 0)
    return "Int32";
  if (strcmp(t_type, "ulong") == 0)
    return "UInt64";
  if (strcmp(t_type, "string") == 0)
    return "String";
  if (strcmp(t_type, "buffer") == 0)
    return "Array(UInt8)"; // Store as byte array
  if (strncmp(t_type, "int", 3) == 0)
    return "Int32";
  return "String"; // default
}

/* Execute ClickHouse query */
int clickhouse_execute(clickhouse_config_t *ch, const char *query)
{
  try {
    ch->client->Execute(query);
    return 0;
  } catch (const std::exception &e) {
    loge("ClickHouse query failed: {}", e.what());
    return -1;
  }
}

/* Initialize ClickHouse connection and create database */
int clickhouse_init(clickhouse_config_t *ch)
{
  char query[1024];

  try {
    // Set connection options
    ch::ClientOptions options;
    options.SetHost(ch->host);
    options.SetPort(ch->port);

    if (!ch->user.empty()) {
      options.SetUser(ch->user);
    }
    if (!ch->password.empty()) {
      options.SetPassword(ch->password);
    }

    // Create client
    ch->client = std::make_unique<ch::Client>(options);
    /* Handle drop database option */
    if (ch->drop_tables) {
      logi("Dropping database: {}", ch->database.c_str());
      snprintf(query, sizeof(query), "DROP DATABASE IF EXISTS %s", ch->database.c_str());

      if (clickhouse_execute(ch, query) != 0) {
        loge("Failed to drop database {}", ch->database);
        exit(1);
      }
    }

    // Create database if not exists
    snprintf(query, sizeof(query), "CREATE DATABASE IF NOT EXISTS %s", ch->database.c_str());
    if (clickhouse_execute(ch, query) != 0) {
      loge("Failed to create database");
      return -1;
    }

    logi("ClickHouse connection initialized");
    return 0;
  } catch (const std::exception &e) {
    loge("Failed to create ClickHouse client: {}", e.what());
    return -1;
  }
}

/* Create table schema from database format */
char *create_table_schema(char *schema_buf, const char *event_name, database_event_format format, const char *database)
{
  char *p = schema_buf;
  bool has_frame = false;
  bool has_slot = false;
  int i;

  // Start CREATE TABLE statement
  p += snprintf(p,
                MAX_QUERY_SIZE - (p - schema_buf),
                "CREATE TABLE IF NOT EXISTS %s.%s (\n  TsSwNs DateTime64(9) DEFAULT now64(9)",
                database,
                event_name);

  for (i = 0; i < format.count; i++) {
    if (strcmp(format.name[i], "frame") == 0)
      has_frame = true;
    if (strcmp(format.name[i], "slot") == 0)
      has_slot = true;
    if (strcmp(format.name[i], "subframe") == 0)
      has_slot = true;
  }
  if (has_frame && has_slot) {
    p += snprintf(p, MAX_QUERY_SIZE - (p - schema_buf), ",\n  TsTxUtcNs DateTime64(9)");
    uint64_t timestamp_ns = std::chrono::system_clock::now().time_since_epoch().count() / NS_PER_SEC;
    if (timestamp_ns > 1861920000) {
      // 1861920000 -> 2029-01-01 00:00:00
      logw("WARNING: Linux time {} is greater than 2029-01-01 00:00:00.", timestamp_ns);
      logw("         Check if a negative leap second has been announced and change UTC_TO_GPS_OFFSET_NS accordingly.",
           timestamp_ns);
    }
  }

  // Add columns from format
  for (i = 0; i < format.count; i++) {
    const char *ch_type = get_clickhouse_type(format.type[i]);
    p += snprintf(p, MAX_QUERY_SIZE - (p - schema_buf), ",\n  %s %s", format.name[i], ch_type);
  }

  p += snprintf(p, MAX_QUERY_SIZE - (p - schema_buf), "\n) ENGINE = Memory();");

#ifdef DB_DEBUG
  logd("schema: {}", schema_buf);
#endif
  return schema_buf;
}

/* Initialize buffer for an event */
void init_event_buffer(int event_id, void *database, const char *database_name)
{
  char *event_name = event_name_from_id(database, event_id);
  database_event_format format = get_format(database, event_id);

  if (!event_name)
    return;

  EventBuffer &buf = g_event_buffers[event_id];
  buf.event_name = std::string(event_name);
  buf.event_table_name = std::string(database_name) + "." + event_name;
  buf.format = format;
  buf.row_count = 0;
  buf.last_insert_time = std::chrono::steady_clock::now();

  // Check if this event has frame/slot fields
  buf.has_frame_slot = false;
  bool has_frame = false;
  bool has_slot = false;
  for (int i = 0; i < format.count; i++) {
    if (strcmp(format.name[i], "frame") == 0)
      has_frame = true;
    if (strcmp(format.name[i], "slot") == 0 || strcmp(format.name[i], "subframe") == 0)
      has_slot = true;
  }
  buf.has_frame_slot = has_frame && has_slot;

  // Create TsSwNs column
  auto ts_col = std::make_shared<ch::ColumnDateTime64>(9);
  ts_col->Reserve(BATCH_SIZE);
  buf.columns.push_back(ts_col);
  buf.column_names.push_back("TsSwNs");
  buf.ts_sw_col = ts_col; // Cache typed pointer

  // Add data columns
  for (int i = 0; i < format.count; i++) {
    if (strcmp(format.type[i], "int") == 0) {
      auto col = std::make_shared<ch::ColumnInt32>();
      col->Reserve(BATCH_SIZE);
      buf.columns.push_back(col);
      buf.column_names.push_back(format.name[i]);
      buf.field_idx_to_typed_col_idx[i] = buf.int_cols.size();
      buf.int_cols.push_back(col);
    } else if (strcmp(format.type[i], "ulong") == 0) {
      auto col = std::make_shared<ch::ColumnUInt64>();
      col->Reserve(BATCH_SIZE);
      buf.columns.push_back(col);
      buf.column_names.push_back(format.name[i]);
      buf.field_idx_to_typed_col_idx[i] = buf.ulong_cols.size();
      buf.ulong_cols.push_back(col);
    } else if (strcmp(format.type[i], "buffer") == 0) {
      buf.data_buffers[i] = std::make_unique<DataBuffer>(BUFFER_SIZE);

      // Placeholder - array column will be created from DataBuffer at flush time
      buf.columns.push_back(nullptr);
      buf.column_names.push_back(format.name[i]);

    } else if (strcmp(format.type[i], "string") == 0) {
      auto col = std::make_shared<ch::ColumnString>();
      col->Reserve(BATCH_SIZE);
      buf.columns.push_back(col);
      buf.column_names.push_back(format.name[i]);
      buf.field_idx_to_typed_col_idx[i] = buf.string_cols.size();
      buf.string_cols.push_back(col);
    } else {
      // Default: treat as int
      auto col = std::make_shared<ch::ColumnInt32>();
      col->Reserve(BATCH_SIZE);
      buf.columns.push_back(col);
      buf.column_names.push_back(format.name[i]);
      buf.field_idx_to_typed_col_idx[i] = buf.int_cols.size();
      buf.int_cols.push_back(col);
    }
  }

  // Add TsTxUtcNs column if event has frame/slot
  if (buf.has_frame_slot) {
    auto gps_col = std::make_shared<ch::ColumnDateTime64>(9);
    gps_col->Reserve(BATCH_SIZE);
    buf.columns.push_back(gps_col);
    buf.column_names.push_back("TsTxUtcNs");
    buf.ts_tx_col = gps_col; // Cache typed pointer
  }

  logi("Initialized buffer for event_id:{:<4} name:{:<31} has_frame_slot:{} columns:{}",
       event_id,
       event_name,
       buf.has_frame_slot,
       buf.columns.size());

  // Print sizes of buffer fields
  for (int i = 0; i < format.count; i++) {
    if (strcmp(format.type[i], "buffer") == 0) {
      logd("  Buffer field '{}' size={} MB", format.name[i], BUFFER_SIZE / (1024 * 1024));
    }
  }
}

/* Create tables for enabled events */
int create_tables(clickhouse_config_t *ch, void *database, int *is_on, int number_of_events)
{
  int i;
  size_t longest_table_name = 0;

  for (i = 0; i < number_of_events; i++) {
    if (!is_on[i])
      continue;

    char *event_name = event_name_from_id(database, i);
    database_event_format format = get_format(database, i);
    if (strlen(event_name) > longest_table_name)
      longest_table_name = strlen(event_name);

    if (!event_name)
      continue;

    logd("Creating table for event: {:<31}", event_name);

    char schema[MAX_QUERY_SIZE];
    create_table_schema(schema, event_name, format, ch->database.c_str());

    if (clickhouse_execute(ch, schema) != 0) {
      loge("Failed to create table {}", event_name);
      return -1;
    }

    // Initialize buffer for this event
    init_event_buffer(i, database, ch->database.c_str());
  }

  return (int)longest_table_name;
}

int insert_buffer(clickhouse_config_t *ch, int event_id)
{
  auto it = g_event_buffers.find(event_id);
  if (it == g_event_buffers.end() || it->second.row_count == 0) {
    return 0; // Nothing to flush
  }

  EventBuffer &buf = it->second;
  buf.insert_now = false;

  auto t_reconstruct_start = std::chrono::high_resolution_clock::now();
  size_t total_data_size = 0;
  for (auto &dbuf_pair : buf.data_buffers) {
    int field_idx = dbuf_pair.first;
    DataBuffer *dbuf = dbuf_pair.second.get();
    int col_idx = field_idx + 1; // +1 because TsSwNs is at index 0

    // Build the array column from the DataBuffer
    size_t num_entries = dbuf->offsets_col->Size();

    if (num_entries != buf.row_count) {
      loge("DataBuffer entry count mismatch for field '{}': {} entries vs {} rows",
           buf.format.name[field_idx],
           num_entries,
           buf.row_count);
    }

    // Copy all data to the DataBuffer's inner column (space already reserved in constructor)
    if (dbuf->current_offset > 0) {
      auto &inner_data = dbuf->inner_col->GetWritableData();
      inner_data.resize(dbuf->current_offset); // Set size (doesn't reallocate since already reserved)
      memcpy(inner_data.data(), dbuf->buffer, dbuf->current_offset);
    }

    // Create the array column from DataBuffer's columns and store it
    auto array_col = std::make_shared<ch::ColumnArrayT<ch::ColumnUInt8>>(dbuf->inner_col, dbuf->offsets_col);
    buf.columns[col_idx] = array_col;

    total_data_size += dbuf->current_offset;
  }
  auto t_reconstruct_end = std::chrono::high_resolution_clock::now();

  ch::Block block;
  for (size_t i = 0; i < buf.columns.size(); i++) {
    if (buf.columns[i]) {
      block.AppendColumn(buf.column_names[i], buf.columns[i]);
    } else {
      loge("Column '{}' at index {} is null when creating block", buf.column_names[i], i);
      exit(1);
    }
  }
  try {
    auto t_insert_start = std::chrono::high_resolution_clock::now();
    ch->client->Insert(buf.event_table_name.c_str(), block);
    auto t_insert_end = std::chrono::high_resolution_clock::now();

    buf.last_column_build_duration_ns = DURATION_NS(t_reconstruct_start, t_reconstruct_end);
    buf.last_block_append_duration_ns = DURATION_NS(t_reconstruct_end, t_insert_start);
    buf.last_insertion_duration_ns = DURATION_NS(t_insert_start, t_insert_end);

    // Clear all columns and re-reserve (skip buffer-type columns which are rebuilt each time)
    for (size_t i = 0; i < buf.columns.size(); i++) {
      if (buf.columns[i]) { // Skip nullptr placeholders for buffer-type columns
        auto col = std::const_pointer_cast<ch::Column>(buf.columns[i]);
        col->Clear();
        col->Reserve(BATCH_SIZE);
      }
    }

    // Reset buffer-type columns back to nullptr
    for (auto &dbuf_pair : buf.data_buffers) {
      int field_idx = dbuf_pair.first;
      int col_idx = field_idx + 1;
      buf.columns[col_idx] = nullptr;
    }

    // Clear all DataBuffers
    for (auto &pair : buf.data_buffers) {
      pair.second->clear();
    }

    int rows = buf.row_count;
    buf.row_count = 0;
    buf.last_insert_time = std::chrono::steady_clock::now();

    // Update statistics
    buf.total_rows += rows;
    buf.rows_since_print += rows;

    return rows;
  } catch (const std::exception &e) {
    loge("Failed to insert buffer for event {}: {}", buf.event_name, e.what());
    return -1;
  }
}

/* Flush all buffers to ClickHouse */
int insert_all_buffers(clickhouse_config_t *ch)
{
  int errors = 0;
  for (auto &pair : g_event_buffers) {
    if (insert_buffer(ch, pair.first) <= 0) {
      errors++;
    }
  }
  return errors > 0 ? -1 : 0;
}

/* Check if buffer should be inserted based on size or time */
uint8_t should_insert(EventBuffer &buf)
{
  if (buf.row_count == 0) {
    return 0;
  }

  if (buf.row_count >= BATCH_SIZE) {
    return 1;
  }

  if (static_cast<uint64_t>(DURATION_MS(buf.last_insert_time, std::chrono::steady_clock::now())) >= INSERT_INTERVAL_MS) {
    return 2;
  }

  if (buf.insert_now) {
    return 3;
  }

  return 0;
}

int insert_event(clickhouse_config_t *ch, int event_id, const char *event_data, int data_length)
{
  auto t_fn_start = std::chrono::high_resolution_clock::now();

  // Get buffer for this event
  auto it = g_event_buffers.find(event_id);
  if (it == g_event_buffers.end()) {
    loge("Buffer not found for event_id {}", event_id);
    return -1;
  }

  EventBuffer &buf = it->second;
  database_event_format format = buf.format;
  const char *event_name = buf.event_name.c_str();
  auto t_names_done = std::chrono::high_resolution_clock::now();

  int16_t frame = -1;
  int16_t slot = -1;

  try {
    const char *data_ptr = event_data;
    int remaining = data_length;
    int buf_len = 0;
    int i;

    auto t_append_start = std::chrono::high_resolution_clock::now();

    // Append to TsSwNs column using cached pointer
    buf.ts_sw_col->Append(std::chrono::system_clock::now().time_since_epoch().count());

    // Process data columns
    for (i = 0; i < format.count && remaining > 0; i++) {
      if (strcmp(format.type[i], "int") == 0) {
        int val = 0;
        if (remaining >= 4) {
          memcpy(&val, data_ptr, 4);
          if (strcmp(format.name[i], "frame") == 0)
            frame = val;
          else if (strcmp(format.name[i], "slot") == 0)
            slot = val;
          else if (strcmp(format.name[i], "subframe") == 0)
            slot = val;
          data_ptr += 4;
          remaining -= 4;
        }

        int typed_col_idx = buf.field_idx_to_typed_col_idx[i];
        buf.int_cols[typed_col_idx]->Append(val);

      } else if (strcmp(format.type[i], "ulong") == 0) {
        uint64_t val = 0;
        if (remaining >= 8) {
          memcpy(&val, data_ptr, 8);
          data_ptr += 8;
          remaining -= 8;
        }

        int typed_col_idx = buf.field_idx_to_typed_col_idx[i];
        buf.ulong_cols[typed_col_idx]->Append(val);

      } else if (strcmp(format.type[i], "buffer") == 0) {
        buf_len = 0;
        const char *buf_data = nullptr;

        if (remaining >= 4) {
          memcpy(&buf_len, data_ptr, 4);
          data_ptr += 4;
          remaining -= 4;

          if (buf_len > remaining)
            buf_len = remaining;
          if (buf_len > 0)
            buf_data = data_ptr;

          data_ptr += buf_len;
          remaining -= buf_len;
        }

        auto it_dbuf = buf.data_buffers.find(i);
        if (it_dbuf != buf.data_buffers.end()) {
          size_t offset = it_dbuf->second->append(buf_data, buf_len);

          buf.insert_now |= it_dbuf->second->insert_now;

          if (offset == (size_t)-1) {
            loge("DataBuffer full for field '{}', cannot append {} bytes", format.name[i], buf_len);
            // Handle dropping tail for other columns and insert
          }
        }
      } else if (strcmp(format.type[i], "string") == 0) {
        std::string str_val;
        if (remaining >= 4) {
          int str_len;
          memcpy(&str_len, data_ptr, 4);
          data_ptr += 4;
          remaining -= 4;

          if (str_len > remaining)
            str_len = remaining;
          if (str_len > 0)
            str_val = std::string(data_ptr, str_len);

          data_ptr += str_len;
          remaining -= str_len;
        }

        int typed_col_idx = buf.field_idx_to_typed_col_idx[i];
        buf.string_cols[typed_col_idx]->Append(str_val);

      } else {
        // Default: treat as int
        int val = 0;
        if (remaining >= 4) {
          memcpy(&val, data_ptr, 4);
          data_ptr += 4;
          remaining -= 4;
        }

        int typed_col_idx = buf.field_idx_to_typed_col_idx[i];
        buf.int_cols[typed_col_idx]->Append(val);
      }
    }
    auto t_append_data_end = std::chrono::high_resolution_clock::now();

    // Add TsTxUtcNs column if event has frame/slot
    if (buf.has_frame_slot) {
      uint64_t timestamp_ns = std::chrono::system_clock::now().time_since_epoch().count();
      uint64_t gps_elapsed_ns;
      uint64_t transmit_time_in_utc_ns;
      auto t_time_calc_start = std::chrono::high_resolution_clock::now();

      if (frame == -1 || slot == -1) { // nr_ue_decode_BCCH_DL_SCH sets frame and slot to -1
        transmit_time_in_utc_ns = timestamp_ns;
      } else {
        gps_elapsed_ns = calculate_sfn_slot_gps_time(frame, slot, 1, timestamp_ns);
        transmit_time_in_utc_ns = gps_to_utc_ns(gps_elapsed_ns);

        if (0) {
          int check_frame, check_slot;
          calculate_sfn_slot_from_unix(transmit_time_in_utc_ns, 1, &check_frame, &check_slot);
          if (check_frame != frame || check_slot != slot) {
            loge("ERROR1: sfn/slot check failed: {} {} {} {}", event_name, frame, slot, check_frame, check_slot);
          }
        }

        auto t_time_calc_end = std::chrono::high_resolution_clock::now();

        int64_t ref_to_t0_offset_ns = (int64_t)timestamp_ns - (int64_t)transmit_time_in_utc_ns;
        // Handle sign separately to properly display negative fractional seconds
        const char *sign = (ref_to_t0_offset_ns < 0) ? "-" : " ";
        int64_t abs_offset = (ref_to_t0_offset_ns < 0) ? -ref_to_t0_offset_ns : ref_to_t0_offset_ns;
        int64_t offset_sec = abs_offset / NS_PER_SEC;
        int64_t offset_ns = abs_offset % NS_PER_SEC;

        if (abs_offset > 50'000'000) { // 50ms
          logi("{:<31} {:4d}.{:2d} TsSwNs={}.{:09} TsTxUtcNs={}.{:09} delta={}{}.{:09d}, {}",
               event_name,
               frame,
               slot,
               timestamp_ns / NS_PER_SEC,
               timestamp_ns % NS_PER_SEC,
               transmit_time_in_utc_ns / NS_PER_SEC,
               transmit_time_in_utc_ns % NS_PER_SEC,
               sign,
               offset_sec,
               offset_ns,
               DURATION_NS(t_time_calc_start, t_time_calc_end));
        }
      }

      // Append to TsTxUtcNs column using cached pointer
      buf.ts_tx_col->Append(transmit_time_in_utc_ns);
    }
    auto t_append_ts_end = std::chrono::high_resolution_clock::now();

    // Increment row count
    buf.row_count++;

    // Check if we should insert this buffer
    int rows = 0;
    auto t_insert_start = std::chrono::high_resolution_clock::now();
    uint8_t insert = should_insert(buf);
    auto t_insert_should_insert_end = std::chrono::high_resolution_clock::now();
    if (insert) {
      rows = insert_buffer(ch, event_id);
    }
    auto t_insert_end = std::chrono::high_resolution_clock::now();

    if (insert) {
      if (DURATION_MS(t_insert_start, t_insert_end) > 5) {
        logi("Slow Insert: {:<31} {:4d}.{:2d}: {:3d} {:3d} {:5d} {:4d} {:5d} {:4d} {:8d} (ns) {:3d} (ms) rows:{:3d} reason: {}",
             event_name,
             frame,
             slot,
             DURATION_NS(t_fn_start, t_names_done),
             DURATION_NS(t_names_done, t_append_start),
             DURATION_NS(t_append_start, t_append_data_end),
             DURATION_NS(t_append_data_end, t_append_ts_end),
             buf.last_column_build_duration_ns,
             buf.last_block_append_duration_ns,
             buf.last_insertion_duration_ns,
             DURATION_MS(t_insert_start, t_insert_end),
             rows,
             insert);
      }
    } else if (DURATION_US(t_fn_start, t_insert_end) > 50) {
      logd("Slow saving data: {:<31} {:4d}.{:2d}: {:3d} {:3d} {:5d} {:4d} {:5d} {:4d} {:8d} (ns) buf_len:{:3d} rows:{:3d} ---",
           event_name,
           frame,
           slot,
           DURATION_NS(t_fn_start, t_names_done),
           DURATION_NS(t_names_done, t_append_start),
           DURATION_NS(t_append_start, t_append_data_end),
           DURATION_NS(t_append_data_end, t_append_ts_end),
           DURATION_NS(t_append_ts_end, t_insert_start),
           DURATION_NS(t_insert_start, t_insert_should_insert_end),
           DURATION_NS(t_insert_should_insert_end, t_insert_end),
           buf_len,
           buf.row_count);
    }

    return 0;
  } catch (const std::exception &e) {
    loge("Failed to insert event {}: {}", event_name, e.what());
    return -1;
  }
}

int main(int n, char **v)
{
  const char *default_database_filename = "T_messages.txt";
  char *database_filename = NULL;
  void *database;
  const char *ip = DEFAULT_REMOTE_IP;
  int port = DEFAULT_REMOTE_PORT;
  char **on_off_name;
  int *on_off_action;
  int on_off_n = 0;
  int *is_on;
  int number_of_events;
  int i;
  char mt;
  int debug_mode = 0;

  clickhouse_config_t ch_config;
  ch_config.drop_tables = 0;
  ch_config.host = DEFAULT_CLICKHOUSE_HOST;
  ch_config.port = DEFAULT_CLICKHOUSE_PORT;
  ch_config.database = DEFAULT_CLICKHOUSE_DB;

  /* write on a socket fails if the other end is closed and we get SIGPIPE */
  if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
    abort();

  on_off_name = (char **)malloc(n * sizeof(char *));
  if (on_off_name == NULL)
    abort();
  on_off_action = (int *)malloc(n * sizeof(int));
  if (on_off_action == NULL)
    abort();

  for (i = 1; i < n; i++) {
    if (!strcmp(v[i], "-h") || !strcmp(v[i], "--help"))
      usage();
    if (!strcmp(v[i], "-d")) {
      if (i > n - 2)
        usage();
      database_filename = v[++i];
      continue;
    }
    if (!strcmp(v[i], "-ip")) {
      if (i > n - 2)
        usage();
      ip = v[++i];
      continue;
    }
    if (!strcmp(v[i], "-p")) {
      if (i > n - 2)
        usage();
      port = atoi(v[++i]);
      continue;
    }
    if (!strcmp(v[i], "-ch-host")) {
      if (i > n - 2)
        usage();
      ch_config.host = v[++i];
      continue;
    }
    if (!strcmp(v[i], "-ch-port")) {
      if (i > n - 2)
        usage();
      ch_config.port = atoi(v[++i]);
      continue;
    }
    if (!strcmp(v[i], "-ch-db")) {
      if (i > n - 2)
        usage();
      ch_config.database = v[++i];
      continue;
    }
    if (!strcmp(v[i], "-ch-user")) {
      if (i > n - 2)
        usage();
      ch_config.user = v[++i];
      continue;
    }
    if (!strcmp(v[i], "-ch-pass")) {
      if (i > n - 2)
        usage();
      ch_config.password = v[++i];
      continue;
    }
    if (!strcmp(v[i], "-ch-droptables")) {
      ch_config.drop_tables = 1;
      continue;
    }
    if (!strcmp(v[i], "-debug")) {
      debug_mode = 1;
      continue;
    }
    if (!strcmp(v[i], "-on")) {
      if (i > n - 2)
        usage();
      on_off_name[on_off_n] = v[++i];
      on_off_action[on_off_n++] = 1;
      logi("on: item: {}, on_off_name: {}, on_off_action: {}", v[i], on_off_name[on_off_n - 1], on_off_action[on_off_n - 1]);
      continue;
    }
    if (!strcmp(v[i], "-off")) {
      if (i > n - 2)
        usage();
      on_off_name[on_off_n] = v[++i];
      on_off_action[on_off_n++] = 0;
      continue;
    }
    if (!strcmp(v[i], "-ON")) {
      on_off_name[on_off_n] = NULL;
      on_off_action[on_off_n++] = 1;
      continue;
    }
    if (!strcmp(v[i], "-OFF")) {
      on_off_name[on_off_n] = NULL;
      on_off_action[on_off_n++] = 0;
      continue;
    }
    usage();
  }

  if (database_filename == NULL) {
    logi("No database file provided, using default: {}", default_database_filename);
    database_filename = strdup(default_database_filename);
  }

  database = parse_database(database_filename);

  load_config_file(database_filename);

  number_of_events = number_of_ids(database);
  logi("Number of events: {}", number_of_events);
  is_on = (int *)calloc(number_of_events, sizeof(int));
  if (is_on == NULL) {
    loge("aborting");
    abort();
  }

  for (i = 0; i < on_off_n; i++) {
    on_off(database, on_off_name[i], is_on, on_off_action[i]);
  }

  /* Initialize fmtlog for benchmarking */
  fmtlog::setLogLevel(debug_mode ? fmtlog::DBG : fmtlog::INF);
  fmtlog::setThreadName("main");

  /* Initialize ClickHouse */
  if (clickhouse_init(&ch_config) != 0) {
    loge("Failed to initialize ClickHouse connection");
    exit(1);
  }

  /* Create tables for enabled events */
  int longest_table_name = create_tables(&ch_config, database, is_on, number_of_events);
  if (longest_table_name < 0) {
    loge("Failed to create tables");
    exit(1);
  }

  socket_fd = connect_to(const_cast<char *>(ip), port);

  /* activate selected traces */
  mt = 1;
  if (socket_send(socket_fd, &mt, 1) == -1 || socket_send(socket_fd, &number_of_events, sizeof(int)) == -1
      || socket_send(socket_fd, is_on, number_of_events * sizeof(int)) == -1)
    abort();

  /* exit on ctrl+c and ctrl+z */
  if (signal(SIGQUIT, force_stop) == SIG_ERR)
    abort();
  if (signal(SIGINT, force_stop) == SIG_ERR)
    abort();
  if (signal(SIGTSTP, force_stop) == SIG_ERR)
    abort();

  OBUF ebuf = {.osize = 0, .omaxsize = 0, .obuf = NULL};

  /* read messages */
  logi("Tables created, waiting for messages...");
  int poll_count = 0;
  auto last_periodic_insert = std::chrono::steady_clock::now();
  auto last_stats_print = std::chrono::steady_clock::now();
  const uint64_t STATS_PRINT_INTERVAL_MS = 5000; // Print stats every 5 seconds

  while (run) {
    int type;
    int32_t length;
    char *v;
    int vpos = 0;

    // Poll fmtlog periodically to insert buffered logs
    if (++poll_count % 10 == 0) {
      fmtlog::poll();
    }

    // Periodic insert check (every 10 messages)
    if (poll_count % 10 == 0) {
      auto now = std::chrono::steady_clock::now();
      auto elapsed_ms =
          static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(now - last_periodic_insert).count());

      if (elapsed_ms >= INSERT_INTERVAL_MS) {
        for (auto &pair : g_event_buffers) {
          if (should_insert(pair.second)) {
            insert_buffer(&ch_config, pair.first);
          }
        }
        last_periodic_insert = now;
      }
      if (static_cast<uint64_t>(DURATION_MS(last_stats_print, now)) >= STATS_PRINT_INTERVAL_MS) {
        for (auto &pair : g_event_buffers) {
          EventBuffer &buf = pair.second;
          if (buf.rows_since_print > 0)
            logi("  {:<31} rows: {:8d}  total: {:10d}", buf.event_name, buf.rows_since_print, buf.total_rows);
          buf.rows_since_print = 0;
        }
        last_stats_print = now;
        logi("");
      }
    }

    if (fullread(socket_fd, &length, 4) == -1)
      goto read_error;
    if (ebuf.omaxsize < length + 4) {
      ebuf.omaxsize = (length + 4 + 65535) & ~65535;
      ebuf.obuf = (char *)realloc(ebuf.obuf, ebuf.omaxsize);
      if (ebuf.obuf == NULL) {
        loge("out of memory");
        exit(1);
      }
    }
    v = ebuf.obuf;
    memcpy(v + vpos, &length, 4);
    vpos += 4;
#ifdef T_SEND_TIME
    if (length < (int32_t)sizeof(struct timespec))
      goto read_error;
    if (fullread(socket_fd, v + vpos, sizeof(struct timespec)) == -1)
      goto read_error;
    // memcpy(v+vpos, &ts, sizeof(struct timespec));
    vpos += sizeof(struct timespec);
    length -= sizeof(struct timespec);
#endif
    if (length < (int32_t)sizeof(int))
      goto read_error;
    if (fullread(socket_fd, &type, sizeof(int)) == -1)
      goto read_error;
    memcpy(v + vpos, &type, sizeof(int));
    vpos += sizeof(int);
    length -= sizeof(int);
    if (fullread(socket_fd, v + vpos, length) == -1)
      goto read_error;

    if (type == -1) {
      append_received_config_chunk(v + vpos, length);
    } else if (type == -2) {
      verify_config();
    } else if (type >= 0 && type < number_of_events && is_on[type]) {
      if (insert_event(&ch_config, type, v + vpos, length) != 0) {
        loge("Failed to insert event type {}", type);
      }
    }
  }

read_error:
  // Flush all remaining buffers before exit
  logi("Inserting all buffers before exit...");
  insert_all_buffers(&ch_config);

  // Print final statistics
  size_t total_rows = 0;
  for (const auto& pair : g_event_buffers) {
    total_rows += pair.second.row_count;
  }
  logi("Final buffer state: {} events with {} uninserted rows",
         g_event_buffers.size(), total_rows);

  // Client cleanup is automatic via std::unique_ptr
  logi("Exiting...");

  // Flush and close fmtlog
  fmtlog::poll();
  fmtlog::closeLogFile();

  return 0;
}
