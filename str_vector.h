#ifndef STR_VECTOR_H
#define STR_VECTOR_H

#define INIT_CAPACITY 1
#include "string.h"

typedef struct vector vector_t;

struct vector {

    string_t* data;
    size_t size;
    size_t capacity;
};

vector_t create_vector();
void push_back(vector_t* vec, const string_t* str);
void pop_back(vector_t* vec);
void destroy_vector(vector_t* vec);
// int in_list(const char* target, vector_t* vec);

#endif // !STR_VECTOR_H
