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

char* parse_query(const char* buffer, int payload_len, uint16_t* id, uint8_t** query);
// void parse_responce(const char* buffer, int payload_len, uint16_t* id, uint8_t** query, int* query_len);
void parse_responce(const char* buffer, int payload_len, uint16_t* id, binary_string_t* query);

#endif // DNS_PARSER_H
