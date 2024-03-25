#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "str_vector.h"
#include "string.h"

vector_t create_vector() {

    vector_t vec = { .data = NULL, .size = 0, .capacity = INIT_CAPACITY };

    vec.data = (string_t*)malloc(sizeof(string_t));
    if (vec.data == NULL) {

        fprintf(stderr, "Vector memory allocation failed\n");
        return vec;
    }

    return vec;
}

void push_back(vector_t* vec, const string_t* str) {

    if (vec->size == vec->capacity) {

        vec->capacity *= 2;
        vec->data = (string_t*)realloc(vec->data, vec->capacity * sizeof(string_t));
        if (vec->data == NULL) {

            fprintf(stderr, "Vector memory reallocation failed\n");
            return;
        }
    }

    vec->data[vec->size].data = strdup(str->data);
    if (vec->data[vec->size].data == NULL) {

        fprintf(stderr, "Pushback was failed\n");
        return;
    }

    vec->data[vec->size].size = str->size;
    vec->size++;
}

void pop_back(vector_t* vec) {

    if (vec->size > 0) {
        destroy_string(&vec->data[vec->size - 1]);
        vec->size--;
    }
}

void destroy_vector(vector_t* vec) {

    if (vec->data == NULL || vec->size == 0) {

        return;
    }

    for (size_t i = 0; i < vec->size; ++i) {

        destroy_string(&vec->data[i]);
    }

    free(vec->data);
    vec->data = NULL;
    vec->size = 0;
    vec->capacity = 0;
}

// int in_list(const char* target, vector_t* vec) {
//
//     for (size_t i = 0; i < vec->size; ++i) {
//         if (strcmp(target, vec->data[i].data) == 0) {
//             return 1;
//         }
//     }
//
//     return 0;
// }
