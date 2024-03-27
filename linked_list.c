#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linked_list.h"

list_node_t* create_list_node(void* data) {

    list_node_t* new_node = (list_node_t*)malloc(sizeof(list_node_t));
    if (!new_node) {
        perror("Memory allocation for list failed");
        free(new_node);
        return NULL;
    }

    new_node->data = data;

    new_node->prev = NULL;
    new_node->next = NULL;

    return new_node;
}

void linked_list_init(linked_list_t* list) {

    list->head = NULL;
    list->tail = NULL;
}

void insert_into_list(linked_list_t* list, void* data) {

    list_node_t* new_node = create_list_node(data);

    if (list->head == NULL) {
        list->head = new_node;
        list->tail = new_node;
    } else {
        new_node->next = list->head;
        list->head->prev = new_node;
        list->head = new_node;
    }
}

int compare_by_query(transaction_info_t* first, transaction_info_t* second, ssize_t len) {

    if (memcmp(first->query, second->query, len)) {
        return 1;
    }
    return 0;
}

void delete_from_list(linked_list_t* list, void* key, ssize_t len, int(*compare)(void*, void*, ssize_t)) {

    if (list == NULL || compare == NULL) {
        return;
    }

    list_node_t* current = list->head;

    while (current != NULL) {
        if ((*compare)(current->data, key, len)) {

            // Adjust pointers of adjacent nodes
            if (current->prev != NULL) {
                current->prev->next = current->next;
            } else {
                // Node to delete is the head of the list
                list->head = current->next;
            }

            if (current->next != NULL) {
                current->next->prev = current->prev;
            } else {
                // Node to delete is the tail of the list
                list->tail = current->prev;
            }

            free(current);
            return;
        }

        current = current->next;
    }
}

void list_iterate(linked_list_t* list, void (*iter)(void*)) {

    list_node_t* current = list->head;

    while (current != NULL) {
        (*iter)(current->data);
        current = current->next;
    }
}

int is_one_node(const linked_list_t* list) {

    return (list->head != NULL && list->head == list->tail);
}
