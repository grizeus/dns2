#ifndef DNS_PARSER_H
#define DNS_PARSER_H

#include <stdint.h>
#include "binary_string.h"

typedef struct dns_header dns_header_t;
typedef struct query_data query_data_t;

struct dns_header {
    uint16_t id;
    uint16_t flags;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
};

struct query_data {
    char* dns_name;
    uint32_t question_hash;
    uint32_t client_hash;
};

query_data_t parse_query(char* buffer, int payload_len);
void parse_response(char* buffer, int payload_len, binary_string_t* answer, uint32_t* question_hash, uint32_t* client_hash);

#endif // DNS_PARSER_H
