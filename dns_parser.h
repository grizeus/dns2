#ifndef DNS_PARSER_H
#define DNS_PARSER_H

#include <stdint.h>

struct DNSHeader {
    uint16_t id;
    uint16_t flags;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
};

char* extract_domain(char* buffer, int payload_len);

#endif // DNS_PARSER_H