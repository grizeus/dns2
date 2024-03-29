#ifndef QUERY_RECORD_H
#define QUERY_RECORD_H

#include <netinet/in.h>

#include "binary_string.h"

typedef struct record {
    struct sockaddr_in address;
    binary_string_t query;
} record_t;

/**
 * @brief Compare two record_t objects.
 *
 * This function compares two record_t objects based on their query data sizes.
 * If the sizes are different, it returns 1. Otherwise, it compares the query
 * data byte by byte using memcmp. If the query data of the first record is
 * lexicographically less than the query data of the second record, it returns
 * a negative value. If they are equal, it returns 0. If the query data of the
 * first record is lexicographically greater than the query data of the second
 * record, it returns a positive value.
 *
 * @param first Pointer to the first record_t object.
 * @param second Pointer to the second record_t object.
 * @return Returns 0 if the records are equal; otherwise, returns a non-zero value.
 */
int compare_record(const record_t* first, const record_t* second);
#endif // !QUERY_RECORD_H
