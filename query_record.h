#ifndef QUERY_RECORD_H
#define QUERY_RECORD_H

#include <netinet/in.h>

#include "binary_string.h"

typedef struct record {
    struct sockaddr_in address;
    binary_string_t query;
} record_t;

int compare_record(const record_t* first, const record_t* second);
#endif // !QUERY_RECORD_H
