#include <stdlib.h>
#include <string.h>

#include "binary_string.h"

binary_string_t binary_string_create(uint8_t* data, size_t size) {

    binary_string_t new_string = { .data = NULL, .size = 0 };
    if (size == 0) {
        return new_string;
    }

    new_string.data = (uint8_t*)malloc(size);
    if (new_string.data != NULL) {
        memcpy(new_string.data, data, size);
        new_string.size = size;
    }

    return new_string;
}

void binary_string_destroy(binary_string_t* str) {

    if (str->data != NULL) {

        free(str->data);
        str->data = NULL;
        str->size = 0;
    }
}