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

/**
 * @brief Entry point for the relational algebra engine demo.
 *
 * Demonstrates creation of relations, tuples, and attributes, and prints results.
 *
 * @return int Exit status code (0 for success).
 */
int main(void) {
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
