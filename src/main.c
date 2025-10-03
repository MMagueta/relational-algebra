/**
 * @file main.c
 * @brief Example usage and entry point for the Relational Algebra Engine.
 *
 * Demonstrates creation and manipulation of relations, tuples, and attributes
 * according to strict relational algebra principles, including join operations.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "attribute.h"
#include "cardinality.h"
#include "infinite_relation.h"
#include "join.h"
#include "primitive_relations.h"
#include "relation.h"
#include "tuple.h"

/**
 * Join predicate: equality on "n" attribute.
 * Since tuple_merge prefixes attributes, we need to check left_n and right_n.
 */
int join_predicate_equality(Tuple *left, Tuple *right, void *userdata) {
  (void)userdata;
  Attribute *left_n = tuple_find_attribute(left, "n");
  Attribute *right_n = tuple_find_attribute(right, "n");

  if (!left_n || !right_n)
    return 0;
  if (left_n->type != ATTR_INT || right_n->type != ATTR_INT)
    return 0;

  return *(int *)left_n->value == *(int *)right_n->value;
}

/**
 * Join predicate: left.n < right.n
 */
int join_predicate_less_than(Tuple *left, Tuple *right, void *userdata) {
  (void)userdata;
  Attribute *left_n = tuple_find_attribute(left, "n");
  Attribute *right_n = tuple_find_attribute(right, "n");

  if (!left_n || !right_n)
    return 0;
  if (left_n->type != ATTR_INT || right_n->type != ATTR_INT)
    return 0;

  return *(int *)left_n->value < *(int *)right_n->value;
}

int relational_example(void) {
  Relation *employees = relation_create("Employees");

  Tuple *t1 = tuple_create();
  int *id1 = malloc(sizeof(int));
  *id1 = 1;
  tuple_add_attribute(t1, attribute_create("id", ATTR_INT, id1));
  tuple_add_attribute(t1, attribute_create("name", ATTR_STRING, strdup("Alice")));
  double *salary1 = malloc(sizeof(double));
  *salary1 = 50000.0;
  tuple_add_attribute(t1, attribute_create("salary", ATTR_DOUBLE, salary1));
  relation_add_tuple(employees, t1);

  Tuple *t2 = tuple_create();
  int *id2 = malloc(sizeof(int));
  *id2 = 2;
  tuple_add_attribute(t2, attribute_create("id", ATTR_INT, id2));
  tuple_add_attribute(t2, attribute_create("name", ATTR_STRING, strdup("Bob")));
  double *salary2 = malloc(sizeof(double));
  *salary2 = 60000.0;
  tuple_add_attribute(t2, attribute_create("salary", ATTR_DOUBLE, salary2));
  relation_add_tuple(employees, t2);

  relation_print_with_cardinality(employees);

  Tuple *found = relation_find_tuple(employees, t2);
  if (found) {
    printf("\nFound tuple (full match):\n");
    tuple_print(found);
  } else {
    printf("\nTuple not found.\n");
  }
  relation_destroy(employees);
  return 0;
}

void successor_relation_example() {
  InfiniteRelation *succ = infinite_relation_create_with_cardinality(
      "SUCCESSOR", (TupleGeneratorFn)successor_generator, NULL,
      cardinality_infinite(CARD_ALEPH_0));
  printf("First 3 tuples of R = {(x, x+1) | x in N}:\n\n");
  infinite_relation_print_prefix_with_cardinality(succ, 3);
  infinite_relation_destroy(succ);
}

void natural_relation_example() {
  InfiniteRelation *nat = infinite_relation_create_with_cardinality(
      "NATURAL", (TupleGeneratorFn)natural_generator, NULL,
      cardinality_infinite(CARD_ALEPH_0));
  printf("First 3 tuples of N:\n\n");
  infinite_relation_print_prefix_with_cardinality(nat, 3);
  infinite_relation_destroy(nat);
}

void integer_relation_example() {
  InfiniteRelation *inte = infinite_relation_create_with_cardinality(
      "INTEGER", (TupleGeneratorFn)integer_generator, NULL,
      cardinality_infinite(CARD_ALEPH_0));
  printf("First 3 tuples of Z:\n\n");
  infinite_relation_print_prefix_with_cardinality(inte, 3);
  infinite_relation_destroy(inte);
}

