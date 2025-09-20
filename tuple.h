#ifndef TUPLE_H
#define TUPLE_H

#include "set.h"
#include "attribute.h"

typedef Set Tuple;
Tuple *tuple_create(void);
void tuple_destroy(Tuple *t);
int tuple_add_attribute(Tuple *t, Attribute *attr);
void tuple_print(const Tuple *t);

#endif // TUPLE_H
