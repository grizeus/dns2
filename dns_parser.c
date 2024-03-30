#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dns_parser.h"

#define MAX_DOMAIN_LEN 255
#define QTYPE_SIZE     2
#define QCLASS_SIZE    2

char* parse_query(const char* buffer, int payload_len, uint16_t* id, uint8_t** query) {

    const uint8_t* dns_payload = (const uint8_t*)buffer;

    dns_header_t* dns_header = (dns_header_t*)dns_payload;
    *id = ntohs(dns_header->id);
    int query_len = payload_len - sizeof(dns_header_t);
    if ((ntohs(dns_header->flags) & 0x8000) == 0) {

        const uint8_t* question = dns_payload + sizeof(dns_header_t);
        char* extracted = malloc(query_len);

        if (extracted == NULL) {
            perror("Memory allocation failed");
            return NULL;
        }

        int domain_ind = 0;
        int i = 0;
        
        while (i < payload_len) {
            int label_len = question[i];

            if (domain_ind + label_len + 1 > MAX_DOMAIN_LEN) {
                fprintf(stderr, "Domain length exceeds maximum allowed\n");
                free(extracted);
                return NULL;
            }

            for (int j = 1; j <= label_len; ++j) {
                extracted[domain_ind++] = (unsigned char)question[i + j];
            }

            if (label_len > 0) {
                extracted[domain_ind++] = '.';
            } else {
                break;
            }

            i += label_len + 1;
        }

        if (domain_ind > 0) {
            extracted[domain_ind - 1] = '\0'; // Null-terminate the string
        } else {
            fprintf(stderr, "No domain extracted\n");
            free(extracted);
            return NULL;
        }
        *query = malloc(query_len);
        memcpy(*query, question, query_len);

        return extracted;
    }

    return NULL;
}

// void parse_responce(const char* buffer, int payload_len, uint16_t* id, uint8_t** query, int* query_len) {

//     const uint8_t* dns_payload = (const uint8_t*)buffer;
//     dns_header_t* dns_header = (dns_header_t*)dns_payload;
//     *id = ntohs(dns_header->id);

//     if ((ntohs(dns_header->flags) & 0x8000) != 0) {

//         const uint8_t* query_resp = dns_payload + sizeof(struct dns_header);
//         int i = 0;
//         *query_len = 0; // initialize
//         *query = malloc(payload_len - sizeof(struct dns_header) + 1);
//         while (i < payload_len) {
//             int label_len = query_resp[i];

//             if (label_len == 0) {
//                 *query_len += QTYPE_SIZE + QCLASS_SIZE + 1;
//                 break;
//             }
//             i += label_len + 1;
//             *query_len += label_len + 1;
//         }
//         memcpy(*query, query_resp, *query_len);
//     }
// }

void parse_responce(const char* buffer, int payload_len, uint16_t* id, binary_string_t* query) {

    const uint8_t* dns_payload = (const uint8_t*)buffer;
    dns_header_t* dns_header = (dns_header_t*)dns_payload;
    *id = ntohs(dns_header->id);

    if ((ntohs(dns_header->flags) & 0x8000) != 0) {

        const uint8_t* query_resp = dns_payload + sizeof(struct dns_header);
        int i = 0;
        size_t query_len = 0; // initialize
        query->data = malloc(payload_len - sizeof(struct dns_header) + 1);
        while (i < payload_len) {
            int label_len = query_resp[i];

            if (label_len == 0) {
                query_len += QTYPE_SIZE + QCLASS_SIZE + 1;
                break;
            }
            i += label_len + 1;
            query_len += label_len + 1;
        }
        memcpy(query->data, query_resp, query_len);
        query->size = query_len;
    }
}
