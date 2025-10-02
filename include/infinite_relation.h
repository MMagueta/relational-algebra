#ifndef INFINITE_RELATION_H
#define INFINITE_RELATION_H

#include "tuple.h"

/**
 * Function type for generating a tuple given an index n ∈ N.
 * Must return a freshly allocated Tuple*.
 */
typedef Tuple *(*TupleGeneratorFn)(size_t n, void *userdata);

/**
 * InfiniteRelation is just a handle for a generator + metadata.
 */
typedef struct {
    const char *name;
    TupleGeneratorFn gen_fn;
    void *userdata;
} InfiniteRelation;

/**
 * Create an infinite relation defined by a generator function.
 */
InfiniteRelation *infinite_relation_create(const char *name,
                                           TupleGeneratorFn fn,
                                           void *userdata);

/**
 * Destroy an infinite relation handle (does not free generated tuples).
 */
void infinite_relation_destroy(InfiniteRelation *r);

/**
 * Get the nth tuple of the infinite relation.
 */
Tuple *infinite_relation_tuple_at(InfiniteRelation *r, size_t n);

/**
 * Print first `count` tuples of the infinite relation.
 */
void infinite_relation_print_prefix(InfiniteRelation *r, size_t count);

#endif // INFINITE_RELATION_H
