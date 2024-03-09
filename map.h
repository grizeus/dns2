#ifndef MAP_H
#define MAP_H

#include <netinet/in.h>
#include <stdint.h>

typedef struct pair_value pair_value_t;
typedef struct node node_t;
typedef struct map map_t;

typedef enum color {
    RED,
    BLACK
} color_t;

struct pair_value {

    uint16_t transactionID;
    struct sockaddr_in client;
};

struct node {
    pair_value_t data;
    color_t color;
    node_t* parent;
    node_t* left;
    node_t* right;
};

struct map {
    node_t* root;
};

node_t* create_node(pair_value_t data);
void insert(map_t* map, pair_value_t data);
void insert_fixup(map_t* map, node_t* node);
void right_rotate(map_t* map, node_t* node);
void left_rotate(map_t* map, node_t* node);
void inorder_traversal(node_t* node);
void destroy_map(map_t* map);
node_t* search(map_t* map, uint16_t key);

#endif // !MAP_H
