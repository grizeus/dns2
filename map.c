#include <stdlib.h>

#include "map.h"


static node_t* node_create(int key, void* data);
static void map_rebalance(map_t* map, node_t* node);
static void right_rotate(map_t* map, node_t* node);
static void left_rotate(map_t* map, node_t* node);
static void iterate_helper(map_t* map, node_t* node, void(*iter)(void*));
static node_t* find_minimum(map_t* map, node_t* node);
static void transplant(map_t* map, node_t* x, node_t* y);
static void delete_fixup(map_t* map, node_t* node);
static void clear_helper(map_t* map, node_t* node, void(*eraser)(void*));
node_t* map_find_node(map_t* map, int key);

static node_t* node_create(int key, void* data) {

    node_t* new_node = (node_t*)malloc(sizeof(node_t));

    if (new_node) {
        new_node->key = key;
        new_node->data = data;
        new_node->color = RED;
        new_node->parent = NULL;
        new_node->left = NULL;
        new_node->right = NULL;
    }

    return new_node;
}

map_t* map_create() {
    map_t* new_map = (map_t*)malloc(sizeof(map_t));

    if (new_map) {
        new_map->sentinel = node_create(-1, NULL);
        new_map->sentinel->color = BLACK;
        new_map->sentinel->left = NULL;
        new_map->sentinel->right = NULL;
        new_map->root = new_map->sentinel;
    }

    return new_map;
}
void map_add(map_t* map, int key, void* data, void(*inner_job)(void*, void*)) {

    node_t* new_node = node_create(key, data);

    node_t* parent = map->sentinel;
    node_t* current = map->root;

    while (current != map->sentinel) {

        parent = current;
        if (new_node->key < current->key) {
            current = current->left;
        } else if (new_node->key > current->key) {
            current = current->right;
        } else {
            if (inner_job) {
                (*inner_job)(&(current->data), data);
            }
            free(new_node);
            return;
        }
    }

    new_node->parent = parent;

    if (parent == map->sentinel) {
        map->root = new_node;
    } else if (new_node->key < parent->key) {
        parent->left = new_node;
    } else {
        parent->right = new_node;
    }

    new_node->left = map->sentinel;
    new_node->right = map->sentinel;
    map_rebalance(map, new_node);
}

