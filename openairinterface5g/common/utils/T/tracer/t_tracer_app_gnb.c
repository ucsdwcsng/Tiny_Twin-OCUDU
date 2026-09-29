/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief T-Tracer gnb service to capture trace Messages from gNB, it is used by data recording application
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include "database.h"
#include "event.h"
#include "handler.h"
#include "logger/logger.h"
#include "utils.h"
#include "event_selector.h"
#include "configuration.h"
#include "shared_memory_config.h"
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <poll.h>
#define MAX_FRAME_INDEX 1023
#define MAX_SLOT_INDEX 19
#define DEBUG_T_Tracer
#define DEBUG_BUFFER
// Duration to discard recording in milliseconds to mitigate reading data from
// the buffer in the stack from the previous record
#define DISCARD_RECORD_DURATION_MS 10

// Sentinel values for field_descriptor: source is e.sending_time
#define FIELD_SENDING_TIME_SEC  -2
#define FIELD_SENDING_TIME_NSEC -3

// State machine constants for shared memory protocol
#define STATE_WAIT   0
#define STATE_CONFIG 1
#define STATE_RECORD 2
#define STATE_STOP   3

// Visual separator for state machine transitions in console output
#define STATE_SEPARATOR "========================================"
#define PRINT_STATE_BANNER(state_name) \
  printf("\n%s\n  [STATE: %s]\n%s\n", STATE_SEPARATOR, state_name, STATE_SEPARATOR)

// Combine bytes in - little-endian format
int combine_bytes(const uint8_t *bytes, size_t num_bytes)
{
  int result = 0;
  for (size_t i = 0; i < num_bytes; ++i) {
    result |= (bytes[i] << (i * 8));
  }
  return result;
}

// Check if a message ID is present in a given list of message IDs
bool is_message_in_list(int msg_id_list[], int n_msgs, int msg_id)
{
  for (int i = 0; i < n_msgs; i++) {
    if (msg_id == msg_id_list[i]) {
      return true;
    }
  }
  return false;
}

// Get the current time
struct timespec get_current_time()
{
  struct timespec time;
  clock_gettime(CLOCK_MONOTONIC, &time);
  return time;
}

// Calculate the time difference in milliseconds
double calculate_time_difference(struct timespec start, struct timespec end)
{
  double start_ms = start.tv_sec * 1000.0 + start.tv_nsec / 1000000.0;
  double end_ms = end.tv_sec * 1000.0 + end.tv_nsec / 1000000.0;
  return end_ms - start_ms;
}

// Get Time Stamp in microseconds in YYYYMMDDHHMMSSmmmuuu format
char *get_time_stamp_usec(char time_stamp_str[])
{
  // initialization to measure time stamp
  time_t my_time;
  struct tm *timeinfo;
  time(&my_time);
  struct timeval tv;

  // get time stamp
  timeinfo = localtime(&my_time);
  gettimeofday(&tv, NULL);
  // Add time stamp: yyyy mm dd hh mm ss usec
  uint16_t year = timeinfo->tm_year + 1900;
  uint8_t mon = timeinfo->tm_mon + 1;
  uint8_t mday = timeinfo->tm_mday;
  uint8_t hour = timeinfo->tm_hour;
  uint8_t min = timeinfo->tm_min;
  uint8_t sec = timeinfo->tm_sec;
  uint32_t usec = (tv.tv_usec);
  sprintf(time_stamp_str, "%04d%02d%02d%02d%02d%02d%06u", year, mon, mday, hour, min, sec, usec);

  return time_stamp_str;
}

void err_exit(char *buf)
{
  fprintf(stderr, "%s\n", buf);
  exit(1);
}

// create shared memory
int create_shm(char **addrN, const char *shm_path, int projectId)
{
  key_t key;
  if (-1 != open(shm_path, O_CREAT, 0777)) {
    key = ftok(shm_path, projectId);
  } else {
    err_exit("Error: open shared memory");
  }

  if (key < 0) {
    err_exit("Error: ftok error");
  }

  int shm_id;
  shm_id = shmget(key, SHMSIZE, IPC_CREAT | IPC_EXCL | 0664);
  if (shm_id == -1) {
    if (errno == EEXIST) {
      printf("Error: shared memory already exists\n");
      shm_id = shmget(key, 0, 0);
      printf("reference shm_id = %d\n", shm_id);
    } else {
      perror("errno");
      err_exit("shmget error");
    }
  }
  char *addr;
  // address to attach - attach for read & write
  if ((addr = shmat(shm_id, 0, 0)) == (void *)-1) {
    if (shmctl(shm_id, IPC_RMID, NULL) == -1)
      err_exit("Error: shmctl error");
    else {
      printf("Attach shared memory failed\n");
      printf("remove shared memory identifier successful\n");
    }
    err_exit("shmat error");
  }
  *addrN = addr;
  return shm_id;
}

