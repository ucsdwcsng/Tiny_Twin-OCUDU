#ifndef JBPF_SRSRAN_DEFS_H_
#define JBPF_SRSRAN_DEFS_H_

#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define JBPF_NUM_OF_LCG       8
#define JBPF_MAX_UE_SUPPORTED (16*5)

struct output_socket
{
    int sockfd;
    struct sockaddr_in server_addr;
};

#endif