#ifndef MAP_H
#define MAP_H

#include <netinet/in.h>
#include <stdint.h>
#include <sys/types.h>
#include "linked_list.h"

typedef struct transaction_record transaction_record_t;
typedef struct map_node map_node_t;
typedef struct map map_t;


typedef enum color {
    RED,
    BLACK
} color_t;



struct transaction_record {

    uint16_t transaction_id;
    linked_list_t* client_info;
};

struct map_node {
    transaction_record_t data;
    color_t color;
    map_node_t* parent;
    map_node_t* left;
    map_node_t* right;
};

struct map {
    map_node_t* root;
};

map_node_t* create_map_node(const uint16_t id, const struct sockaddr_in* client_addr, const uint8_t* query);
void save_client(map_t* map, const uint16_t id, const struct sockaddr_in* client_addr, const uint8_t* query);
void insert_fixup(map_t* map, map_node_t* node);
void right_rotate(map_t* map, map_node_t* node);
void left_rotate(map_t* map, map_node_t* node);
void map_traversal(map_node_t* node);
void destroy_map(map_t* map);
const struct sockaddr_in* get_client(map_t* map, uint16_t id, const uint8_t* query, ssize_t query_len);
void remove_client(map_t* map, uint16_t id, const uint8_t* query, ssize_t query_len);

#endif // !MAP_H
