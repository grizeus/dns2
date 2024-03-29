#include <stdlib.h>

#include "map.h"

static void map_rebalance(map_t** root, map_t* node); // static
static void right_rotate(map_t** root, map_t* node);
static void left_rotate(map_t** root, map_t* node);

map_t* map_create(int key, void* data) {

    map_t* new_node = (map_t*)malloc(sizeof(map_t));

    if (new_node != NULL) {

        new_node->key = key;
        new_node->data = data;
        new_node->color = RED;
        new_node->parent = NULL;
        new_node->left = NULL;
        new_node->right = NULL;
    }

    return new_node;
}

void map_add(map_t** root, int key, void* data, void(*inner_job)(void*, void*)) {

    map_t* new_node = map_create(key, data);

    map_t* parent = NULL;
    map_t* current = *root;

    while (current != NULL) {

        parent = current;
        if (new_node->key < current->key) {
            current = current->left;
        } else if (new_node->key > current->key) {
            current = current->right;
        } else {
            if (inner_job) {
                inner_job(current->data, data);
            }
            free(new_node);
            return;
        }
    }

    new_node->parent = parent;

    if (parent == NULL) {
        *root = new_node;
    } else if (new_node->key < parent->key) {
        parent->left = new_node;
    } else {
        parent->right = new_node;
    }

    map_rebalance(root, new_node);
}

static void map_rebalance(map_t** root, map_t* node) {

    while (node->parent != NULL && node->parent->color == RED) {

        if (node->parent == node->parent->parent->left) {

            map_t* uncle = node->parent->parent->right;
            if (uncle != NULL && uncle->color == RED) {
                node->parent->color = BLACK;
                uncle->color = BLACK;
                node->parent->parent->color = RED;
                node = node->parent->parent;
            } else {
                if (node == node->parent->right) {
                    node = node->parent;
                    left_rotate(root, node);
                }

                node->parent->color = BLACK;
                node->parent->parent->color = RED;
                right_rotate(root, node->parent->parent);
            }
        } else {

            map_t* uncle = node->parent->parent->left;
            if (uncle != NULL && uncle->color == RED) {
                node->parent->color = BLACK;
                uncle->color = BLACK;
                node->parent->parent->color = RED;
                node = node->parent->parent;
            } else {
                if (node == node->parent->left) {
                    node = node->parent;
                    right_rotate(root, node);
                }

                node->parent->color = BLACK;
                node->parent->parent->color = RED;
                left_rotate(root, node->parent->parent);
            }
        }
    }

    (*root)->color = BLACK;
}

static void right_rotate(map_t** root, map_t* node) {

    map_t* pivot = node->left;
    map_t* new_left_child = pivot->right;

    node->left = new_left_child;
    if (new_left_child != NULL) {
        new_left_child->parent = node;
    }

    pivot->parent = node->parent;

    if (node->parent == NULL) {
        *root = pivot;
    } else if (node == node->parent->left) {
        node->parent->left = pivot;
    } else {
        node->parent->right = pivot;
    }

    pivot->right = node;
    node->parent = pivot;
}

void left_rotate(map_t** root, map_t* node) {

    map_t* pivot = node->right;
    map_t* new_right_child = pivot->left;

    node->right = new_right_child;
    if (new_right_child != NULL) {
        new_right_child->parent = node;
    }

    pivot->parent = node->parent;

    if (node->parent == NULL) {
        *root = pivot;
    } else if (node == node->parent->left) {
        node->parent->left = pivot;
    } else {
        node->parent->right = pivot;
    }

    pivot->left = node;
    node->parent = pivot;
}

void* map_find(map_t* root, int key) {

    if(root == NULL) {
        return NULL;
    }

    if (key == root->key) {
        return root->data;
    } else if (key < root->key) {
        return map_find(root->left, key);
    } else {
        return map_find(root->right, key);
    }
}

void map_iterate(map_t* node, void(*iter)(void*)) {

    if (node != NULL) {
        map_iterate(node->left, iter);
        (*iter)(node->data);
        map_iterate(node->right, iter);
    }
}

void map_delete(map_t** root, int key, void(*deleter)(void*)) {

    if (*root == NULL) {
        return;
    }

    if (key < (*root)->key) {
        map_delete(&((*root)->left), key, deleter);
    } else if (key > (*root)->key) {
        map_delete(&((*root)->right), key, deleter);
    } else {

        map_t* node_to_delete = *root;
        // Case 1: Node has no children or one child
        if (node_to_delete->left == NULL || node_to_delete->right == NULL) {
            if (node_to_delete->left == NULL) {
                *root = node_to_delete->right;
            } else {
                *root = node_to_delete->left;
            }

            if (*root != NULL) {
                (*root)->parent = node_to_delete->parent;
            }

            if (deleter != NULL) {
                deleter(node_to_delete->data);
            }

            free(node_to_delete);
        }
        // Case 2: Node has one child
        else {

            map_t* successor = node_to_delete->right;
            while (successor->left != NULL) {
                successor = successor->left;
            }

            node_to_delete->key = successor->key;
            node_to_delete->data = successor->data;

            map_delete(&(node_to_delete->right), successor->key, deleter);
        }
    }
}

void map_clear(map_t** root, void(*deleter)(void*)) {

    if (*root == NULL) {
        return;
    }

    map_clear(&((*root)->left), deleter);
    map_clear(&((*root)->right), deleter);

    map_delete(root, (*root)->key, deleter);
}