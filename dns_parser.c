#include <errno.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "binary_string.h"
#include "dns_parser.h"

#define MAX_DOMAIN_LEN 255
#define QTYPE_SIZE 2
#define QCLASS_SIZE 2
#define ID_SIZE 2
#define FNV_PRIME 16777619
#define FNV_OFFSET 2166136261u

static uint32_t fnv1a_hash_func(const uint8_t *str) {

  uint32_t hash = FNV_OFFSET;
  while (*str) {
    hash ^= (uint32_t)*str++;
    hash ^= FNV_PRIME;
  }

  return hash;
}

query_data_t parse_query(binary_string_t *payload) {

  query_data_t query_data = {NULL, 0, 0};
  dns_header_t *dns_header = (dns_header_t *)payload->data;
  int question_len = payload->size - sizeof(dns_header_t);
  if ((ntohs(dns_header->flags) & 0x8000) == 0) {

    const uint8_t *question = payload->data + sizeof(dns_header_t);

    char *extracted = (char *)malloc(question_len);
    if (extracted == NULL) {
      fprintf(stderr, "Memory allocation failed with code %d: %s.\n", errno,
              strerror(errno));
      return query_data;
    }

    int domain_ind = 0;
    int i = 0;

    while (i < payload->size) {
      int label_len = question[i];

      if (domain_ind + label_len + 1 > MAX_DOMAIN_LEN) {
        fprintf(stderr, "Domain length exceeds maximum allowed\n");
        free(extracted);
        return query_data;
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
      return query_data;
    }
    // hash dns question
    query_data.question_hash = fnv1a_hash_func(question);
    // create string with id + question
    uint8_t *id_w_question = (uint8_t *)malloc(question_len + ID_SIZE);
    if (id_w_question == NULL) {
      fprintf(stderr, "Memory allocation failed with code %d: %s.\n", errno,
              strerror(errno));
      return query_data;
    }
    // copy id first, and then all question
    memcpy(id_w_question, payload->data, ID_SIZE);
    memcpy(id_w_question + ID_SIZE, question, question_len);

    // hash this to identificate our client (specific ID + question)
    query_data.client_hash = fnv1a_hash_func(id_w_question);
    free(id_w_question);
    query_data.dns_name = extracted;
  }

  return query_data;
}

response_data_t parse_response(binary_string_t *payload) {
  binary_string_t *answer = (binary_string_t *)malloc(sizeof(binary_string_t));
  response_data_t response_data = {answer, 0, 0};
  dns_header_t *dns_header = (dns_header_t *)payload->data;

  if ((ntohs(dns_header->flags) & 0x8000) != 0) {
    // question_answer is starting point of question and answer in response
    uint8_t *question_answer = payload->data + sizeof(struct dns_header);
    int i = 0;
    size_t question_len = 0; // initialize
    // find length of question
    while (i < payload->size) {
      int label_len = question_answer[i];

      if (label_len == 0) {
        question_len += QTYPE_SIZE + QCLASS_SIZE + 1;
        break;
      }
      i += label_len + 1;
      question_len += label_len + 1;
    }
    uint8_t *question = (uint8_t *)malloc(question_len);
    if (question == NULL) {
      fprintf(stderr, "Memory allocation failed with code %d: %s.\n", errno,
              strerror(errno));
      return response_data;
    }
    memcpy(question, question_answer, question_len);
    uint32_t question_hash = fnv1a_hash_func(question);

    // create string with id + question
    uint8_t *id_w_question = (uint8_t *)malloc(question_len + ID_SIZE);
    if (id_w_question == NULL) {
      fprintf(stderr, "Memory allocation failed with code %d: %s.\n", errno,
              strerror(errno));
      free(question);
      return response_data;
    }
    // copy id first, and then whole question
    memcpy(id_w_question, payload->data, ID_SIZE);
    memcpy(id_w_question + ID_SIZE, question, question_len);

    free(question);
    // hash this to identificate our client (specific ID + question)
    uint32_t client_hash = fnv1a_hash_func(id_w_question);

    free(id_w_question);

    size_t answer_len =
        payload->size - (sizeof(struct dns_header) + question_len);
    // allocate memory for answer and copy it
    uint8_t *answer_str = (uint8_t *)malloc(answer_len);
    if (answer_str == NULL) {
      fprintf(stderr, "Memory allocation failed with code %d: %s.\n", errno,
              strerror(errno));
      return response_data;
    }
    memcpy(answer_str, question_answer + question_len, answer_len);

    answer->data = answer_str;
    answer->size = answer_len;

    response_data.answer = answer;
    response_data.client_hash = client_hash;
    response_data.question_hash = question_hash;
  }
  return response_data;
}
