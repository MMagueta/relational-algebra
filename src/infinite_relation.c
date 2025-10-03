#include <stdio.h>
#include <stdlib.h>

#include "infinite_relation.h"

InfiniteRelation *infinite_relation_create(const char *name,
                                           TupleGeneratorFn fn,
                                           void *userdata) {
    InfiniteRelation *r = malloc(sizeof(InfiniteRelation));
    r->name = name;
    r->gen_fn = fn;
    r->userdata = userdata;
    r->cardinality = cardinality_infinite(CARD_ALEPH_0); // Default to countably infinite
    return r;
}

InfiniteRelation *infinite_relation_create_with_cardinality(const char *name,
                                                            TupleGeneratorFn fn,
                                                            void *userdata,
                                                            Cardinality card) {
    InfiniteRelation *r = infinite_relation_create(name, fn, userdata);
    r->cardinality = card;
    return r;
}

void infinite_relation_destroy(InfiniteRelation *r) {
    if (r) free(r);
}

Tuple *infinite_relation_tuple_at(InfiniteRelation *r, size_t n) {
    if (!r || !r->gen_fn) return NULL;
    return r->gen_fn(n, r->userdata);
}

void infinite_relation_print_prefix(InfiniteRelation *r, size_t count) {
    if (!r) return;
    printf("Relation %s {\n", r->name);
    for (size_t i = 0; i < count; i++) {
        Tuple *t = infinite_relation_tuple_at(r, i);
        if (t) {
            printf(" ");
            tuple_print(t);
            tuple_destroy(t); // free after printing
        }
    }
    printf("}\n");
}

void infinite_relation_print_prefix_with_cardinality(InfiniteRelation *r, size_t count) {
    if (!r) return;
    printf("Relation %s (", r->name);
    cardinality_print(r->cardinality);
    printf(") {\n");
    for (size_t i = 0; i < count; i++) {
        Tuple *t = infinite_relation_tuple_at(r, i);
        if (t) {
            printf(" ");
            tuple_print(t);
            tuple_destroy(t); // free after printing
        }
    }
    printf("}\n");
}
