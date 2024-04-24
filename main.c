#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "binary_string.h"
#include "communicate.h"
#include "file_parser.h"
#include "utility.h"
#include "dns_parser.h"
#include "map.h"

#define SLEEP_INTERVAL_US 1000

int main(int argc, char** argv) {

    if (argc != 2) {
        printf("Program must run with %s local_address\n", argv[0]);
        return 1;
    }
    // initialize addresses
    init_data_t init_data = initialize("config.ini");
    if (init_data.upstream == NULL || init_data.black_list == NULL) {
        perror("Configuration is failed. Error with 'config.ini' occured");
        puts("Please check for 'config.ini' in directory along with the executable binary");
        exit(1);
    }

    server_config_t config = {
        .local_address = argv[1],
        .upstream_name = init_data.upstream
    };

    map_t* clients = map_create();
    map_t* lookup = map_create();

    int sockfd, dns_sockfd;
    struct sockaddr_in server_addr, dns_addr, client_addr;

    setup_sockets(&sockfd, &dns_sockfd, &server_addr, &client_addr, &dns_addr, &config);

    printf("Server listening...\n");

    // Set the socket to non-blocking mode
    if (fcntl(sockfd, F_SETFL, O_NONBLOCK) == -1) {
        perror("Failed to set non-blocking mode");
        exit(1);
    }

    while (1) {

        ssize_t recv_len;
        uint32_t question_hash;
        uint32_t client_hash;
        binary_string_t* answer = {0};

        char* recv_message = receive_from(sockfd, &client_addr, &recv_len);
        if (!recv_message) {
            usleep(SLEEP_INTERVAL_US);
            continue;
        }
        query_data_t query_data  = parse_query(recv_message, recv_len);
        if (!query_data.dns_name) {
            usleep(SLEEP_INTERVAL_US);
            continue;
        }
        printf("DNS %s is received\n", query_data.dns_name);
        if (in_list(query_data.dns_name, init_data.black_list)) {
            printf("A request has been made for a blocked %s resource\n", query_data.dns_name);
            char* blocked_response = generate_blocked_response(query_data.dns_name);
            send_to(sockfd, blocked_response, strlen(blocked_response), &client_addr);
            free(query_data.dns_name);
            continue;
        }

        free(query_data.dns_name);

        // get real data from map (answer), DO NOT FREE!
        answer = map_find(lookup, query_data.question_hash);

        if (!answer) {
            puts("send to upstream");
            send_to(dns_sockfd, recv_message, recv_len, &dns_addr);
            map_add(clients, query_data.client_hash, &client_addr, NULL);
        } else {
            size_t response_len;
            char* new_response = build_response(recv_message, recv_len, answer, &response_len);
            send_to(sockfd, new_response, response_len, &client_addr);
            free(new_response);
            continue;
        }

        answer = (binary_string_t*)malloc(sizeof(binary_string_t));
        recv_message = receive_from(dns_sockfd, &dns_addr, &recv_len);
        if (recv_len < 0) {
            usleep(SLEEP_INTERVAL_US);
            continue;
        }

        parse_response(recv_message, recv_len, answer, &question_hash, &client_hash);
        // search in the map for the client's address to send
        struct sockaddr_in* result = map_find(clients, client_hash);
        if (result){
            client_addr = *result;
        } else {
            perror("Address not found");
            continue;
        }
        if (send_to(sockfd, recv_message, recv_len, &client_addr)) {
            // add actual response to map(DO NOT FREE THIS HERE)
            map_add(lookup, question_hash, answer, NULL);
            map_delete(clients, client_hash, NULL, NULL, NULL);
        }
    }
    return 0;
}
