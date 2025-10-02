/**
 * @file main.c
 * @brief Example usage and entry point for the Relational Algebra Engine.
 *
 * Demonstrates creation and manipulation of relations, tuples, and attributes
 * according to strict relational algebra principles.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "attribute.h"
#include "relation.h"
#include "tuple.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "attribute.h"
#include "relation.h"
#include "tuple.h"
#include "infinite_relation.h"
#include "primitive_relations.h"

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

  relation_print(employees);

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
  InfiniteRelation *succ = infinite_relation_create("SUCCESSOR", (TupleGeneratorFn)successor_generator, NULL);
    printf("First 3 tuples of R = {(x, x+1) | x in N}:\n\n");
    infinite_relation_print_prefix(succ, 3);
    infinite_relation_destroy(succ);
}

void natural_relation_example() {
    InfiniteRelation *nat = infinite_relation_create("NATURAL", (TupleGeneratorFn)natural_generator, NULL);
    printf("First 3 tuples of R = N:\n\n");
    infinite_relation_print_prefix(nat, 3);
    infinite_relation_destroy(nat);
}

void integer_relation_example() {
    InfiniteRelation *inte = infinite_relation_create("INTEGER", (TupleGeneratorFn)integer_generator, NULL);
    printf("First 3 tuples of R = Z:\n\n");
    infinite_relation_print_prefix(inte, 3);
    infinite_relation_destroy(inte);
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
  successor_relation_example();
  natural_relation_example();
  integer_relation_example();
  return 0;
}
