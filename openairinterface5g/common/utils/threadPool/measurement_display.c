/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
*/

#include <common/utils/simple_executable.h>
#include "notified_fifo.h"

#define SEP "\t"

uint64_t cpuCyclesMicroSec;

int main(int argc, char *argv[]) {
  if(argc != 2) {
    printf("Need one parameter: the trace Linux pipe (fifo)");
    exit(1);
  }

  mkfifo(argv[1],0666);
  int fd=open(argv[1], O_RDONLY);

  if ( fd == -1 ) {
    perror("open read mode trace file:");
    exit(1);
  }

  uint64_t deb=rdtsc_oai();
  usleep(100000);
  cpuCyclesMicroSec=(rdtsc_oai()-deb)/100000;
  printf("Cycles per µs: %lu\n",cpuCyclesMicroSec);
  printf("Key" SEP "delay to process" SEP "processing time" SEP "delay to be read answer\n");
  notifiedFIFO_elt_t doneRequest;

  while ( 1 ) {
    if ( read(fd,&doneRequest, sizeof(doneRequest)) ==  sizeof(doneRequest)) {
      printf("%lu" SEP "%llu" SEP "%llu" SEP "%llu" "\n",
             doneRequest.key,
             (doneRequest.startProcessingTime-doneRequest.creationTime)/cpuCyclesMicroSec,
             (doneRequest.endProcessingTime-doneRequest.startProcessingTime)/cpuCyclesMicroSec,
             (doneRequest.returnTime-doneRequest.endProcessingTime)/cpuCyclesMicroSec
            );
    } else {
      printf("no measurements\n");
      sleep(1);
    }
  }
}
