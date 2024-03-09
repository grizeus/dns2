#include "string.h"

#include <stdlib.h>
#include <string.h>

string_t create_string(char *data) {

    string_t str = { .data = NULL, .size = 0 };
    size_t size = strlen(data);

    if (size == 0) {
        return str;
    }

    str.data = (char*)malloc(size + 1);

    if (str.data != NULL) {
        strncpy(str.data, data, size);
        str.data[size] = '\0';
        str.size = size;
    }

    return str;
}

void destroy_string(string_t *str) {

    if (str->data != NULL) {

        free(str->data);
        str->data = NULL;
        str->size = 0;
    }

}
