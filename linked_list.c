#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linked_list.h"

list_t* list_new(void* data) {

    list_t* new_node = (list_t*)malloc(sizeof(list_t));
    if (!new_node) {
        perror("Memory allocation for list failed");
        free(new_node);
        return NULL;
    }

    new_node->data = data;
    new_node->next = NULL;

    return new_node;
}

void list_add(list_t** head, void* data) {

    list_t* new_node = list_new(data);

    if (*head == NULL) {
        *head = new_node;
    } else {
        new_node->next = *head;
        *head = new_node;
    }
}

void list_add_node(list_t** head, list_t* node) {

    if (*head == NULL) {
        *head = node;
    } else {
        node->next = *head;
        *head = node;
    }
}
void list_iterate(list_t* list, void (*iter)(void*)) {

    list_t* current = list;

    while (current != NULL) {
        (*iter)(current->data);
        current = current->next;
    }
}

list_t* list_find(list_t* list, void* key, int(*compare)(void*, void*)) {

    list_t* current = list;
    while (current != NULL) {
        if (compare(current->data, key) == 0) {
            return current;
        }
        current = current->next;
    }

    return NULL;
}

void list_delete(list_t** list, void* key, int(*compare)(void*, void*)) {

    if (*list == NULL || compare == NULL) {
        return;
    }

    list_t* current = *list;
    list_t* prev = NULL;

    while (current != NULL) {
        if ((*compare)(current->data, key) == 0) {

            // Adjust pointers of adjacent nodes
            if (prev != NULL) {
                prev->next = current->next;
            } else {
                *list = current->next;
            }

            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

void list_clear(list_t** head) {

    list_t* current = *head;
    list_t* next = NULL;

    while (current != NULL) {
        
        next = current->next;
        free(current);
        current = next;
    }

    *head = NULL;
}
// int is_one_node(const linked_list_t* list) {
//
//     return (list->head != NULL && list->head == list->tail);
// }
