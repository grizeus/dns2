#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "binary_string.h"
#include "communicate.h"
#include "dns_parser.h"
#include "file_parser.h"
#include "map.h"
#include "utility.h"

#define MAX_BUFF_SIZE 1024
#define SLEEP_INTERVAL_US 1000

int main(int argc, char **argv) {

  if (argc != 2) {
    printf("Program must run with %s local_address\n", argv[0]);
    return 1;
  }
  // initialize addresses
  init_data_t init_data = initialize("config.ini");
  if (init_data.upstream == NULL || init_data.black_list == NULL) {
    perror("Configuration is failed. Error with 'config.ini' occured");
    puts("Please check for 'config.ini' in directory along with the executable "
         "binary");
    exit(1);
  }

  server_config_t config = {.local_address = argv[1],
                            .upstream_name = init_data.upstream};

  map_t *clients = map_create();
  map_t *lookup = map_create();

  int sockfd, dns_sockfd;
  struct sockaddr_in server_addr, dns_addr, client_addr;

  setup_sockets(&sockfd, &dns_sockfd, &server_addr, &client_addr, &dns_addr,
                &config);

  printf("Server listening...\n");

  // Set the socket to non-blocking mode
  if (fcntl(sockfd, F_SETFL, O_NONBLOCK) == -1) {
    perror("Failed to set non-blocking mode");
    exit(1);
  }

  while (1) {

    // ssize_t recv_len;
    char buffer[MAX_BUFF_SIZE];
    binary_string_t *answer = {0};
    binary_string_t recv_message = {0};

    recv_message = receive_from(sockfd, &client_addr, buffer);
    if (!recv_message.data) {
      usleep(SLEEP_INTERVAL_US);
      continue;
    }
    query_data_t query_data = parse_query(&recv_message);
    if (!query_data.dns_name) {
      usleep(SLEEP_INTERVAL_US);
      continue;
    }
    printf("DNS %s is received\n", query_data.dns_name);
    if (in_list(query_data.dns_name, init_data.black_list)) {
      printf("A request has been made for a blocked %s resource\n",
             query_data.dns_name);
      binary_string_t blocked_response =
          generate_blocked_response(query_data.dns_name);
      send_to(sockfd, &blocked_response, &client_addr);

      binary_string_destroy(&blocked_response);
      binary_string_destroy(&recv_message);
      free(query_data.dns_name);
      continue;
    }

    free(query_data.dns_name);

    // get real data from map (answer), DO NOT FREE!
    answer = map_find(lookup, query_data.question_hash);

    if (!answer) {
      puts("send to upstream");
      send_to(dns_sockfd, &recv_message, &dns_addr);
      binary_string_destroy(&recv_message);
      map_add(clients, query_data.client_hash, &client_addr, NULL);
    } else {
      binary_string_t new_response = build_response(recv_message.data, recv_message.size, answer);
      send_to(sockfd, &new_response, &client_addr);
      binary_string_destroy(&new_response);
      binary_string_destroy(&recv_message);
      continue;
    }

    recv_message = receive_from(dns_sockfd, &dns_addr, buffer);
    if (recv_message.size < 0) {
      usleep(SLEEP_INTERVAL_US);
      continue;
    }
    response_data_t response_data = parse_response(&recv_message);
    struct sockaddr_in *result = map_find(clients, response_data.client_hash);
    if (result) {
      client_addr = *result;
    } else {
      perror("Address not found");
      binary_string_destroy(&recv_message);
      continue;
    }
    if (send_to(sockfd, &recv_message, &client_addr)) {
      // add actual response to map(DO NOT FREE THIS HERE)
      map_add(lookup, response_data.question_hash, response_data.answer, NULL);
      map_delete(clients, response_data.client_hash, NULL, NULL, NULL);
      binary_string_destroy(&recv_message);
    }
  }

  return 0;
}
