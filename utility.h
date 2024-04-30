#ifndef UTILITY_H
#define UTILITY_H

#include "binary_string.h"
#include "file_parser.h"
#include "linked_list.h"
typedef struct {
  char *upstream_name;
  char *local_address;
} server_config_t;

void setup_sockets(int *sockfd, int *dns_sockfd,
                   struct sockaddr_in *server_addr,
                   struct sockaddr_in *client_addr,
                   struct sockaddr_in *dns_addr, server_config_t *config);
char *build_response(char *initial_query, size_t query_size,
                     binary_string_t *answer, size_t *new_size);
char *generate_blocked_response(const char *blocked_domain);
#endif // UTILITY_H
