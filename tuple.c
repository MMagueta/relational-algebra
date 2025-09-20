#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tuple.h"

static int attribute_cmp(const void *a, const void *b) {
    const Attribute *attr_a = (const Attribute *)a;
    const Attribute *attr_b = (const Attribute *)b;
    return strcmp(attr_a->name, attr_b->name);
}

static void attribute_free(void *a) {
    attribute_destroy((Attribute *)a);
}

Tuple *tuple_create(void) {
    return set_create(attribute_cmp, attribute_free);
}

void tuple_destroy(Tuple *t) {
    set_destroy(t);
}

int tuple_add_attribute(Tuple *t, Attribute *attr) {
    return set_add(t, attr);
}

static void print_attr(void *element, void *userdata) {
    (void)userdata; /* unused */
    Attribute *attr = (Attribute *)element;
    attribute_print(attr);
}

void tuple_print(const Tuple *t) {
    printf("Tuple {\n");
    set_foreach(t, print_attr, NULL);
    printf("}\n");
}
