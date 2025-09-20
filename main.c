#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "set.h"
#include "tuple.h"
#include "attribute.h"

int compare_str_tuple(const void *tuple_a, const void *tuple_b) {
    return strcmp((const char *)tuple_a, (const char *)tuple_b);
}

int compare_set_tuple(const void *tuple_a, const void *tuple_b) {
    const Set *set_a = (const Set *)tuple_a;
    const Set *set_b = (const Set *)tuple_b;
    if (set_size(set_a) < set_size(set_b)) return -1;
    if (set_size(set_a) > set_size(set_b)) return 1;
    return 0;
}

void free_str_tuple(void *tuple) { free(tuple); }
void free_set_tuple(void *tuple) { set_destroy((Set *)tuple); }

void print_str_tuple(void *tuple, void *ud) {
    /* (void)ud; */
    printf("%s, ", (char *)tuple);
}

void print_set_tuple(void *tuple, void *ud) {
    /* (void)ud; */
    Set *inner = (Set *)tuple;
    printf("( ");
    set_foreach(inner, print_str_tuple, NULL);
    printf(" )\n");
}

int main(void) {
    Tuple *t = tuple_create();
    int *id = malloc(sizeof(int));
    *id = 1;
    tuple_add_attribute(t, attribute_create("id", ATTR_INT, id));
    tuple_add_attribute(t, attribute_create("name", ATTR_STRING, strdup("Alice")));
    double *salary = malloc(sizeof(double));
    *salary = 50000.0;
    tuple_add_attribute(t, attribute_create("salary", ATTR_DOUBLE, salary));
    tuple_print(t);
    tuple_destroy(t);
    return 0;
}

int main2(void) {
    Set *fruits = set_create(compare_str_tuple, free_str_tuple);
    set_add(fruits, strdup("apple"));
    set_add(fruits, strdup("banana"));
    Set *berries = set_create(compare_str_tuple, free_str_tuple);
    set_add(berries, strdup("cherry"));
    Set *meta = set_create(compare_set_tuple, free_set_tuple);
    set_add(meta, fruits);
    set_add(meta, berries);
    printf("Set of sets:\n");
    set_foreach(meta, print_set_tuple, NULL);
    set_destroy(meta);
    return 0;
}
