#include <arpa/inet.h>
#include <stdint.h>
#include <stdio.h>
#include <netinet/in.h>
#include <linux/if_packet.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "binary_string.h"
#include "communicate.h"
#include "utility.h"
#include "dns_parser.h"
#include "map.h"

int main(int argc, char** argv) {

    map_t* clients = map_create();
    map_t* lookup = map_create();
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
        uint32_t dns_id;
        uint32_t client_id;
        uint8_t* query;

        if ((message = receive_from(sockfd, client_addr, &recv_cl_len)) != NULL) {

            char* dns_name = parse_query(message, recv_cl_len, &dns_id, &client_id);
            if (in_list(dns_name, black_list)) {
                send_to(sockfd, "Error", 6, client_addr);
                free(dns_name);
                continue;
            }
            free(dns_name);
            // 1. search in cache for match query with responce
            // 2. if success, return response to client
            // 3. if not, send to upstream, return response from upstream and save into cache table
            binary_string_t* answer = map_find(lookup, dns_id);
            if (!answer) {
                if (send_to(dns_sockfd, message, recv_cl_len, &dns_addr)) {
                    map_add(clients, client_id, client_addr, NULL);
                    free(message);
                }
            } else {
                size_t response_len;
                char* response = build_response(message, recv_cl_len, answer, &response_len);
                send_to(sockfd, response, response_len, client_addr);
                free(response);
                free(message);
            }
            is_received = 1;
        }
        binary_string_t response;
        if (message == receive_from(dns_sockfd, client_addr, &recv_cl_len)) {
            parse_responce(message, recv_cl_len, &response, &dns_id, &client_id);
            client_addr = map_find(clients, client_id);
            if (send_to(sockfd, message, recv_cl_len, client_addr)) {
                map_add(lookup, dns_id, &response, NULL);
                map_delete(clients, client_id, NULL, NULL, NULL);
            }
            is_received = 1;
        }
        if (!is_received) {
            usleep(10000);
        }
    }
    return 0;
}
