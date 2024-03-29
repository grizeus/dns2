#ifndef BINARY_STRING_H
#define BINARY_STRING_H

#include <stdint.h>
#include <stddef.h>

typedef struct string {
    uint8_t* data;
    size_t size;
}binary_string_t;

#endif // !BINARY_STR_H
