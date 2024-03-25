#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <netinet/in.h>
#include <stdint.h>

typedef struct list_node list_node_t;
typedef struct linked_list linked_list_t;

struct list_node {

    const struct sockaddr_in* client_addr;
    const uint8_t* query;
    list_node_t* prev;
    list_node_t* next;
};

struct linked_list {

    list_node_t* head;
    list_node_t* tail;
};

list_node_t* create_list_node(const struct sockaddr_in* client_addr, const uint8_t* query);
void linked_list_init(linked_list_t* list);
void insert_into_list(linked_list_t* list, const struct sockaddr_in* client_addr, const uint8_t* query);
list_node_t* search_by_query(linked_list_t* list, const uint8_t* query, int query_len);
void delete_from_list(linked_list_t* list, const uint8_t* query, int query_len);
void list_traversal(linked_list_t* list);
int is_one_node(const linked_list_t* list);
#endif // !LINKED_LIST_H