// delete shared memory
void del_shm(char *addr, int shm_id)
{
  if (shmdt(addr) < 0)
    err_exit("shmdt error");

  if (shmctl(shm_id, IPC_RMID, NULL) == -1)
    err_exit("shmctl error");
  else {
    printf("Remove shared memory identifier successful\n");
  }
}

/* this function sends the activated traces to the nr-softmodem */
void activate_traces(int socket, int number_of_events, int *is_on)
{
  char t = 1;
  if (socket_send(socket, &t, 1) == -1 || socket_send(socket, &number_of_events, sizeof(int)) == -1
      || socket_send(socket, is_on, number_of_events * sizeof(int)) == -1)
    abort();
}

void usage(void)
{
  printf(
      "options:\n"
      "    -d <database file>        this option is mandatory\n"
      "    -ip <host>                connect to given IP address (default %s)\n"
      "    -p <port>                 connect to given port (default %d)\n",
      DEFAULT_REMOTE_IP,
      DEFAULT_REMOTE_PORT);
  exit(1);
}

// struct for trace message based on Data Collection Trace Messages Structure
// you need to define the variables of each message
typedef struct {
  /* Data Collection Trace Message Structure  */
  int frame;
  int slot;
  int frame_type, freq_range, subcarrier_spacing, cyclic_prefix, symbols_per_slot;
  int Nid_cell, rnti;
  int rb_size, rb_start, start_symbol_index, nr_of_symbols;
  int qam_mod_order, mcs_index, mcs_table, nrOfLayers, transform_precoding;
  int dmrs_config_type, ul_dmrs_symb_pos, number_dmrs_symbols;
  int dmrs_port, dmrs_nscid, nb_antennas_rx, number_of_bits;
  int data;
} event_trace_msg_ul_data;

void setup_trace_msg_ul_data(event_trace_msg_ul_data *d, void *database)
{
  database_event_format f;
  int i;

  // Initialize all fields to -1 (marks unset indices)
  memset(d, -1, sizeof(*d));

/* this macro looks for a particular element and checks its type */
#define G(var_name, var_type, var)                                      \
  if (!strcmp(f.name[i], var_name)) {                                   \
    if (strcmp(f.type[i], var_type)) {                                  \
      printf("Error: Trace Message has a bad type for %s\n", var_name); \
      exit(1);                                                          \
    }                                                                   \
    var = i;                                                            \
    continue;                                                           \
  }

  // Get a template of any UL message based on Data Collection Trace Messages Structure
  int Trace_MSG_ID = event_id_from_name(database, "GNB_PHY_UL_FD_PUSCH_IQ");
  f = get_format(database, Trace_MSG_ID);

  // Map each field name to its index in the event structure
  for (i = 0; i < f.count; i++) {
    G("frame", "int", d->frame)
    G("slot", "int", d->slot)
    G("frame_type", "int", d->frame_type)
    G("freq_range", "int", d->freq_range)
    G("subcarrier_spacing", "int", d->subcarrier_spacing)
    G("cyclic_prefix", "int", d->cyclic_prefix)
    G("symbols_per_slot", "int", d->symbols_per_slot)
    G("Nid_cell", "int", d->Nid_cell)
    G("rnti", "int", d->rnti)
    G("rb_size", "int", d->rb_size)
    G("rb_start", "int", d->rb_start)
    G("start_symbol_index", "int", d->start_symbol_index)
    G("nr_of_symbols", "int", d->nr_of_symbols)
    G("qam_mod_order", "int", d->qam_mod_order)
    G("mcs_index", "int", d->mcs_index)
    G("mcs_table", "int", d->mcs_table)
    G("nrOfLayers", "int", d->nrOfLayers)
    G("transform_precoding", "int", d->transform_precoding)
    G("dmrs_config_type", "int", d->dmrs_config_type)
    G("ul_dmrs_symb_pos", "int", d->ul_dmrs_symb_pos)
    G("number_dmrs_symbols", "int", d->number_dmrs_symbols)
    G("dmrs_port", "int", d->dmrs_port)
    G("dmrs_nscid", "int", d->dmrs_nscid)
    G("nb_antennas_rx", "int", d->nb_antennas_rx)
    G("number_of_bits", "int", d->number_of_bits)
    G("data", "buffer", d->data)
  }
#undef G
}

