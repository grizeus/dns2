#ifndef MAP_H
#define MAP_H

typedef struct map map_t;

typedef enum color {
    RED,
    BLACK
} color_e;

struct map {
    void* data;
    map_t* parent;
    map_t* left;
    map_t* right;
    int key;
    color_e color;
};

map_t* map_create(int key, void* data);
void map_add(map_t** root, int key, void* data, void(*inner_job)(void*, void*));
void map_delete(map_t** root, int key, void(*deleter)(void*));
void* map_find(map_t* root, int key);
void map_iterate(map_t* root, void(*iter)(void*));
void map_clear(map_t** root, void(*deleter)(void*));

#endif // !MAP_H