#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

#include "map.h"

node_t* create_node(pair_value_t data) {

    node_t* new_node = (node_t*)malloc(sizeof(node_t));

    if (new_node != NULL) {
        new_node->data = data;
        new_node->color = RED;
        new_node->parent = NULL;
        new_node->left = NULL;
        new_node->right = NULL;
    }

    return new_node;
}

void insert(map_t* map, pair_value_t data) {

    node_t* new_node = create_node(data);

    node_t* parent = NULL;
    node_t* current = map->root;

    while (current != NULL) {
        parent = current;
        if (new_node->data.transactionID < current->data.transactionID) {
            current = current->left;
        } else {
            current = current->right;
        }
    }

    new_node->parent = parent;

    if (parent == NULL) {
        map->root = new_node;
    } else if (new_node->data.transactionID < parent->data.transactionID) {
        parent->left = new_node;
    } else {
        parent->right = new_node;
    }

    insert_fixup(map, new_node);
}

void insert_fixup(map_t* map, node_t* node) {

    while (node->parent != NULL && node->parent->color == RED) {

        if (node->parent == node->parent->parent->left) {

            node_t* uncle = node->parent->parent->right;
            if (uncle != NULL && uncle->color == RED) {
                node->parent->color = BLACK;
                uncle->color = BLACK;
                node->parent->parent->color = RED;
                node = node->parent->parent;
            } else {
                if (node == node->parent->right) {
                    node = node->parent;
                    left_rotate(map, node);
                }

                node->parent->color = BLACK;
                node->parent->parent->color = RED;
                right_rotate(map, node->parent->parent);
            }
        } else {

            node_t* uncle = node->parent->parent->left;
            if (uncle != NULL && uncle->color == RED) {
                node->parent->color = BLACK;
                uncle->color = BLACK;
                node->parent->parent->color = RED;
                node = node->parent->parent;
            } else {
                if (node == node->parent->left) {
                    node = node->parent;
                    right_rotate(map, node);
                }

                node->parent->color = BLACK;
                node->parent->parent->color = RED;
                left_rotate(map, node->parent->parent);
            }
        }
    }

    map->root->color = BLACK;
}

void right_rotate(map_t* map, node_t* node) {

    node_t* pivot = node->left;
    node_t* new_left_child = pivot->right;

    node->left = new_left_child;
    if (new_left_child != NULL) {
        new_left_child->parent = node;
    }

    pivot->parent = node->parent;

    if (node->parent == NULL) {
        map->root = pivot;
    } else if (node == node->parent->left) {
        node->parent->left = pivot;
    } else {
        node->parent->right = pivot;
    }

    pivot->right = node;
    node->parent = pivot;
}

void left_rotate(map_t* map, node_t* node) {

    node_t* pivot = node->right;
    node_t* new_right_child = pivot->left;

    node->right = new_right_child;
    if (new_right_child != NULL) {
        new_right_child->parent = node;
    }

    pivot->parent = node->parent;

    if (node->parent == NULL) {
        map->root = pivot;
    } else if (node == node->parent->left) {
        node->parent->left = pivot;
    } else {
        node->parent->right = pivot;
    }

    pivot->left = node;
    node->parent = pivot;
}

void inorder_traversal(node_t* node) {

    if (node != NULL) {
        inorder_traversal(node->left);
        printf("(%u, %s:%d)", node->data.transactionID,
               inet_ntoa(node->data.client.sin_addr),
               ntohs(node->data.client.sin_port));
        inorder_traversal(node->right);
    }
}

node_t* search(map_t* map, uint16_t key) {

    node_t* current = map->root;

    while (current != NULL && current->data.transactionID != key) {
        if (key < current->data.transactionID) {
            current = current->left;
        } else {
            current = current->right;
        }
    }

    return current;
}

static void destroy_node(node_t* root) {

    if (root != NULL) {
        destroy_node(root->left);
        destroy_node(root->right);
        free(root);
    }
}

void destroy_map(map_t* map) {

    destroy_node(map->root);
    map->root = NULL;
}
