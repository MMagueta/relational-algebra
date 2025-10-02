#include <stdint.h>
#include <stdlib.h>

#include "primitive_relations.h"

/* Generator for successor relation R = {(x, x+1) | x ∈ N} */
Tuple *successor_generator(size_t n, void *userdata) {
    (void)userdata; // to avoid annoying warning because I don't use it
    Tuple *t = tuple_create();
    int *x = malloc(sizeof(int));
    int *y = malloc(sizeof(int));
    *x = (int)n;
    *y = (int)n + 1;
    tuple_add_attribute(t, attribute_create("in", ATTR_INT, x));
    tuple_add_attribute(t, attribute_create("out", ATTR_INT, y));
    return t;
}

/* Generator for naturals N */
Tuple *natural_generator(size_t n, void *userdata) {
    (void)userdata; // to avoid annoying warning because I don't use it
    Tuple *t = tuple_create();
    uint64_t *x = malloc(sizeof(uint64_t));
    *x = (uint64_t)n;
    tuple_add_attribute(t, attribute_create("n", ATTR_INT, x));
    return t;
}

/* Generator for integers Z */
Tuple *integer_generator(size_t n, void *userdata) {
    (void)userdata; // to avoid annoying warning because I don't use it
    Tuple *t = tuple_create();
    int64_t *x = malloc(sizeof(int64_t));
    *x = (int64_t)n;
    tuple_add_attribute(t, attribute_create("z", ATTR_INT, x));
    return t;
}

