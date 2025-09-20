#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "attribute.h"
#include "relation.h"
#include "tuple.h"

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
