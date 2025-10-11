#ifndef RELATION_H
#define RELATION_H

#include "cardinality.h"
#include "set.h"
#include "tuple.h"

typedef struct {
  char *name;
  Set *tuples;
  Cardinality cardinality;
} Relation;

/**
 * @brief Project a relation onto a subset of attributes.
 *
 * @param r Pointer to the input Relation.
 * @param attr_names Array of attribute names to keep.
 * @param num_attrs Number of attribute names in attr_names.
 * @param new_name Name for the projected relation.
 * @return Pointer to a new Relation containing only the specified attributes.
 *
 * Example usage:
 * @code{.c}
 *   const char *attrs[] = {"id", "name"};
 *   Relation *proj = relation_project(employees, attrs, 2, "EmpNames");
 *   relation_print(proj);
 *   relation_destroy(proj);
 * @endcode
 */
Relation *relation_project(const Relation *r, const char **attr_names, size_t num_attrs,
                           const char *new_name);

Relation *relation_create(const char *name);
int relation_add_tuple(Relation *r, Tuple *t);
void relation_destroy(Relation *r);
void relation_print(const Relation *r);
Tuple *relation_find_tuple(Relation *r, Tuple *t);

Relation *relation_create_with_cardinality(const char *name, Cardinality card);
void relation_print_with_cardinality(const Relation *r);
void relation_update_cardinality(Relation *r);

#endif // RELATION_H
