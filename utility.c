#include <arpa/inet.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "utility.h"
#include "binary_string.h"

#define PORT 53
#define DNS_PORT 53

static void query_setup(uint8_t query[]);

void setup_sockets(int *sockfd, int *dns_sockfd,
                   struct sockaddr_in *server_addr,
                   struct sockaddr_in *client_addr,
                   struct sockaddr_in *dns_addr, server_config_t *config) {

  if ((*sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("Socket creation failed");
    exit(1);
  }

  if ((*dns_sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    perror("DNS socket creation failed");
    exit(1);
  }

  memset(server_addr, 0, sizeof(*server_addr));
  memset(client_addr, 0, sizeof(*client_addr));
  memset(dns_addr, 0, sizeof(*dns_addr));

  server_addr->sin_family = AF_INET;
  server_addr->sin_port = htons(PORT);
  inet_pton(AF_INET, config->local_address, &server_addr->sin_addr);

  dns_addr->sin_family = AF_INET;
  dns_addr->sin_port = htons(DNS_PORT);
  inet_pton(AF_INET, config->upstream_name, &dns_addr->sin_addr);

  if (bind(*sockfd, (const struct sockaddr *)server_addr,
           sizeof(*server_addr)) < 0) {
    perror("Binding failed");
    exit(1);
  }
  puts("Sockets setup is done");
}

binary_string_t build_response(binary_string_t* query,
                     binary_string_t *answer) {

  binary_string_t new_response = { NULL, 0};
  if (query->size == 0 || answer->size == 0) {
    return new_response;
  }
  // allocate memory for response
  size_t size = query->size + answer->size;
  uint8_t *response = (uint8_t  *)malloc(size);
  if (response == NULL) {
    return new_response;
  }
  query_setup(query->data);
  memcpy(response, query->data, query->size);
  memcpy(response + query->size, answer->data, answer->size);

  new_response.data = response;
  new_response.size = size;

  return new_response;
}

static void query_setup(uint8_t query[]) {

  // change flags to 8180(standard response, no error)
  query[2] = 0x81;
  query[3] = 0x80;

  // change answer count to 1
  query[7] = 0x01;
}

binary_string_t generate_blocked_response(const char *blocked_domain) {

  char *str = (char *)malloc(1024 * sizeof(char));

  if (!str) {
    perror("Memory allocation failed");
    exit(1);
  }

  snprintf(str, 1024, "\
HTTP/1.1 200 OK\r\n\
Content-Type: text/html\r\n\
\r\n\
<!DOCTYPE html>\n\
<html>\n\
<head>\n\
<title>Blocked Domain</title>\n\
<style>\n\
    body {\n\
        font-family: Arial, sans-serif;\n\
        margin: 20px;\n\
    }\n\
    .container {\n\
        max-width: 600px;\n\
        margin: auto;\n\
        text-align: center;\n\
    }\n\
    h1 {\n\
        color: #FF0000;\n\
    }\n\
</style>\n\
</head>\n\
<body>\n\
    <div class=\"container\">\n\
        <h1>Domain Blocked</h1>\n\
        <p>The domain <strong>%s</strong> you are trying to access is blocked due to being on a blacklist.</p>\n\
        <p>Please contact your network administrator for further assistance.</p>\n\
    </div>\n\
</body>\n\
</html>", blocked_domain);
  binary_string_t blocked_response = {(uint8_t*)str, 1024};
  return blocked_response;
}
