#ifndef TUPLE_H
#define TUPLE_H

#include "attribute.h"
#include "set.h"

typedef Set Tuple;

Tuple *tuple_create(void);
void tuple_destroy(Tuple *t);
int tuple_add_attribute(Tuple *t, Attribute *attr);
void tuple_print(const Tuple *t);
Attribute *tuple_find_attribute(Tuple *t, const char *name);
int tuple_equals(Tuple *a, Tuple *b);
#endif // TUPLE_H
