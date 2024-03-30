#include "query_record.h"
#include <string.h>

int compare_record(const record_t* first, const binary_string_t* second) {

    if (first->query.size != second->size) {
        return 1;
    }

    return memcmp(first->query.data, second->data, second->size);
}