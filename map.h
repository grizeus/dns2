#ifndef MAP_H
#define MAP_H

#include <stdint.h>

typedef struct map map_t;
typedef struct node node_t;

typedef enum color { RED, BLACK } color_e;

struct map {
  node_t *root;
  node_t *sentinel;
};
struct node {
  void *data;
  node_t *parent;
  node_t *left;
  node_t *right;
  uint32_t key;
  color_e color;
};

map_t *map_create();
void map_add(map_t *map, uint32_t key, void *data,
             void (*inner_job)(void *, void *));
void map_delete(map_t *map, uint32_t key, void (*deleter)(void *, void *),
                void *additional_key, void (*eraser)(void *));
void *map_find(map_t *map, uint32_t key);
void map_iterate(map_t *map, void (*iter)(void *));
void map_clear(map_t *map, void (*eraser)(void *));
void map_destroy(map_t *map, void (*eraser)(void *));
#endif // !MAP_H
