#include <arpa/inet.h>
#include <stdint.h>
#include <stdio.h>
#include <netinet/in.h>
#include <linux/if_packet.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "communicate.h"
#include "binary_string.h"
#include "query_record.h"
#include "file_parser.h"
#include "dns_parser.h"
#include "linked_list.h"
#include "map.h"

#define DNS_PORT       53
#define PORT           8888

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

int main(int argc, char** argv)
{
    map_t clients;
    char* upstream_name;
    char** black_list;
    initialize("file.ini", black_list, upstream_name);

    int sockfd, dns_sockfd;
    struct sockaddr_in server_addr, client_addr, dns_addr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    if ((dns_sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        perror("DNS socket creation failed");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));
    memset(&dns_addr, 0, sizeof(dns_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    dns_addr.sin_family = AF_INET;
    dns_addr.sin_port = htons(DNS_PORT);
    inet_pton(AF_INET, upstream_name, &dns_addr.sin_addr);

    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Binding failed");
        exit(1);
    }

    printf("Server listening on port %d...\n", PORT);

    // Set the socket to non-blocking mode
    if (fcntl(sockfd, F_SETFL, O_NONBLOCK) == -1) {
        perror("Failed to set non-blocking mode");
        exit(1);
    }
    
    while (1)
    {
        ssize_t recv_cl_len;
        char* message;
        int is_received = 0;
        uint16_t id;
        uint8_t* query;
        if ((message = receive_from(sockfd, &client_addr, &recv_cl_len)) != NULL) {

            char* dns_name = parse_query(message, recv_cl_len, &id, &query);
            if (in_list(dns_name, black_list))
            {
                send_to(sockfd, "Error", 6, &client_addr);
                continue;
            }
            if (send_to(dns_sockfd, message, recv_cl_len, &dns_addr)) {
                list_t* new_record = create_record(&client_addr, query, recv_cl_len);
                map_add(&clients, id, new_record, list_add_node);
            }
            is_received = 1;
        }
        binary_string_t response_query;
        if (message == receive_from(dns_sockfd, &client_addr, &recv_cl_len))
        {
            parse_responce(message, recv_cl_len, &id, &response_query);
            list_t* result = map_find(&clients, id);
            client_addr = *get_address(result, &response_query);
            if (send_to(sockfd, message, recv_cl_len, &client_addr)) {
                map_delete(&clients, id, &response_query, client_remover);
            }
            is_received = 1;
        }
        if (!is_received)
        {
            usleep(10000);
        }
    }
    return 0;
}
