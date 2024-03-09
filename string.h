#ifndef STRING_H
#define STRING_H

#include <stddef.h>

typedef struct string string_t;

struct string {
    char* data;
    size_t size;
};

string_t create_string(char* data);
void destroy_string(string_t* str);

#endif // !STRING_H
