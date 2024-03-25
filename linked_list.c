#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linked_list.h"

#define MAX_DNS_QUERY_SIZE 512

list_node_t* create_list_node(const struct sockaddr_in* client_addr, const uint8_t* query) {

    list_node_t* new_node = (list_node_t*)malloc(sizeof(list_node_t));

    new_node->client_addr = client_addr;
    new_node->query = query;

    new_node->prev = NULL;
    new_node->next = NULL;

    return new_node;
}

void linked_list_init(linked_list_t* list) {

    list->head = NULL;
    list->tail = NULL;
}

void insert_into_list(linked_list_t* list, const struct sockaddr_in* client_addr, const uint8_t* query) {

    list_node_t* new_node = create_list_node(client_addr, query);

    if (list->head == NULL) {
        list->head = new_node;
        list->tail = new_node;
    } else {
        new_node->next = list->head;
        list->head->prev = new_node;
        list->head = new_node;
    }
}

list_node_t* search_by_query(linked_list_t* list, const uint8_t* query, int query_len) {

    list_node_t* current = list->head;
    while (current != NULL) {
        
        if (memcmp(current->query, query, query_len) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void delete_from_list(linked_list_t* list, const uint8_t* query, int query_len) {
    // Search for the node with the specified query
    list_node_t* node_to_delete = search_by_query(list, query, query_len);

    if (node_to_delete == NULL) {
        // Node with the specified query not found
        return;
    }

    // Adjust pointers of adjacent nodes
    if (node_to_delete->prev != NULL) {
        node_to_delete->prev->next = node_to_delete->next;
    } else {
        // Node to delete is the head of the list
        list->head = node_to_delete->next;
    }

    if (node_to_delete->next != NULL) {
        node_to_delete->next->prev = node_to_delete->prev;
    } else {
        // Node to delete is the tail of the list
        list->tail = node_to_delete->prev;
    }

    // Free memory allocated for the node
    free(node_to_delete);
}

void list_traversal(linked_list_t* list) {

    list_node_t* current = list->head;

    printf("address|query: ");
    while (current != NULL) {
        char ip_address[INET_ADDRSTRLEN];
        
        if (inet_ntop(AF_INET, &(current->client_addr->sin_addr), ip_address, INET_ADDRSTRLEN) == NULL) {
            perror("Error converting IP address to string");
            return;
        }
        printf("%s: ", ip_address);

        for (int i = 0; i < MAX_DNS_QUERY_SIZE; ++i) {
            printf("%02x ", current->query[i] & 0xFF);
        }
        current = current->next;
    }
    printf("\n");
}

int is_one_node(const linked_list_t* list) {

    return (list->head != NULL && list->head == list->tail);
}
