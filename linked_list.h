#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <netinet/in.h>
#include <stdint.h>

typedef struct list_node list_node_t;
typedef struct linked_list linked_list_t;
typedef struct transaction_info transaction_info_t;

struct transaction_info {

    const struct sockaddr_in* client_addr;
    const uint8_t* query;
};

struct list_node {

    void* data;
    list_node_t* prev;
    list_node_t* next;
};

struct linked_list {

    list_node_t* head;
    list_node_t* tail;
};

list_node_t* create_list_node(void* data);
void linked_list_init(linked_list_t* list);
void insert_into_list(linked_list_t* list, void* data);
int compare_by_query(transaction_info_t* first, transaction_info_t* second, ssize_t len);
void delete_from_list(linked_list_t* list, void* key, ssize_t len, int(*compare)(void*, void*, ssize_t));
void list_iterate(linked_list_t* list, void (*iter)(void*));
int is_one_node(const linked_list_t* list);
#endif // !LINKED_LIST_H
