#ifndef RELATION_H
#define RELATION_H

#include "set.h"
#include "tuple.h"

typedef struct {
  char *name;
  Set *tuples;
} Relation;

Relation *relation_create(const char *name);
int relation_add_tuple(Relation *r, Tuple *t);
void relation_destroy(Relation *r);
void relation_print(const Relation *r);
Tuple *relation_find_tuple(Relation *r, Tuple *t);

#endif // RELATION_H