void finite_join_example() {
  printf("\n=== Finite Join Example ===\n");
  printf("Join {1,2,3} with {2,3,4} on equality\n\n");

  // Create first relation: {1, 2, 3}
  Relation *r1 = relation_create("R1");
  for (int i = 1; i <= 3; i++) {
    Tuple *t = tuple_create();
    int *val = malloc(sizeof(int));
    *val = i;
    tuple_add_attribute(t, attribute_create("n", ATTR_INT, val));
    relation_add_tuple(r1, t);
  }

  // Create second relation: {2, 3, 4}
  Relation *r2 = relation_create("R2");
  for (int i = 2; i <= 4; i++) {
    Tuple *t = tuple_create();
    int *val = malloc(sizeof(int));
    *val = i;
    tuple_add_attribute(t, attribute_create("n", ATTR_INT, val));
    relation_add_tuple(r2, t);
  }

  printf("R1:\n");
  relation_print_with_cardinality(r1);
  printf("\nR2:\n");
  relation_print_with_cardinality(r2);

  // Perform join
  Relation *result = relation_join(r1, r2, join_predicate_equality, NULL, "R1 ⋈ R2");

  printf("\nJoin Result (equality):\n");
  relation_print_with_cardinality(result);

  relation_destroy(r1);
  relation_destroy(r2);
  relation_destroy(result);
}

void infinite_join_example_equality() {
  printf("\n=== Infinite Join Example: N ⋈ N (equality) ===\n");
  printf("This produces a countably infinite relation (ℵ₀)\n");
  printf("Each tuple has left_n = right_n\n\n");

  InfiniteRelation *nat1 = infinite_relation_create_with_cardinality(
      "N₁", (TupleGeneratorFn)natural_generator, NULL,
      cardinality_infinite(CARD_ALEPH_0));

  InfiniteRelation *nat2 = infinite_relation_create_with_cardinality(
      "N₂", (TupleGeneratorFn)natural_generator, NULL,
      cardinality_infinite(CARD_ALEPH_0));

  // Join with equality predicate
  InfiniteRelation *joined = infinite_relation_join(
      nat1, nat2, join_predicate_equality, NULL, "N ⋈ N (n₁ = n₂)",
      cardinality_infinite(CARD_ALEPH_0));

  printf("First 5 tuples of N ⋈ N where n₁ = n₂:\n");
  printf("Expected: (0,0), (1,1), (2,2), (3,3), (4,4)\n\n");
  infinite_relation_print_prefix_with_cardinality(joined, 5);

  infinite_relation_destroy(nat1);
  infinite_relation_destroy(nat2);
  infinite_relation_destroy(joined);
}

void infinite_join_example_less_than() {
  printf("\n=== Infinite Join Example: N ⋈ N (less than) ===\n");
  printf("This produces a countably infinite relation (ℵ₀)\n");
  printf("Each tuple has left_n < right_n\n\n");

  InfiniteRelation *nat1 = infinite_relation_create_with_cardinality(
      "N₁", (TupleGeneratorFn)natural_generator, NULL,
      cardinality_infinite(CARD_ALEPH_0));

  InfiniteRelation *nat2 = infinite_relation_create_with_cardinality(
      "N₂", (TupleGeneratorFn)natural_generator, NULL,
      cardinality_infinite(CARD_ALEPH_0));

  // Join with less-than predicate
  InfiniteRelation *joined = infinite_relation_join(
      nat1, nat2, join_predicate_less_than, NULL, "N ⋈ N (n₁ < n₂)",
      cardinality_infinite(CARD_ALEPH_0));

  printf("First 10 tuples of N ⋈ N where n₁ < n₂:\n");
  printf("Expected: (0,1), (0,2), (1,2), (0,3), (1,3), (2,3), ...\n\n");
  infinite_relation_print_prefix_with_cardinality(joined, 10);

  printf("\nNote: The result is infinite because for every natural number,\n");
  printf("there are infinitely many greater natural numbers.\n");

  infinite_relation_destroy(nat1);
  infinite_relation_destroy(nat2);
  infinite_relation_destroy(joined);
}

/**
 * Generator for finite relation as if it were infinite (cycles through elements).
 * This allows us to use the infinite join infrastructure.
 */
typedef struct {
  Relation *finite_rel;
  size_t size;
} FiniteAsInfiniteContext;

/**
 * Context for copying attributes from one tuple to another.
 */
typedef struct {
  Tuple *target;
} CopyAttrContext;

/**
 * Callback to copy an attribute to a target tuple.
 */
static void copy_attr_cb(void *attr_elem, void *copy_userdata) {
  Attribute *attr = (Attribute *)attr_elem;
  CopyAttrContext *cc = (CopyAttrContext *)copy_userdata;
  
  void *value_copy = NULL;
  switch (attr->type) {
    case ATTR_INT: {
      int *v = malloc(sizeof(int));
      *v = *(int *)attr->value;
      value_copy = v;
      break;
    }
    case ATTR_DOUBLE: {
      double *v = malloc(sizeof(double));
      *v = *(double *)attr->value;
      value_copy = v;
      break;
    }
    case ATTR_STRING:
      value_copy = strdup((char *)attr->value);
      break;
    default:
      break;
  }
  
  Attribute *attr_copy = attribute_create(attr->name, attr->type, value_copy);
  tuple_add_attribute(cc->target, attr_copy);
}

