#ifndef UTILITY_H
#define UTILITY_H

#include "binary_string.h"

typedef struct {
  char *upstream_name;
  char *local_address;
} server_config_t;

void setup_sockets(int *sockfd, int *dns_sockfd,
                   struct sockaddr_in *server_addr,
                   struct sockaddr_in *client_addr,
                   struct sockaddr_in *dns_addr, server_config_t *config);
binary_string_t build_response(binary_string_t* query,
                               binary_string_t *answer);
binary_string_t generate_blocked_response(const char *blocked_domain);
#endif // UTILITY_H