// -------------------------------------------------------------------
// Table-driven metadata shared memory writer and debug printer
// -------------------------------------------------------------------

// Descriptor for one metadata field to write to shared memory
typedef struct {
  int field_idx;     // index into event e.e[] array
  size_t wire_size;  // number of bytes to write to shm
} field_descriptor;

// Number of UL metadata fields written to shared memory (excludes msg_id and buffer)
#define N_UL_METADATA_FIELDS 27

// Build UL field descriptor array from a populated event_trace_msg_ul_data struct.
// Must be called after setup_trace_msg_ul_data() so that field indices are populated.
void build_ul_field_descriptors(field_descriptor *fields, const event_trace_msg_ul_data *d)
{
  int i = 0;
  fields[i++] = (field_descriptor){ d->frame, sizeof(uint16_t) };
  fields[i++] = (field_descriptor){ d->slot, sizeof(uint8_t) };
  fields[i++] = (field_descriptor){ FIELD_SENDING_TIME_SEC, sizeof(uint32_t) };
  fields[i++] = (field_descriptor){ FIELD_SENDING_TIME_NSEC, sizeof(uint32_t) };
  fields[i++] = (field_descriptor){ d->frame_type, sizeof(uint8_t) };
  fields[i++] = (field_descriptor){ d->freq_range, sizeof(uint8_t) };
  fields[i++] = (field_descriptor){ d->subcarrier_spacing, sizeof(uint8_t) };
  fields[i++] = (field_descriptor){ d->cyclic_prefix, sizeof(uint8_t) };
  fields[i++] = (field_descriptor){ d->symbols_per_slot, sizeof(uint8_t) };
  fields[i++] = (field_descriptor){ d->Nid_cell, sizeof(uint16_t) };
  fields[i++] = (field_descriptor){ d->rnti, sizeof(uint16_t) };
  fields[i++] = (field_descriptor){ d->rb_size, sizeof(uint16_t) };
  fields[i++] = (field_descriptor){ d->rb_start, sizeof(uint16_t) };
  fields[i++] = (field_descriptor){ d->start_symbol_index, sizeof(uint8_t) };
  fields[i++] = (field_descriptor){ d->nr_of_symbols, sizeof(uint8_t) };
  fields[i++] = (field_descriptor){ d->qam_mod_order, sizeof(uint8_t) };
  fields[i++] = (field_descriptor){ d->mcs_index, sizeof(uint8_t) };
  fields[i++] = (field_descriptor){ d->mcs_table, sizeof(uint8_t) };
  fields[i++] = (field_descriptor){ d->nrOfLayers, sizeof(uint8_t) };
  fields[i++] = (field_descriptor){ d->transform_precoding, sizeof(uint8_t) };
  fields[i++] = (field_descriptor){ d->dmrs_config_type, sizeof(uint8_t) };
  fields[i++] = (field_descriptor){ d->ul_dmrs_symb_pos, sizeof(uint16_t) };
  fields[i++] = (field_descriptor){ d->number_dmrs_symbols, sizeof(uint8_t) };
  fields[i++] = (field_descriptor){ d->dmrs_port, sizeof(uint16_t) };
  fields[i++] = (field_descriptor){ d->dmrs_nscid, sizeof(uint16_t) };
  fields[i++] = (field_descriptor){ d->nb_antennas_rx, sizeof(uint8_t) };
  fields[i++] = (field_descriptor){ d->number_of_bits, sizeof(uint32_t) };
}

