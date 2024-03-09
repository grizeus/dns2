#include <stdlib.h>
#include "dns_parser.h"

char* extract_domain(const uint8_t* dnsPayload, int payload_len) {
    struct DNSHeader* dnsHeader = (struct DNSHeader*)dnsPayload;

    if ((ntohs(dnsHeader->flags) & 0x8000) == 0) {
        const uint8_t* question = dnsPayload + sizeof(struct DNSHeader);
        char* extractedDomain = malloc(payload_len);

        int i = 0;
        int domainind = 0;

        while (i < payload_len) {
            int labelLength = question[i];
            for (int j = 1; j <= labelLength; ++j) {
                extractedDomain[domainind++] = (unsigned char)question[i + j];
            }

            if (labelLength > 0) {
                extractedDomain[domainind++] = '.';
            } else {
                break;
            }

            i += labelLength + 1;
        }

        extractedDomain[domainind - 1] = '\0';

        return extractedDomain;
    }

    return NULL;
}