static void map_rebalance(map_t* map, node_t* node) {

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

static void right_rotate(map_t* map, node_t* node) {

    node_t* pivot = node->left;
    node_t* new_left_child = pivot->right;

    node->left = new_left_child;
    if (new_left_child != map->sentinel) {
        new_left_child->parent = node;
    }

    pivot->parent = node->parent;

    if (node->parent == map->sentinel) {
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
    if (new_right_child != map->sentinel) {
        new_right_child->parent = node;
    }

    pivot->parent = node->parent;

    if (node->parent == map->sentinel) {
        map->root = pivot;
    } else if (node == node->parent->left) {
        node->parent->left = pivot;
    } else {
        node->parent->right = pivot;
    }

    pivot->left = node;
    node->parent = pivot;
}

void* map_find(map_t* map, int key) {

    if(map->root == map->sentinel) {
        return NULL;
    }

    node_t* current = map->root;

    while (current != map->sentinel && key != current->key){
        if (key < current->key) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    
    return current->data;
}

node_t* map_find_node(map_t* map, int key) {


    if(map->root == map->sentinel) {
        return map->sentinel;
    }

    node_t* current = map->root;

    while (current != map->sentinel && key != current->key){
        if (key < current->key) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    
    return current;
}

static void iterate_helper(map_t* map, node_t* node, void(*iter)(void*)) {
    if (node != map->sentinel) {
        iterate_helper(map, node->left, iter);
        (*iter)(node->data);
        iterate_helper(map, node->right, iter);
    }
}

void map_iterate(map_t* map, void(*iter)(void*)) {

    iterate_helper(map, map->root, iter);
}

void map_delete(map_t* map, int key, void(*deleter)(void*, void*), void* additional_key, void(*eraser)(void*)) {

    if (map->root == map->sentinel) {
        return;
    }
    node_t* node_to_delete = map_find_node(map, key);
    if (node_to_delete == map->sentinel) {
        printf("Node by key %d not found\n", key);
        return;
    }
    node_t* successor = NULL;
    int is_empty = 0;
    node_t* minimum_node = node_to_delete;
    color_e original_color = node_to_delete->color;

    // perform delete from inner structure
    if (eraser) {
        (*eraser)(&(node_to_delete->data));
    }
    if (deleter) {
        (*deleter)(&(node_to_delete->data), additional_key);
        if (node_to_delete->data == NULL) {
            is_empty = 1;
        }
        if (!is_empty) {
            return;
        }
    }

    if (node_to_delete->left == map->sentinel) {
        successor = node_to_delete->right;
        transplant(map, node_to_delete, node_to_delete->right);
    } else if (node_to_delete->right == map->sentinel) {
        successor = node_to_delete->left;
        transplant(map, node_to_delete, node_to_delete->left);
    } else {
        minimum_node = find_minimum(map, node_to_delete->right);
        original_color = minimum_node->color;
        successor = minimum_node->right;

        if (minimum_node->parent == node_to_delete) {
            if (successor != NULL) {
                successor->parent = minimum_node;
            }
        } else {
            transplant(map, minimum_node, minimum_node->right);
            minimum_node->right = node_to_delete->right;
            minimum_node->right->parent = minimum_node;
        }
        
        transplant(map, node_to_delete, minimum_node);
        minimum_node->left = node_to_delete->left;
        minimum_node->left->parent = minimum_node;
        minimum_node->color = node_to_delete->color;
    }

    if (original_color == BLACK) {
        delete_fixup(map, successor);
    }

    if (node_to_delete == map->root) {
        map->root = successor;
    }
    
    free(node_to_delete);
}

static void delete_fixup(map_t* map, node_t* node) {

    node_t* sibling;
    while (node != map->root && node->color == BLACK) {
        
        if (node == node->parent->left) {
            sibling = node->parent->right;

            if (sibling->color == RED) {
                sibling->color = BLACK;
                node->parent->color = RED;
                left_rotate(map, node->parent);
                sibling = node->parent->right;
            }

            if (sibling->left->color == BLACK && sibling->right->color == BLACK) {
                sibling->color = RED;
                node = node->parent;
            } else {

                if (sibling->right->color == BLACK) {
                    sibling->left->color = BLACK;
                    sibling->color = RED;
                    right_rotate(map, sibling);
                    sibling = node->parent->right;
                }

                sibling->color = node->parent->color;
                node->parent->color = BLACK;
                sibling->right->color = BLACK;
                left_rotate(map, node->parent);
                node = map->root;
            }

        } else {
            sibling = node->parent->left;

            if (sibling->color == RED) {
                sibling->color = BLACK;
                node->parent->color = RED;
                right_rotate(map, node->parent);
                sibling = node->parent->left;
            }

            if (sibling->left->color == BLACK && sibling->right->color == BLACK) {
                sibling->color = RED;
                node = node->parent;
            } else {

                if (sibling->left->color == BLACK) {
                    sibling->right->color = BLACK;
                    sibling->color = RED;
                    left_rotate(map, sibling);
                    sibling = node->parent->left;
                }

                sibling->color = node->parent->color;
                node->parent->color = BLACK;
                sibling->left->color = BLACK;
                right_rotate(map, node->parent);
                node = map->root;
            }
        }
    }
    node->color = BLACK;
}

static void clear_helper(map_t* map, node_t* node, void(*eraser)(void*)) {

    if (node != map->sentinel) {
        clear_helper(map, node->left, eraser);
        clear_helper(map, node->right, eraser);
        map_delete(map, node->key, NULL, NULL, eraser);
    }
}

void map_clear(map_t* map, void(*eraser)(void*)) {

    if (map->root == map->sentinel) {
        return;
    }

    clear_helper(map, map->root, eraser);
    map->root = map->sentinel;
}

static node_t* find_minimum(map_t* map, node_t* node) {
    while (node->left != map->sentinel) {
        node = node->left;
    }
    return node;
}

static void transplant(map_t* map, node_t* x, node_t* y) {
    if (x->parent == map->sentinel) {
        map->root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->parent = x->parent;
}