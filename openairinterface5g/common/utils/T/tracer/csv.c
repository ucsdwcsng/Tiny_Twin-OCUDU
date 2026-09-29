/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"
#include "event.h"
#include "handler.h"
#include "configuration.h"

#define DEFAULT_IP   "127.0.0.1"
#define DEFAULT_PORT 2021

#define TIMESTAMP_FIELD -1

typedef struct {
  int *fields;
  enum event_arg_type *type;
  int count;
} ev_data;

int do_flush = 0;
char *separator = ",";

void csv(void *_d, event e)
{
  ev_data *d = _d;
  for (int i = 0; i < d->count; i++) {
    if (d->fields[i] == TIMESTAMP_FIELD) {
      struct tm *t = localtime(&e.sending_time.tv_sec);
      printf("%2.2d:%2.2d:%2.2d.%6.6ld",
             t->tm_hour, t->tm_min, t->tm_sec, e.sending_time.tv_nsec / 1000);
    } else switch (d->type[i]) {
      case EVENT_INT: printf("%d", e.e[d->fields[i]].i); break;
      case EVENT_ULONG: printf("%ld", e.e[d->fields[i]].ul); break;
      case EVENT_FLOAT: printf("%g", e.e[d->fields[i]].f); break;
      case EVENT_STRING: printf("%s", e.e[d->fields[i]].s); break;
      default: fprintf(stderr, "fatal: unsupported type\n"); exit(1);
    }
    if (i != d->count - 1)
      printf("%s", separator);
  }
  printf("\n");
  if (do_flush)
    fflush(stdout);
}

void usage(void)
{
  printf(
    "usage:\n"
    "    [options] <event name> <event field 1> ... <event field n>\n"
    "options:\n"
    "    -d <database file>        this option is mandatory\n"
    "    -ip <IP address>          tracee's IP address (default %s)\n"
    "    -p <port>                 tracee's port (default %d)\n"
    "    -f                        flush stdout after each printed line\n"
    "    -s <separator (default ,)>\n"
    "    -t <timestamp name>       set timestamp name, use it as <event field x>\n",
    DEFAULT_IP,
    DEFAULT_PORT
  );
  exit(1);
}

int main(int n, char **v)
{
  char *database_filename = NULL;
  void *database;
  event_handler *h;
  char *ip = DEFAULT_IP;
  int port = DEFAULT_PORT;
  char *timestamp_field_name = NULL;

  char *event_name = NULL;
  char **event_fields = NULL;
  int count = 0;

  for (int i = 1; i < n; i++) {
    if (!strcmp(v[i], "-h") || !strcmp(v[i], "--help")) usage();
    if (!strcmp(v[i], "-d"))         { if(i>n-2)usage(); database_filename = v[++i];   continue; }
    if (!strcmp(v[i], "-ip"))        { if(i>n-2)usage(); ip = v[++i];                  continue; }
    if (!strcmp(v[i], "-p"))         { if(i>n-2)usage(); port = atoi(v[++i]);          continue; }
    if (!strcmp(v[i], "-f"))         { do_flush = 1;                                   continue; }
    if (!strcmp(v[i], "-s"))         { if(i>n-2)usage(); separator = v[++i];           continue; }
    if (!strcmp(v[i], "-t"))         { if(i>n-2)usage(); timestamp_field_name= v[++i]; continue; }
    if (!event_name) { event_name = v[i]; continue; }
    count++;
    event_fields = realloc(event_fields, count * sizeof(char *)); if (!event_fields) abort();
    event_fields[count - 1] = v[i];
  }

  if (event_name == NULL || count == 0) {
    fprintf(stderr, "fatal: provide an event and the fields of this event you want to trace\n");
    usage();
  }

  ev_data d = { 0 };
  d.count = count;
  d.fields = malloc(count * sizeof(int)); if (!d.fields) abort();
  d.type = malloc(count * sizeof(enum event_arg_type)); if (!d.type) abort();

  if (database_filename == NULL) {
    printf("ERROR: provide a database file (-d)\n");
    printf("use -h for help on usage\n");
    exit(1);
  }

  database = parse_database(database_filename);
  load_config_file(database_filename);
  h = new_handler(database);

  /* setup the trace - event name and event fields */
  int ev_id = event_id_from_name(database, event_name);
  database_event_format f = get_format(database, ev_id);
  for (int i = 0; i < count; i++) {
    int ev;
    if (timestamp_field_name && !strcmp(event_fields[i], timestamp_field_name)) {
      d.fields[i] = TIMESTAMP_FIELD;
      continue;
    }
    for (ev = 0; ev < f.count; ev++)
      if (!strcmp(f.name[ev], event_fields[i]))
        break;
    if (ev == f.count) {
      fprintf(stderr, "fatal: field '%s' not found in event '%s'\n", event_fields[i], event_name);
      exit(1);
    }
    d.fields[i] = ev;
    if (!strcmp(f.type[ev], "int")) {
      d.type[i] = EVENT_INT;
    } else if (!strcmp(f.type[ev], "ulong")) {
      d.type[i] = EVENT_ULONG;
    } else if (!strcmp(f.type[ev], "float")) {
      d.type[i] = EVENT_FLOAT;
    } else if (!strcmp(f.type[ev], "string")) {
      d.type[i] = EVENT_STRING;
    } else {
      fprintf(stderr, "fatal: field '%s': unsupported field type '%s'\n", f.name[ev], f.type[ev]);
      exit(1);
    }
  }

  /* connect to tracee, activate wanted trace */
  int in = connect_to(ip, port);

  char mt = 1;
  int  number_of_events = number_of_ids(database);
  int *is_on = calloc(number_of_events, sizeof(int));

  if (is_on == NULL) {
    printf("ERROR: out of memory\n");
    exit(1);
  }

  on_off(database, event_name, is_on, 1);

  /* activate selected trace */
  if (socket_send(in, &mt, 1) == -1 ||
      socket_send(in, &number_of_events, sizeof(int)) == -1 ||
      socket_send(in, is_on, number_of_events * sizeof(int)) == -1) {
    printf("ERROR: socket_send failed\n");
    exit(1);
  }

  free(is_on);

  register_handler_function(h, ev_id, csv, &d);

  OBUF ebuf = {.osize = 0, .omaxsize = 0, .obuf = NULL};

  for (int i = 0; i < count; i++) {
    printf("%s", event_fields[i]);
    if (i != count - 1)
      printf("%s", separator);
  }
  printf("\n");
  if (do_flush)
    fflush(stdout);

  /* read messages */
  while (1) {
    event e;
    e = get_event(in, &ebuf, database);
    if (e.type == -1) break;
    handle_event(h, e);
  }

  return 0;
}
