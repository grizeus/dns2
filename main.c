#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include "binary_string.h"
#include "communicate.h"
#include "utility.h"
#include "dns_parser.h"
#include "map.h"

#define SLEEP_INTERVAL_US 1000

int main(int argc, char** argv) {

    if (argc != 3) {
        printf("Program must run with %s local_address upstream\n", argv[0]);
        return 1;
    }
    // initialize addresses
    server_config_t config = {
        .local_address = argv[1],
        .upstream_name = argv[2]
    };

    map_t* clients = map_create();
    map_t* lookup = map_create();
    char** black_list = NULL;
    initialize_black_list("config.ini", &black_list);

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
        uint32_t dns_id;
        uint32_t client_id_in;
        uint32_t client_id_out;
        char* recv_message;
        binary_string_t* answer = {0};

        recv_message = receive_from(sockfd, &client_addr, &recv_len);
        if (!recv_message) {
                usleep(SLEEP_INTERVAL_US);
                continue;
        }
        char* dns_name = parse_query(recv_message, recv_len, &dns_id, &client_id_in);
        if (!dns_name) {
                usleep(SLEEP_INTERVAL_US);
                continue;
        }

        if (in_list(dns_name, black_list)) {
            // TODO: maybe need to send some real shit
            send_to(sockfd, "Error", 6, &client_addr);
            free(dns_name);
            continue;
        }

        free(dns_name);

        // get real data from map (answer), DO NOT FREE!
        answer = map_find(lookup, dns_id);

        if (!answer) {
            puts("send to upstream");
            send_to(dns_sockfd, recv_message, recv_len, &dns_addr);
            map_add(clients, client_id_in, &client_addr, NULL);
            free(recv_message);
        } else {
            size_t response_len;
            char* new_response = build_response(recv_message, recv_len, answer, &response_len);
            send_to(sockfd, new_response, response_len, &client_addr);
            free(new_response);
            free(recv_message);
            continue;
        }

        answer = (binary_string_t*)malloc(sizeof(binary_string_t));
        recv_message = receive_from(dns_sockfd, &dns_addr, &recv_len);
        if (recv_len < 0) {
            usleep(SLEEP_INTERVAL_US);
            continue;
        }

        parse_response(recv_message, recv_len, answer, &dns_id, &client_id_out);
        // search in the map for the client's address to send
        struct sockaddr_in* result = map_find(clients, client_id_out);
        if (result){
            client_addr = *result;
        } else {
            perror("Address not found");
            continue;
        }
        if (send_to(sockfd, recv_message, recv_len, &client_addr)) {
            // add actual response to map(DO NOT FREE THIS HERE)
            map_add(lookup, dns_id, answer, NULL);
            map_delete(clients, client_id_out, NULL, NULL, NULL);
        }
    }
    return 0;
}
