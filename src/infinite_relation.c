#include <stdio.h>
#include <stdlib.h>

#include "infinite_relation.h"

InfiniteRelation *infinite_relation_create(const char *name, TupleGeneratorFn fn, void *userdata) {
  InfiniteRelation *r = malloc(sizeof(InfiniteRelation));
  r->name = name;
  r->gen_fn = fn;
  r->userdata = userdata;
  r->cardinality = cardinality_infinite(CARD_ALEPH_0); // Default to countably infinite
  return r;
}

InfiniteRelation *infinite_relation_create_with_cardinality(const char *name, TupleGeneratorFn fn,
                                                            void *userdata, Cardinality card) {
  InfiniteRelation *r = infinite_relation_create(name, fn, userdata);
  r->cardinality = card;
  return r;
}

void infinite_relation_destroy(InfiniteRelation *r) {
  if (r)
    free(r);
}

Tuple *infinite_relation_tuple_at(InfiniteRelation *r, size_t n) {
  if (!r || !r->gen_fn)
    return NULL;
  return r->gen_fn(n, r->userdata);
}

void infinite_relation_print_prefix(InfiniteRelation *r, size_t count) {
  if (!r)
    return;
  printf("Relation %s {\n", r->name);
  for (size_t i = 0; i < count; i++) {
    Tuple *t = infinite_relation_tuple_at(r, i);
    if (t) {
      printf(" ");
      tuple_print(t);
      tuple_destroy(t); // free after printing
    }
  }
  printf("}\n");
}

void infinite_relation_print_prefix_with_cardinality(InfiniteRelation *r, size_t count) {
  if (!r)
    return;
  printf("Relation %s (", r->name);
  cardinality_print(r->cardinality);
  printf(") {\n");
  for (size_t i = 0; i < count; i++) {
    Tuple *t = infinite_relation_tuple_at(r, i);
    if (t) {
      printf(" ");
      tuple_print(t);
      tuple_destroy(t); // free after printing
    }
  }
  printf("}\n");
}

InfiniteRelationIterator *infinite_relation_iterator_create(InfiniteRelation *r) {
  if (!r)
    return NULL;
  InfiniteRelationIterator *iter = malloc(sizeof(InfiniteRelationIterator));
  if (!iter)
    return NULL;
  iter->relation = r;
  iter->current_index = 0;
  iter->exhausted = 0;
  return iter;
}

Tuple *infinite_relation_iterator_next(InfiniteRelationIterator *iter) {
  if (!iter || iter->exhausted)
    return NULL;
  Tuple *t = infinite_relation_tuple_at(iter->relation, iter->current_index);
  if (!t) {
    iter->exhausted = 1;
    return NULL;
  }
  iter->current_index++;
  return t;
}

void infinite_relation_iterator_destroy(InfiniteRelationIterator *iter) { free(iter); }

Tuple *infinite_relation_find_tuple(InfiniteRelation *r, Tuple *target) {
  if (!r || !target)
    return NULL;

  InfiniteRelationIterator *iter = infinite_relation_iterator_create(r);
  if (!iter)
    return NULL;

  Tuple *found = NULL;
  Tuple *candidate;

  // For infinite relations, we still need some practical limit
  // but now it's explicit and uses the iterator pattern
  const size_t SEARCH_LIMIT = cardinality_is_infinite(r->cardinality) ? 1000000 : SIZE_MAX;
  size_t count = 0;

  while ((candidate = infinite_relation_iterator_next(iter)) != NULL && count < SEARCH_LIMIT) {
    if (tuple_equals(candidate, target)) {
      found = candidate;
      break;
    }
    tuple_destroy(candidate);
    count++;
  }

  infinite_relation_iterator_destroy(iter);
  return found;
}
