#include "query_record.h"
#include <string.h>

int compare_record(const record_t* first, const record_t* second) {

    if (first->query.size != second->query.size) {
        return 1;
    }

    return memcmp(first->query.data, second->query.data, first->query.size);
}