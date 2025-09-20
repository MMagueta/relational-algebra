#ifndef SET_H
#define SET_H

#include <stddef.h>

typedef struct Set Set;

/* Function pointer type for comparison.
   Should return 0 if equal, <0 if a < b, >0 if a > b */
typedef int (*SetCompareFn)(const void *a, const void *b);

/* Function pointer type for freeing elements (optional, can be NULL) */
typedef void (*SetFreeFn)(void *elem);

Set *set_create(SetCompareFn cmp, SetFreeFn freer);
void set_destroy(Set *set);

/* Basic operations */
int set_add(Set *set, void *elem);
int set_remove(Set *set, const void *elem);
int set_contains(const Set *set, const void *elem);
size_t set_size(const Set *set);

/* Iteration */
typedef void (*SetIterFn)(void *elem, void *userdata);
void set_foreach(const Set *set, SetIterFn fn, void *userdata);

#endif // SET_H
