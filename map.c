#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

#include "map.h"

static map_node_t* find_successor(map_node_t* node);
static void destroy_node(map_node_t* root);

map_node_t* create_map_node(const uint16_t id, const struct sockaddr_in* client_addr, const uint8_t* query) {

    map_node_t* new_node = (map_node_t*)malloc(sizeof(map_node_t));

    if (new_node != NULL) {
        new_node->data.transaction_id = id;
        linked_list_t* list;
        linked_list_init(list);
        insert_into_list(list, client_addr, query);
        new_node->data.client_info = list;
        new_node->color = RED;
        new_node->parent = NULL;
        new_node->left = NULL;
        new_node->right = NULL;
    }

    return new_node;
}

void save_client(map_t* map, const uint16_t id, const struct sockaddr_in* client_addr, const uint8_t* query) {

    map_node_t* new_node = create_map_node(id, client_addr, query);

    map_node_t* parent = NULL;
    map_node_t* current = map->root;

    while (current != NULL) {
        parent = current;
        if (new_node->data.transaction_id < current->data.transaction_id) {
            current = current->left;
        } else if (new_node->data.transaction_id > current->data.transaction_id){
            current = current->right;
        } else { // for node with the same transaction_id
            insert_into_list(current->data.client_info, client_addr, query);
            free(new_node);
            return;
        }
    }

    new_node->parent = parent;

    if (parent == NULL) {
        map->root = new_node;
    } else if (new_node->data.transaction_id < parent->data.transaction_id) {
        parent->left = new_node;
    } else {
        parent->right = new_node;
    }

    insert_fixup(map, new_node);
}

void insert_fixup(map_t* map, map_node_t* node) {

    while (node->parent != NULL && node->parent->color == RED) {

        if (node->parent == node->parent->parent->left) {

            map_node_t* uncle = node->parent->parent->right;
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

            map_node_t* uncle = node->parent->parent->left;
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

void right_rotate(map_t* map, map_node_t* node) {

    map_node_t* pivot = node->left;
    map_node_t* new_left_child = pivot->right;

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

void left_rotate(map_t* map, map_node_t* node) {

    map_node_t* pivot = node->right;
    map_node_t* new_right_child = pivot->left;

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

void map_traversal(map_node_t* node) {

    if (node != NULL) {
        map_traversal(node->left);
        printf("(%u)", node->data.transaction_id);
        list_traversal(node->data.client_info);
        map_traversal(node->right);
    }
}

const struct sockaddr_in* get_client(map_t* map, uint16_t id, const uint8_t* query, int query_len) {

    map_node_t* current = map->root;

    while (current != NULL && current->data.transaction_id != id) {
        if (id < current->data.transaction_id) {
            current = current->left;
        } else {
            current = current->right;
        }
    }

    if (is_one_node(current->data.client_info)) {
        return current->data.client_info->head->client_addr;
    } else {
        list_node_t* req_node = search_by_query(current->data.client_info, query, query_len);
        return req_node->client_addr;
    }
}

// Function to remove a node from the map
void remove_client(map_t* map, uint16_t id, const uint8_t* query, int query_len) {
    map_node_t* current = map->root;
    map_node_t* parent = NULL;
    map_node_t* node_to_remove = NULL;

    // Find the node to remove
    while (current != NULL) {
        if (id == current->data.transaction_id) {
            if (is_one_node(current->data.client_info)) {
                node_to_remove = current;
                break;
            } else {
                delete_from_list(current->data.client_info, query, query_len);
                return;
            }
        } else if (id < current->data.transaction_id) {
            parent = current;
            current = current->left;
        } else {
            parent = current;
            current = current->right;
        }
    }

    if (node_to_remove == NULL) {
        // Node with the given ID not found
        return;
    }

    // Case 1: Node has no children
    if (node_to_remove->left == NULL && node_to_remove->right == NULL) {
        if (parent == NULL) {
            // Root node
            map->root = NULL;
        } else if (parent->left == node_to_remove) {
            parent->left = NULL;
        } else {
            parent->right = NULL;
        }
        destroy_node(node_to_remove);
    }
    // Case 2: Node has one child
    else if (node_to_remove->left == NULL || node_to_remove->right == NULL) {
        map_node_t* child = (node_to_remove->left != NULL) ? node_to_remove->left : node_to_remove->right;
        if (parent == NULL) {
            // Root node
            map->root = child;
        } else if (parent->left == node_to_remove) {
            parent->left = child;
        } else {
            parent->right = child;
        }
        destroy_node(node_to_remove);
    }
    // Case 3: Node has two children
    else {
        map_node_t* successor = find_successor(node_to_remove->right);
        node_to_remove->data = successor->data;
        remove_client(map, successor->data.transaction_id, query, query_len);
    }
}

// Function to find the successor of a node (used in case of removing a node with two children)
static map_node_t* find_successor(map_node_t* node) {
    while (node->left != NULL) {
        node = node->left;
    }
    return node;
}

static void destroy_node(map_node_t* root) {

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
