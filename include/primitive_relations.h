#ifndef PRIMITIVE_RELATIONS_H
#define PRIMITIVE_RELATIONS_H

#include "tuple.h"

/* Generator for successor relation R = {(x, x+1) | x ∈ N} */
Tuple *successor_generator(size_t n, void *userdata);

/* Generator for naturals N */
Tuple *natural_generator(size_t n, void *userdata);

/* Generator for integers Z */
Tuple *integer_generator(size_t n, void *userdata);

#endif // PRIMITIVE_RELATIONS_H
