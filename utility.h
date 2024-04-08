#ifndef UTILITY_H
#define URILITY_H

#include "linked_list.h"
#include "binary_string.h"
#include "file_parser.h"

void setup_sockets(int* sockfd, int* dns_sockfd, struct sockaddr_in* server_addr, struct sockaddr_in* client_addr, struct sockaddr_in* dns_addr, server_config_t* config);
list_t* create_record(const struct sockaddr_in* address, const uint8_t* query, size_t size);
struct sockaddr_in* get_address(list_t* head, binary_string_t* key);
void client_remover(list_t** head, binary_string_t* query);
#endif // UTILITY_H