// Print UL metadata fields for debug output
void print_ul_metadata_debug(event e, const event_trace_msg_ul_data *d, void *database)
{
  printf("\nUnix TS: %ld.%09ld", e.sending_time.tv_sec, e.sending_time.tv_nsec);
  printf("\nMessage Info: msg_id %s (%d) \n", event_name_from_id(database, e.type), e.type);
  printf("frame %d, slot %d, unix_ts %ld.%09ld\n",
         e.e[d->frame].i, e.e[d->slot].i,
         e.sending_time.tv_sec, e.sending_time.tv_nsec);
  printf("frame_type %d, freq_range %d, subcarrier_spacing %d, cyclic_prefix %d, symbols_per_slot %d\n",
         e.e[d->frame_type].i, e.e[d->freq_range].i,
         e.e[d->subcarrier_spacing].i, e.e[d->cyclic_prefix].i,
         e.e[d->symbols_per_slot].i);
  printf("Nid_cell %d, rnti %d, rb_size %d, rb_start %d, start_symbol_index %d, nr_of_symbols %d\n",
         e.e[d->Nid_cell].i, e.e[d->rnti].i,
         e.e[d->rb_size].i, e.e[d->rb_start].i,
         e.e[d->start_symbol_index].i, e.e[d->nr_of_symbols].i);
  printf("qam_mod_order %d, mcs_index %d, mcs_table %d, nrOfLayers %d, transform_precoding %d\n",
         e.e[d->qam_mod_order].i, e.e[d->mcs_index].i,
         e.e[d->mcs_table].i, e.e[d->nrOfLayers].i,
         e.e[d->transform_precoding].i);
  printf("dmrs_config_type %d, ul_dmrs_symb_pos %d, number_dmrs_symbols %d, dmrs_port %d, dmrs_nscid %d\n",
         e.e[d->dmrs_config_type].i, e.e[d->ul_dmrs_symb_pos].i,
         e.e[d->number_dmrs_symbols].i, e.e[d->dmrs_port].i,
         e.e[d->dmrs_nscid].i);
  printf("nb_antennas_rx %d, number_of_bits %d, data size %d\n",
         e.e[d->nb_antennas_rx].i, e.e[d->number_of_bits].i,
         e.e[d->data].bsize);
}

// Write metadata fields to shared memory using field descriptor table.
// Writes msg_id first, then loops through all fields.
void write_metadata_to_shm(char *addr_wr, unsigned int *bufIdx_wr, event e,
                           const field_descriptor *fields, int n_fields)
{
  // Write message ID
  memcpy(&addr_wr[*bufIdx_wr], &e.type, sizeof(uint16_t));
  *bufIdx_wr += sizeof(uint16_t);
  // Write all metadata fields
  for (int i = 0; i < n_fields; i++) {
    if (fields[i].field_idx == FIELD_SENDING_TIME_SEC) {
      uint32_t ts_sec = (uint32_t)e.sending_time.tv_sec;
      memcpy(&addr_wr[*bufIdx_wr], &ts_sec, fields[i].wire_size);
    } else if (fields[i].field_idx == FIELD_SENDING_TIME_NSEC) {
      uint32_t ts_nsec = (uint32_t)e.sending_time.tv_nsec;
      memcpy(&addr_wr[*bufIdx_wr], &ts_nsec, fields[i].wire_size);
    } else {
      memcpy(&addr_wr[*bufIdx_wr], &e.e[fields[i].field_idx].i, fields[i].wire_size);
    }
    *bufIdx_wr += fields[i].wire_size;
  }
}

void reestablish_connection(int *socket, char *ip, int port, int number_of_events, int *is_on)
{
  clear_remote_config();
  if (*socket != -1)
    close(*socket);

  /* connect to the nr-softmodem */
  *socket = connect_to(ip, port);
  if (*socket == -1) {
    printf("\n Failed to connect to nr-softmodem. Retrying...\n");
    return;
  }
  printf("\n Connected");

  /* activate the traces in the nr-softmodem */
  activate_traces(*socket, number_of_events, is_on);
  printf("\n Activated Traces in nr-softmodem");
}

