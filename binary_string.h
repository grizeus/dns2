#ifndef BINARY_STRING_H
#define BINARY_STRING_H

#include <stdint.h>
#include <stddef.h>

typedef struct string {
    uint8_t* data;
    size_t size;
}binary_string_t;

binary_string_t binary_string_create(uint8_t* data, size_t size);
void binary_string_destroy(binary_string_t* str);
#endif // !BINARY_STR_H
