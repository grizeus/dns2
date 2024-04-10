#ifndef DNS_PARSER_H
#define DNS_PARSER_H

#include <stdint.h>
#include "binary_string.h"

typedef struct dns_header dns_header_t;

struct dns_header {
    uint16_t id;
    uint16_t flags;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
};

char* parse_query(char* buffer, int payload_len, uint32_t* query_hash, uint32_t* client_hash);
void parse_responce(char* buffer, int payload_len, binary_string_t* answer, uint32_t* query_hash, uint32_t* client_hash);

#endif // DNS_PARSER_H
