#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "utility.h"
#include "query_record.h"

void setup_sockets(int* sockfd, int* dns_sockfd,
                    struct sockaddr_in* server_addr,
                    struct sockaddr_in* client_addr,
                    struct sockaddr_in* dns_addr,
                    server_config_t* config) {
    
    if ((*sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(1);
    }
    
    if ((*dns_sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        perror("DNS socket creation failed");
        exit(1);
    }
    
    memset(server_addr, 0, sizeof(server_addr));
    memset(client_addr, 0, sizeof(client_addr));
    memset(dns_addr, 0, sizeof(dns_addr));

    server_addr->sin_family = AF_INET;
    server_addr->sin_port = htons(config->port);
    inet_pton(AF_INET, config->local_address, &(server_addr->sin_addr));

    dns_addr->sin_family = AF_INET;
    dns_addr->sin_port = htons(config->dns_port);
    inet_pton(AF_INET, config->upstream_name, &(dns_addr->sin_addr));

    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Binding failed");
        exit(1);
    }
}

list_t* create_record(const struct sockaddr_in* address, const uint8_t* query, size_t size) {
    record_t* new_record = (record_t*)malloc(sizeof(record_t));
        if (new_record == NULL) {
            return NULL;
        }
        
        new_record->address = *address;
        new_record->query = binary_string_create(query, size);
        list_t* new_list = list_new(new_record);

        return new_list;
}
struct sockaddr_in* get_address(list_t* head, binary_string_t* key) {

    // if list have only one node
    if (head->next == NULL) {
        record_t* rec_ptr = head->data;
        if((key->size == rec_ptr->query.size) &&
                memcmp(key->data, rec_ptr->query.data, key->size) == 0) {
            return &rec_ptr->address;
        }
        return NULL;
    }

    list_t* searching_node = list_find(head, key, compare_record);
    return &((record_t*)searching_node->data)->address;
}

void client_remover(list_t** head, binary_string_t* query) {

    list_delete(head, query, compare_record);
}