/**
 * Context for finding a tuple by index in a set.
 */
typedef struct {
  size_t target_index;
  size_t current_index;
  Tuple *found;
} FindByIndexContext;

/**
 * Callback to find a tuple at a specific index.
 */
static void find_by_index_cb(void *element, void *userdata) {
  FindByIndexContext *fc = (FindByIndexContext *)userdata;
  if (fc->current_index == fc->target_index && !fc->found) {
    // Copy the tuple so we can return it
    Tuple *orig = (Tuple *)element;
    Tuple *copy = tuple_create();
    
    CopyAttrContext copy_ctx = {.target = copy};
    set_foreach(orig, copy_attr_cb, &copy_ctx);
    fc->found = copy;
  }
  fc->current_index++;
}

static Tuple *finite_as_infinite_generator(size_t n, void *userdata) {
  FiniteAsInfiniteContext *ctx = (FiniteAsInfiniteContext *)userdata;
  
  // Cycle through the finite relation's tuples
  size_t index = n % ctx->size;
  
  // We need to get the index-th tuple from the relation
  FindByIndexContext find_ctx = {.target_index = index, .current_index = 0, .found = NULL};
  set_foreach(ctx->finite_rel->tuples, find_by_index_cb, &find_ctx);
  return find_ctx.found;
}

void mixed_join_example() {
  printf("\n=== Mixed Join Example: {1,2,3} ⋈ N (less than) ===\n");
  printf("Finite relation joined with infinite relation\n");
  printf("Cardinality: finite × ℵ₀ with selective predicate = ℵ₀\n\n");

  // Create finite relation: {1, 2, 3}
  Relation *finite = relation_create("Finite");
  for (int i = 1; i <= 3; i++) {
    Tuple *t = tuple_create();
    int *val = malloc(sizeof(int));
    *val = i;
    tuple_add_attribute(t, attribute_create("n", ATTR_INT, val));
    relation_add_tuple(finite, t);
  }

  printf("Finite relation:\n");
  relation_print_with_cardinality(finite);

  // Create infinite natural numbers relation
  InfiniteRelation *nat = infinite_relation_create_with_cardinality(
      "N", (TupleGeneratorFn)natural_generator, NULL,
      cardinality_infinite(CARD_ALEPH_0));

  // Convert finite relation to "infinite" generator (cycles through)
  FiniteAsInfiniteContext *fin_ctx = malloc(sizeof(FiniteAsInfiniteContext));
  fin_ctx->finite_rel = finite;
  fin_ctx->size = set_size(finite->tuples);
  
  InfiniteRelation *finite_as_inf = infinite_relation_create_with_cardinality(
      "Finite", finite_as_infinite_generator, fin_ctx,
      cardinality_finite(3));

  printf("\nPerforming join where finite.n < N.n:\n");
  
  // Perform the join
  InfiniteRelation *joined = infinite_relation_join(
      finite_as_inf, nat, join_predicate_less_than, NULL, 
      "{1,2,3} ⋈ N (finite.n < N.n)",
      cardinality_infinite(CARD_ALEPH_0));

  printf("\nFirst 10 tuples of the join result:\n");
  printf("Expected: (1,2), (1,3), (2,3), (1,4), (2,4), (3,4), ...\n\n");
  infinite_relation_print_prefix_with_cardinality(joined, 10);

  printf("\nNote: This produces infinite results because each of the 3 finite elements\n");
  printf("joins with infinitely many naturals greater than it.\n");

  relation_destroy(finite);
  infinite_relation_destroy(nat);
  infinite_relation_destroy(finite_as_inf);
  infinite_relation_destroy(joined);
  free(fin_ctx);
}

/**
 * @brief Entry point for the relational algebra engine demo.
 *
 * Demonstrates creation of relations, tuples, and attributes, and prints results.
 *
 * @return int Exit status code (0 for success).
 */
int main(void) {
  relational_example();
  printf("\n");
  successor_relation_example();
  printf("\n");
  natural_relation_example();
  printf("\n");
  integer_relation_example();
  printf("\n");

  // Join examples
  finite_join_example();
  infinite_join_example_equality();
  infinite_join_example_less_than();
  mixed_join_example();

  return 0;
}
