#ifndef DNS_PARSER_H
#define DNS_PARSER_H

#include "binary_string.h"
#include <stdint.h>

typedef struct dns_header dns_header_t;
typedef struct query_data query_data_t;
typedef struct response_data response_data_t;

struct dns_header {
  uint16_t id;
  uint16_t flags;
  uint16_t qdcount;
  uint16_t ancount;
  uint16_t nscount;
  uint16_t arcount;
};

struct query_data {
  char *dns_name;
  uint32_t question_hash;
  uint32_t client_hash;
};

struct response_data {
  binary_string_t* answer;
  uint32_t question_hash;
  uint32_t client_hash;
};

query_data_t parse_query(binary_string_t *payload);
response_data_t parse_response(binary_string_t *payload);

#endif // DNS_PARSER_H
