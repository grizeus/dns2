#ifndef DNS_PARSER_H
#define DNS_PARSER_H

#include <stdint.h>
#include <sys/types.h>
#include <stdint.h>

#ifdef __FAVOR_BSD

    struct udphdr {
        uint16_t uh_sport;
        uint16_t uh_dport;
        uint16_t uh_ulen;
        uint16_t uh_sum;

    };
#else

    struct udphdr {
        uint16_t source;
        uint16_t dest;
        uint16_t len;
        uint16_t check;
    };

#endif // ifdef __FAVOR_BSD

struct DNSHeader {
    uint16_t id;
    uint16_t flags;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
};

char* extract_domain(const uint8_t* dnsPayload, int payload_len);

#endif // DNS_PARSER_H