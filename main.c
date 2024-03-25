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
#include "string.h"
#include "file_parser.h"
#include "dns_parser.h"
#include "map.h"

#define DNS_PORT       53
#define PORT           8888

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
                save_client(&clients, id, &client_addr, query);
            }
            is_received = 1;
        }
        int query_len;
        if (message == receive_from(dns_sockfd, &client_addr, &recv_cl_len))
        {
            parse_responce(message, recv_cl_len, &id, &query, &query_len);
            client_addr = *get_client(&clients, id, query, query_len);
            if (send_to(sockfd, message, recv_cl_len, &client_addr))
                remove_client(&clients, id, query, query_len);
            is_received = 1;
        }
        if (!is_received)
        {
            usleep(10000);
        }
    }
    return 0;
}
