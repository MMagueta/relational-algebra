#include "set.h"
#include <stdlib.h>

typedef struct SetNode {
  void *data;
  struct SetNode *next;
} SetNode;

struct Set {
  SetNode *head;
  SetCompareFn cmp;
  SetFreeFn freer;
  size_t size;
};

Set *set_create(SetCompareFn cmp, SetFreeFn freer) {
  if (!cmp)
    return NULL;
  Set *s = malloc(sizeof(Set));
  if (!s)
    return NULL;
  s->head = NULL;
  s->cmp = cmp;
  s->freer = freer;
  s->size = 0;
  return s;
}

void set_destroy(Set *set) {
  if (!set)
    return;
  SetNode *cur = set->head;
  while (cur) {
    SetNode *next = cur->next;
    if (set->freer)
      set->freer(cur->data);
    free(cur);
    cur = next;
  }
  free(set);
}

int set_contains(const Set *set, const void *elem) {
  for (SetNode *n = set->head; n; n = n->next) {
    if (set->cmp(n->data, elem) == 0)
      return 1;
  }
  return 0;
}

int set_add(Set *set, void *elem) {
  if (set_contains(set, elem))
    return 0; // already present
  SetNode *n = malloc(sizeof(SetNode));
  if (!n)
    return -1;
  n->data = elem;
  n->next = set->head;
  set->head = n;
  set->size++;
  return 1;
}

int set_remove(Set *set, const void *elem) {
  SetNode *prev = NULL, *cur = set->head;
  while (cur) {
    if (set->cmp(cur->data, elem) == 0) {
      if (prev)
        prev->next = cur->next;
      else
        set->head = cur->next;
      if (set->freer)
        set->freer(cur->data);
      free(cur);
      set->size--;
      return 1;
    }
    prev = cur;
    cur = cur->next;
  }
  return 0; // not found
}

size_t set_size(const Set *set) { return set->size; }

void set_foreach(const Set *set, SetIterFn fn, void *userdata) {
  for (SetNode *n = set->head; n; n = n->next) {
    fn(n->data, userdata);
  }
}
