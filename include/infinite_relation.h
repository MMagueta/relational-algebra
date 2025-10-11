#ifndef INFINITE_RELATION_H
#define INFINITE_RELATION_H

#include "cardinality.h"
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
  Cardinality cardinality;
} InfiniteRelation;

typedef struct {
  InfiniteRelation *relation;
  size_t current_index;
  int exhausted;
} InfiniteRelationIterator;

/**
 * Create an infinite relation defined by a generator function.
 */
InfiniteRelation *infinite_relation_create(const char *name, TupleGeneratorFn fn, void *userdata);

/**
 * Create an infinite relation with explicit cardinality.
 */
InfiniteRelation *infinite_relation_create_with_cardinality(const char *name, TupleGeneratorFn fn,
                                                            void *userdata, Cardinality card);

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

/**
 * Print first `count` tuples with cardinality info.
 */
void infinite_relation_print_prefix_with_cardinality(InfiniteRelation *r, size_t count);

/**
 * Find a tuple in the infinite relation that matches the target.
 * Returns a newly allocated Tuple* if found, NULL otherwise.
 * Note: Searches up to a limit to avoid infinite loops.
 */
Tuple *infinite_relation_find_tuple(InfiniteRelation *r, Tuple *target);

InfiniteRelationIterator *infinite_relation_iterator_create(InfiniteRelation *r);

Tuple *infinite_relation_iterator_next(InfiniteRelationIterator *iter);

void infinite_relation_iterator_destroy(InfiniteRelationIterator *iter);

#endif // INFINITE_RELATION_H