int main(int n, char **v)
{
  // Define and initialize an array of strings to list
  // trace_msgs_support_data_Collection_format
  // it is used to parse the requested messages if it is based
  // on Data Collection Trace Messages Structure and supported tracer messages indices
  char *traces_ul_support_data_Collection_format[] = {"GNB_PHY_UL_FD_PUSCH_IQ",
                                                      "GNB_PHY_UL_FD_DMRS",
                                                      "GNB_PHY_UL_FD_CHAN_EST_DMRS_POS",
                                                      "GNB_PHY_UL_FD_CHAN_EST_DMRS_INTERPL",
                                                      "GNB_PHY_UL_PAYLOAD_RX_BITS",
                                                      "UE_PHY_UL_SCRAMBLED_TX_BITS",
                                                      "UE_PHY_UL_PAYLOAD_TX_BITS"};
  // extra number of records to simplify sync between base station and UE synchronization records.
  // if you have network delay, you can increase the number of records to capture
  int max_sync_offset = 6; // 6 frames ~ 60 ms
  // Calculate the size of the message array
  int n_ul_msgs = sizeof(traces_ul_support_data_Collection_format) / sizeof(traces_ul_support_data_Collection_format[0]);

  uint16_t msg_id = 0;
  uint16_t start_frame_number = 0;
  uint32_t number_records = 0; // number of records to capture, it is number of slots
  // array to store the requested tracer messages indices
  int req_tracer_msgs_indices[100] = {0};
  char *database_filename = NULL;
  void *database;
  char *ip = DEFAULT_REMOTE_IP;
  char ip_address[16]; // max IP address length is 15 + 1 for null terminator
  int port = DEFAULT_REMOTE_PORT;
  int *is_on;
  int number_of_events;
  int i;
  int socket = -1;
  char trace_time_stamp_str[30];

  // data structure for the trace messages based on Data Collection Trace Messages Structure
  event_trace_msg_ul_data trace_msg_ul_data;

  // initialization variables
  unsigned int bufIdx_wr = 0;
  unsigned int bufIdx_rd = 0;
  uint8_t num_req_tracer_msgs = 0;

  // initialize shared memory
  char *addr_wr, *addr_rd;
  PRINT_STATE_BANNER("INIT");
  printf("  Data Collection Service: Initializing shared memory ...\n");
  printf("  Directory 1: %s, Directory 2: %s\n", GETKEYDIR1_gNB, GETKEYDIR2_gNB);
  printf("  Project ID: %d\n", PROJECTID_gNB);
  int shm_id_wr = create_shm(&addr_wr, GETKEYDIR1_gNB, PROJECTID_gNB);
  int shm_id_rd = create_shm(&addr_rd, GETKEYDIR2_gNB, PROJECTID_gNB);
  del_shm(addr_wr, shm_id_wr);
  del_shm(addr_rd, shm_id_rd);
  shm_id_wr = create_shm(&addr_wr, GETKEYDIR1_gNB, PROJECTID_gNB);
  shm_id_rd = create_shm(&addr_rd, GETKEYDIR2_gNB, PROJECTID_gNB);

  /* write on a socket fails if the other end is closed and we get SIGPIPE */
  if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
    abort();

  /* parse command line options */
  // Port number and IP address are given via API and not via command line
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
    usage();
  }

  if (database_filename == NULL) {
    printf("ERROR: Provide a database file (-d)\n");
    exit(1);
  }

  /* load the database T_messages.txt */
  database = parse_database(database_filename);
  load_config_file(database_filename);

  /* an array of int for all the events defined in the database is needed */
  number_of_events = number_of_ids(database);
  is_on = calloc(number_of_events, sizeof(int));
  if (is_on == NULL)
    abort();

  // Set first byte of the shared memory to 0
  addr_rd[0] = 0; // important to check if we have new requested messages

  // read requested tracer msg indices from memory
  PRINT_STATE_BANNER("WAIT");
  printf("  Data Collection Service: Waiting for config message ...\n");
  // Wait for Action Config
  while (1) {
    if ((uint8_t)(addr_rd[0]) == STATE_WAIT) {
      usleep(50); // sleep for 50 us: 0.5 ms slot duration
    }
    // config state
    else if ((uint8_t)(addr_rd[0]) == STATE_CONFIG) {
      get_time_stamp_usec(trace_time_stamp_str);
      PRINT_STATE_BANNER("CONFIG");
      printf("  Received config message. Time Stamp: %s\n", trace_time_stamp_str);

      // get the IP address length in bytes
      bufIdx_rd = 1;
      uint8_t ip_address_length = addr_rd[bufIdx_rd];
      bufIdx_rd += 1; // + 1 byte = IP address length

      // read the IP address
      for (int i = 0; i < ip_address_length; i++) {
        ip_address[i] = addr_rd[bufIdx_rd];
        bufIdx_rd += 1;
      }

      ip = ip_address;

      // get the array bytes of the port number : 2 bytes
      uint8_t port_number_bytes[2] = {addr_rd[bufIdx_rd], addr_rd[bufIdx_rd + 1]};
      bufIdx_rd += 2; // + 2 bytes = port number

      port = combine_bytes(port_number_bytes, 2);
      printf("  Parameters: IP Address length: %d, IP Address: %s, Port Number: %d\n", ip_address_length, ip_address, port);
      addr_rd[0] = STATE_WAIT; // reset memory : to wait for record action
      break;
    }
  }
  // Establish connection to the nr-softmodem
  clear_remote_config();
  if (socket != -1)
    close(socket);

  /* connect to the nr-softmodem */
  socket = connect_to(ip, port);
  printf("  Connected to nr-softmodem\n");

  // Read Action record or stop
  PRINT_STATE_BANNER("WAIT");
  printf("  Data Collection Service: Waiting for record/stop message ...\n");
  while (1) {
    // wait for Action record
    if ((uint8_t)(addr_rd[0]) == STATE_WAIT) {
      usleep(50); // sleep for 50 us: 0.5 ms slot duration
    }

    // quit state
    else if ((uint8_t)(addr_rd[0]) == STATE_STOP) {
      PRINT_STATE_BANNER("STOP");
      printf("  Received 'stop' command. Exiting...\n");
      // Clean up and exit
      break;
    }
    // record state
    else if ((uint8_t)(addr_rd[0]) == STATE_RECORD) {
      // clear remote buffer if there is
      clear_remote_config();
      get_time_stamp_usec(trace_time_stamp_str);
      PRINT_STATE_BANNER("RECORD");
      printf("  Received record message. Time Stamp: %s\n", trace_time_stamp_str);

      // read number of requested messages
      bufIdx_rd = 1;
      num_req_tracer_msgs = addr_rd[bufIdx_rd];
      bufIdx_rd += 1;
      printf("  Number of requested tracer messages: %d\n", num_req_tracer_msgs);
      // reset memory : action to wait for next record action
      addr_rd[0] = STATE_WAIT;

      // read tracer msg IDs - every message ID has been stored in two bytes
      for (uint8_t msg_n = 0; msg_n < num_req_tracer_msgs; msg_n++) {
        // get the array bytes of the tracer message ID: 2 bytes
        uint8_t msg_id_bytes[2] = {addr_rd[bufIdx_rd], addr_rd[bufIdx_rd + 1]};
        bufIdx_rd += 2; // + 2 bytes = message ID
        msg_id = combine_bytes(msg_id_bytes, 2);

        req_tracer_msgs_indices[msg_n] = msg_id;
        printf(" msg_id: %d, ", msg_id);
      }
      // get the array bytes of the number of records: 4 bytes
      uint8_t number_records_bytes[4] = {addr_rd[bufIdx_rd],
                                         addr_rd[bufIdx_rd + 1],
                                         addr_rd[bufIdx_rd + 2],
                                         addr_rd[bufIdx_rd + 3]};
      bufIdx_rd += 4; // + 4 bytes = number of records

      number_records = combine_bytes(number_records_bytes, 4);

      printf("num_records: %d, ", number_records);

      // get the array bytes of the start frame number : 2 bytes
      uint8_t start_frame_number_bytes[2] = {addr_rd[bufIdx_rd], addr_rd[bufIdx_rd + 1]};
      bufIdx_rd += 2; // + 2 bytes = start frame number

      start_frame_number = combine_bytes(start_frame_number_bytes, 2);

      printf("start_frame: %d\n", start_frame_number);

      /* activate the trace in this array */
      printf("  Activating tracer messages:\n");
      for (i = 0; i < num_req_tracer_msgs; i++) {
        char *on_off_name = event_name_from_id(database, req_tracer_msgs_indices[i]);
        on_off(database, on_off_name, is_on, 1);
        printf("    %d: %s\n", req_tracer_msgs_indices[i], on_off_name);
      }

      // Build UL message ID array for generic UL dispatch
      int ul_msg_ids[n_ul_msgs];
      for (int i = 0; i < n_ul_msgs; i++) {
        ul_msg_ids[i] = event_id_from_name(database, traces_ul_support_data_Collection_format[i]);
      }

      // setup data for the trace messages
      setup_trace_msg_ul_data(&trace_msg_ul_data, database);
      printf("  Setup UL trace data done\n");

      // Build field descriptor table for table-driven shared memory writes
      field_descriptor ul_fields[N_UL_METADATA_FIELDS];
      build_ul_field_descriptors(ul_fields, &trace_msg_ul_data);

      // Get the start time
      struct timespec start_time = get_current_time();

      /* activate the traces in the nr-softmodem */
      activate_traces(socket, number_of_events, is_on);
      printf("  Activated traces in nr-softmodem\n");

      /* a buffer needed to receive events from the nr-softmodem */
      OBUF ebuf = {.osize = 0, .omaxsize = 0, .obuf = NULL};

      /* read events */
      int nrecord_idx = 0;
      bufIdx_wr = 0;
      // logic to be sure that we have a complete slot record
      bool start_recording = false;
      bool got_ref_frame_slot = false;
      int ref_slot = 0;
      int ref_frame = 0;
      int slot_difference = 0;
      int frame_difference = 0;
      int current_frame = 0, prev_frame = 0, current_slot = 0, prev_slot = 0;
      // offset to sync between base station and UE synchronization records or power measurements
      int sync_offset_index = 0; // increase the index only if the index of frame changes after getting all records
      // since we will use the frame difference to do extra records, we should be sure that the last slot is recorded completely
      printf("\n  Data Collection Service: Start reading messages ...\n");
      struct pollfd event_poll_fd;
      event_poll_fd.fd = socket;
      event_poll_fd.events = POLLIN;
      while (1) {
        // stop if number of records is done
        if ((nrecord_idx >= number_records) && (sync_offset_index >= max_sync_offset)) {
          // We added one to the number of records to capture the last record completely if
          // we have several messages enabled per slot
          break;
        }
        int poll_ret = poll(&event_poll_fd, 1, 1); // 1 ms timeout for poll
        if (poll_ret > 0 && (event_poll_fd.revents & POLLIN)) {
          event e = get_event(socket, &ebuf, database);

          if (e.type == -1) {
            printf("\n Link broken or unexpected message received. Re-establishing connection...\n");
            reestablish_connection(&socket, ip, port, number_of_events, is_on);
            continue; // Skip further processing and retry
          }

          // is it a requested message
          if (is_message_in_list(req_tracer_msgs_indices, num_req_tracer_msgs, e.type)) {
            // is it based on Data Collection Trace Messages Structure
            if (is_message_in_list(ul_msg_ids, n_ul_msgs, e.type)) {
              // Start recording from the next slot to mitigate capturing partial data
              // check if the current frame and slot are different from the previous frame and slot
              // Then, increase the record index
              if (start_recording == false) {
                if (got_ref_frame_slot == false) {
                  ref_frame = e.e[trace_msg_ul_data.frame].i;
                  ref_slot = e.e[trace_msg_ul_data.slot].i;
                  printf("\nMessage Info: msg_id %s (%d) \n", event_name_from_id(database, e.type), e.type);
                  got_ref_frame_slot = true;
                }

                current_frame = e.e[trace_msg_ul_data.frame].i;
                current_slot = e.e[trace_msg_ul_data.slot].i;
                frame_difference = (current_frame - ref_frame + MAX_FRAME_INDEX + 1) % (MAX_FRAME_INDEX + 1);
                slot_difference = (current_slot - ref_slot + MAX_SLOT_INDEX + 1) % (MAX_SLOT_INDEX + 1);
                printf("\n First frame.slot: %d.%d, current frame.slot: %d.%d, diff frame.slot: %d.%d",
                       ref_frame,
                       ref_slot,
                       current_frame,
                       current_slot,
                       frame_difference,
                       slot_difference);

                if ((ref_frame != current_frame) || (ref_slot != current_slot)) {
                  start_recording = true;
                  printf("\n Start recording from frame: %d, slot: %d ", e.e[trace_msg_ul_data.frame].i, e.e[trace_msg_ul_data.slot].i);
                }
              }
              // start recording from the next frame to mitigate capturing partial data
              if (start_recording == true) {
                printf("\n\nRecord number: %d", nrecord_idx);
#ifdef DEBUG_BUFFER
                printf("\nBuffer index in bytes: %d", bufIdx_wr);
#endif
                // --- UL IQ/Bits handler ---
                // Write metadata + buffer to shared memory
                write_metadata_to_shm(addr_wr, &bufIdx_wr, e, ul_fields, N_UL_METADATA_FIELDS);
                // Write buffer: size (uint32_t) + raw data
                memcpy(&addr_wr[bufIdx_wr], &e.e[trace_msg_ul_data.data].bsize, sizeof(uint32_t));
                bufIdx_wr += sizeof(uint32_t);
                memcpy(&addr_wr[bufIdx_wr], e.e[trace_msg_ul_data.data].b, e.e[trace_msg_ul_data.data].bsize);
                bufIdx_wr += e.e[trace_msg_ul_data.data].bsize;
#ifdef DEBUG_T_Tracer
                print_ul_metadata_debug(e, &trace_msg_ul_data, database);
#endif
                // check if the current frame and slot are different from the previous frame and slot
                // Then, increase the record index
                current_frame = e.e[trace_msg_ul_data.frame].i;
                current_slot = e.e[trace_msg_ul_data.slot].i;

                // increase sync offset index if the current frame is different from the previous frame
                if ((current_frame != prev_frame) && nrecord_idx >= number_records) {
                  sync_offset_index++;
                }

                if (current_frame != prev_frame || current_slot != prev_slot) {
                  nrecord_idx++;
                  // Update previous frame and slot numbers
                  prev_frame = current_frame;
                  prev_slot = current_slot;
                }

              } // End of start recording flag
            } // end of if statement for the supported messages based on Data Collection Trace Messages Structure
            else {
              printf("ERROR: Requested Message is not based on Data Collection Trace Messages Structure\n");
              printf("ERROR: Requested Message ID: %d\n", e.type);
            }
          } // end of if statement for the requested messages
        } // end while loop of reading events
        else {
          // No data, just loop and check time
          usleep(50); // optional: avoid busy-waiting
        }
      } // End of while loop to read events

      // de-activate the traces in the nr-softmodem
      printf("\n  De-activating tracer messages\n");
      for (i = 0; i < num_req_tracer_msgs; i++) {
        char *on_off_name = event_name_from_id(database, req_tracer_msgs_indices[i]);
        on_off(database, on_off_name, is_on, 0);
      }
      activate_traces(socket, number_of_events, is_on);
      printf("  De-activated traces\n");
      // Get the end time
      struct timespec end_time = get_current_time();
      // Calculate the time difference
      double time_diff = calculate_time_difference(start_time, end_time);
      printf("  Total time: %.2f ms\n", time_diff);
      printf("  Time per record: %.2f ms\n", time_diff / (number_records + max_sync_offset));

      //-----------------------------
      // discard stale or previous record data for the first DISCARD_RECORD_DURATION_MS
      //-----------------------------
      struct timespec record_start, record_now;
      clock_gettime(CLOCK_MONOTONIC, &record_start);

      while (1) {
        clock_gettime(CLOCK_MONOTONIC, &record_now);
        double elapsed_ms = calculate_time_difference(record_start, record_now);
        if (elapsed_ms >= DISCARD_RECORD_DURATION_MS) {
          break; // Stop after 10ms
        }
        int poll_ret = poll(&event_poll_fd, 1, 1); // 1 ms timeout for poll
        if (poll_ret > 0 && (event_poll_fd.revents & POLLIN)) {
          event e = get_event(socket, &ebuf, database);
          printf("%d", e.type);
          if (e.type == -1) {
            printf("\n Link broken or unexpected message received. Re-establishing connection...\n");
            reestablish_connection(&socket, ip, port, number_of_events, is_on);
            continue;
          }
        } else {
          // No data, just loop and check time
          usleep(50); // optional: avoid busy-waiting
        }
      } // End of while loop to discard stale records
    }
  } // End of while loop for record/stop

  free_database(database);
  free(is_on);
  close(socket);

  return 0;
}
