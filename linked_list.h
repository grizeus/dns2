#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct list {
  void *data;
  struct list *next;
} list_t;

list_t *list_new(void *data);
void list_add(list_t **head, void *data);
void list_add_node(list_t **head, list_t *node);
void list_iterate(list_t *head, void (*iter)(void *));
list_t *list_find(list_t *head, void *key, int (*compare)(void *, void *));
void list_delete(list_t **head, void *key, int (*compare)(void *, void *));
void list_clear(list_t **head);

#endif // !LIST_H
