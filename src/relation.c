/**
 * @file relation.c
 * @brief Relation implementation for the Relational Algebra Engine.
 *
 * Provides creation, destruction, and manipulation of relations (for now, simply sets of tuples),
 * adhering to the strict theoretical model of relational algebra.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "relation.h"

/* Compare tuples by their attributes — for simplicity, we'll compare by pointer equality here.
   Later we could define a real "tuple equal" check. */
/**
 * @brief Compare two tuples by pointer equality.
 *
 * @param a Pointer to first tuple.
 * @param b Pointer to second tuple.
 * @return 0 if equal, <0 if a < b, >0 if a > b.
 */
static int tuple_cmp(const void *a, const void *b) {
  const Tuple *t1 = (const Tuple *)a;
  const Tuple *t2 = (const Tuple *)b;
  return (t1 == t2) ? 0 : ((t1 < t2) ? -1 : 1);
}

/**
 * @brief Free a tuple by destroying it.
 *
 * @param t Pointer to the tuple to free.
 */
static void tuple_free(void *t) { tuple_destroy((Tuple *)t); }

/**
 * @brief Create a new Relation.
 *
 * @param name Name of the relation (copied).
 * @return Pointer to the new Relation, or NULL on failure.
 */
Relation *relation_create(const char *name) {
  Relation *r = malloc(sizeof(Relation));
  if (!r)
    return NULL;
  r->name = strdup(name);
  r->tuples = set_create(tuple_cmp, tuple_free);
  r->cardinality = cardinality_finite(0);
  return r;
}

/**
 * @brief Create a new Relation with specified cardinality.
 *
 * @param name Name of the relation (copied).
 * @param card Cardinality of the relation.
 * @return Pointer to the new Relation, or NULL on failure.
 */
Relation *relation_create_with_cardinality(const char *name, Cardinality card) {
  Relation *r = relation_create(name);
  if (r) {
    r->cardinality = card;
  }
  return r;
}

/**
 * @brief Destroy a Relation and free its memory.
 *
 * @param r Pointer to the Relation to destroy. Safe to pass NULL.
 */
void relation_destroy(Relation *r) {
  if (!r)
    return;
  free(r->name);
  set_destroy(r->tuples);
  free(r);
}

/*
 * @brief Add a tuple to a relation.
 *
 * @param r Pointer to the Relation.
 * @param t Pointer to the Tuple to add.
 * @return 1 if added, 0 if already present, -1 on error.
 */
int relation_add_tuple(Relation *r, Tuple *t) {
  int result = set_add(r->tuples, t);
  if (result == 1 && cardinality_is_finite(r->cardinality)) {
    // Update finite cardinality
    r->cardinality.finite_count = set_size(r->tuples);
  }
  return result;
}

/**
 * @brief Update cardinality based on current tuple count.
 *
 * @param r Pointer to the Relation.
 */
void relation_update_cardinality(Relation *r) {
  if (cardinality_is_finite(r->cardinality)) {
    r->cardinality.finite_count = set_size(r->tuples);
  }
}

/**
 * @brief Print a tuple (used as callback for set_foreach).
 *
 * @param element Pointer to the tuple.
 * @param userdata Unused.
 */
static void print_tuple(void *element, void *userdata) {
  (void)userdata; /* unused */
  Tuple *t = (Tuple *)element;
  printf(" ");
  tuple_print(t);
}

/**
 * @brief Print a Relation to stdout.
 *
 * @param r Pointer to the Relation to print.
 */
void relation_print(const Relation *r) {
  printf("Relation %s {\n", r->name);
  set_foreach(r->tuples, print_tuple, NULL);
  printf("}\n");
}

/**
 * @brief Print a Relation with its cardinality.
 *
 * @param r Pointer to the Relation to print.
 */
void relation_print_with_cardinality(const Relation *r) {
  printf("Relation %s (", r->name);
  cardinality_print(r->cardinality);
  printf(") {\n");
  set_foreach(r->tuples, print_tuple, NULL);
  printf("}\n");
}

typedef struct {
  Tuple *target;
  Tuple *found;
} FindTupleContext;

/**
 * @brief Callback to check if a tuple matches the target (used in find).
 *
 * @param element Pointer to the candidate tuple.
 * @param userdata Pointer to FindTupleContext.
 */
static void check_tuple_cb(void *element, void *userdata) {
  Tuple *candidate = (Tuple *)element;
  FindTupleContext *ctx = (FindTupleContext *)userdata;

  if (tuple_equals(candidate, ctx->target)) {
    ctx->found = candidate;
  }
}

/**
 * @brief Find a tuple in a relation matching the target.
 *
 * @param r Pointer to the Relation.
 * @param target Pointer to the Tuple to find.
 * @return Pointer to the found Tuple, or NULL if not found.
 */
Tuple *relation_find_tuple(Relation *r, Tuple *target) {
  FindTupleContext ctx = {.target = target, .found = NULL};
  set_foreach(r->tuples, check_tuple_cb, &ctx);
  return ctx.found;
}
