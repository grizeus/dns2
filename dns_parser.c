#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dns_parser.h"

#define MAX_DOMAIN_LEN 255
#define QTYPE_SIZE     2
#define QCLASS_SIZE    2
#define ID_SIZE        2
#define FNV_PRIME      16777619
#define FNV_OFFSET     2166136261u

static uint32_t fnv1a_hash_func(const uint8_t* str) {

    uint32_t hash = FNV_OFFSET;
    while (*str) {
        hash ^= (uint32_t)*str++;
        hash ^= FNV_PRIME;
    }

    return hash;
}
// i need to retrieve hash value of dns name
char* parse_query(char* buffer, int payload_len, uint32_t* question_hash, uint32_t* client_hash) {

    uint8_t* dns_payload = (uint8_t*)buffer;

    dns_header_t* dns_header = (dns_header_t*)dns_payload;
    int question_len = payload_len - sizeof(dns_header_t);
    if ((ntohs(dns_header->flags) & 0x8000) == 0) {

        const uint8_t* question = dns_payload + sizeof(dns_header_t);
        char* extracted = malloc(question_len);

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
        // hash dns question
        *question_hash = fnv1a_hash_func(question);
        // create string with id + question
        uint8_t* id_w_question = (uint8_t*)malloc(question_len + ID_SIZE);
        // copy id first, and then all question
        memcpy(id_w_question, dns_payload, ID_SIZE);
        memcpy(id_w_question + ID_SIZE, question, question_len);

        // hash this to identificate our client (specific ID + question)
        *client_hash = fnv1a_hash_func(id_w_question);
        free(id_w_question);
        return extracted;
    }

    return NULL;
}

void parse_response(char* buffer, int payload_len, binary_string_t* answer, uint32_t* question_hash, uint32_t* client_hash) {

    uint8_t* dns_payload = (uint8_t*)buffer;
    dns_header_t* dns_header = (dns_header_t*)dns_payload;

    if ((ntohs(dns_header->flags) & 0x8000) != 0) {
        // question_answer is starting point of question and answer in response
        uint8_t* question_answer = dns_payload + sizeof(struct dns_header);
        int i = 0;
        size_t question_len = 0; // initialize
        // find length of question
        while (i < payload_len) {
            int label_len = question_answer[i];

            if (label_len == 0) {
                question_len += QTYPE_SIZE + QCLASS_SIZE + 1;
                break;
            }
            i += label_len + 1;
            question_len += label_len + 1;
        }
        uint8_t* question = (uint8_t*)malloc(question_len);
        memcpy(question, question_answer, question_len);
        *question_hash = fnv1a_hash_func(question);
        
        // create string with id + question
        uint8_t* id_w_question = (uint8_t*)malloc(question_len + ID_SIZE);
        // copy id first, and then all question
        memcpy(id_w_question, dns_payload, ID_SIZE);
        memcpy(id_w_question + ID_SIZE, question, question_len);

        // hash this to identificate our client (specific ID + question)
        *client_hash = fnv1a_hash_func(id_w_question);
        free(question);
        question = NULL;
        free(id_w_question);
        id_w_question = NULL;

        size_t answer_len = payload_len - (sizeof(struct dns_header) + question_len);
        // move pointer to begin of answer part
        uint8_t* answer_p = question_answer + question_len;
        // allocate memory for answer and copy it
        uint8_t* temp_str = (uint8_t*)malloc(answer_len);
        memcpy(temp_str, answer_p, answer_len);

        answer->data = temp_str;
        answer->size = answer_len;
    }
}
