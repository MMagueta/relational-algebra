/**
 * @file set.c
 * @brief Set implementation for the Relational Algebra Engine.
 *
 * Implements a generic set data structure used to represent relations and tuples
 * as sets, in accordance with relational theory.
 *
 */
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

/**
 * @brief Create a new Set.
 *
 * @param cmp Comparison function for elements.
 * @param freer Free function for elements (can be NULL).
 * @return Pointer to new Set, or NULL on failure.
 */
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

/**
 * @brief Destroy a Set and free its memory.
 *
 * @param set Pointer to the Set to destroy. Safe to pass NULL.
 */
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

/**
 * @brief Check if a Set contains an element.
 *
 * @param set Pointer to the Set.
 * @param elem Pointer to the element to check.
 * @return 1 if present, 0 if not.
 */
int set_contains(const Set *set, const void *elem) {
  for (SetNode *n = set->head; n; n = n->next) {
    if (set->cmp(n->data, elem) == 0)
      return 1;
  }
  return 0;
}

/**
 * @brief Add an element to a Set.
 *
 * @param set Pointer to the Set.
 * @param elem Pointer to the element to add.
 * @return 1 if added, 0 if already present, -1 on error.
 */
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

/**
 * @brief Remove an element from a Set.
 *
 * @param set Pointer to the Set.
 * @param elem Pointer to the element to remove.
 * @return 1 if removed, 0 if not found.
 */
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

/**
 * @brief Get the number of elements in a Set.
 *
 * @param set Pointer to the Set.
 * @return Number of elements in the Set.
 */
size_t set_size(const Set *set) { return set->size; }

/**
 * @brief Iterate over all elements in a Set.
 *
 * @param set Pointer to the Set.
 * @param fn Callback function to call for each element.
 * @param userdata User data to pass to callback.
 */
void set_foreach(const Set *set, SetIterFn fn, void *userdata) {
  for (SetNode *n = set->head; n; n = n->next) {
    fn(n->data, userdata);
  }
}
