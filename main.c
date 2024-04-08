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
#include "utility.h"
#include "binary_string.h"
#include "query_record.h"
#include "file_parser.h"
#include "dns_parser.h"
#include "linked_list.h"
#include "map.h"

int main(int argc, char** argv)
{
    map_t clients;
    server_config_t config;
    char** black_list;
    initialize("file.ini", black_list, &config);

    int sockfd, dns_sockfd;
    struct sockaddr_in server_addr, dns_addr, *client_addr;

    setup_sockets(&sockfd, &dns_sockfd, &server_addr, client_addr, &dns_addr, &config);

    printf("Server listening on port %d...\n", config.port);

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
        if ((message = receive_from(sockfd, client_addr, &recv_cl_len)) != NULL) {

            char* dns_name = parse_query(message, recv_cl_len, &id, &query);
            if (in_list(dns_name, black_list))
            {
                send_to(sockfd, "Error", 6, client_addr);
                continue;
            }
            if (send_to(dns_sockfd, message, recv_cl_len, &dns_addr)) {
                list_t* new_record = create_record(client_addr, query, recv_cl_len);
                map_add(&clients, id, new_record, list_add_node);
            }
            is_received = 1;
        }
        binary_string_t response_query;
        if (message == receive_from(dns_sockfd, client_addr, &recv_cl_len))
        {
            parse_responce(message, recv_cl_len, &id, &response_query);
            list_t* result = map_find(&clients, id);
            client_addr = get_address(result, &response_query);
            if (send_to(sockfd, message, recv_cl_len, client_addr)) {
                map_delete(&clients, id, client_remover, &response_query, NULL);
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