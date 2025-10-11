#ifndef FFI_H
#define FFI_H

#include "arithmetic_relations.h"
#include "attribute.h"
#include "cardinality.h"
#include "infinite_relation.h"
#include "join.h"
#include "primitive_relations.h"
#include "relation.h"
#include "set.h"
#include "tuple.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Set operations */

extern Set *set_create(SetCompareFn cmp, SetFreeFn freer);
extern void set_destroy(Set *set);
extern int set_add(Set *set, void *elem);
extern int set_remove(Set *set, const void *elem);
extern int set_contains(const Set *set, const void *elem);
extern size_t set_size(const Set *set);
extern void set_foreach(const Set *set, SetIterFn fn, void *userdata);

/* Attribute operations */

extern Attribute *attribute_create(const char *name, AttributeType type, void *value);
extern void attribute_destroy(Attribute *attr);
extern void attribute_print(const Attribute *attr);

/* Tuple operations */

extern Tuple *tuple_create(void);
extern void tuple_destroy(Tuple *t);
extern int tuple_add_attribute(Tuple *t, Attribute *attr);
extern void tuple_print(const Tuple *t);
extern Attribute *tuple_find_attribute(Tuple *t, const char *name);
extern int tuple_equals(Tuple *a, Tuple *b);

/* Cardinality operations */

extern Cardinality cardinality_finite(uint64_t count);
extern Cardinality cardinality_infinite(CardinalityType type);
extern int cardinality_is_finite(Cardinality c);
extern int cardinality_is_infinite(Cardinality c);
extern void cardinality_print(Cardinality c);
extern Cardinality cardinality_product(Cardinality a, Cardinality b);

/* Relation operations */

extern Relation *relation_project(const Relation *r, const char **attr_names, size_t num_attrs,
                                  const char *new_name);
extern Relation *relation_create(const char *name);
extern int relation_add_tuple(Relation *r, Tuple *t);
extern void relation_destroy(Relation *r);
extern void relation_print(const Relation *r);
extern Tuple *relation_find_tuple(Relation *r, Tuple *t);
extern Relation *relation_create_with_cardinality(const char *name, Cardinality card);
extern void relation_print_with_cardinality(const Relation *r);
extern void relation_update_cardinality(Relation *r);

/* Infinite Relation operations */

extern InfiniteRelation *infinite_relation_create(const char *name, TupleGeneratorFn fn,
                                                  void *userdata);
extern InfiniteRelation *infinite_relation_create_with_cardinality(const char *name,
                                                                   TupleGeneratorFn fn,
                                                                   void *userdata,
                                                                   Cardinality card);
extern void infinite_relation_destroy(InfiniteRelation *r);
extern Tuple *infinite_relation_tuple_at(InfiniteRelation *r, size_t n);
extern void infinite_relation_print_prefix(InfiniteRelation *r, size_t count);
extern void infinite_relation_print_prefix_with_cardinality(InfiniteRelation *r, size_t count);
extern Tuple *infinite_relation_find_tuple(InfiniteRelation *r, Tuple *target);
extern InfiniteRelationIterator *infinite_relation_iterator_create(InfiniteRelation *r);
extern Tuple *infinite_relation_iterator_next(InfiniteRelationIterator *iter);
extern void infinite_relation_iterator_destroy(InfiniteRelationIterator *iter);

/* Join operations */

extern Relation *relation_join(Relation *left, Relation *right, JoinPredicateFn predicate,
                               void *userdata, const char *result_name);
extern InfiniteRelation *infinite_relation_join(InfiniteRelation *left, InfiniteRelation *right,
                                                JoinPredicateFn predicate, void *userdata,
                                                const char *result_name,
                                                Cardinality result_cardinality);
extern Tuple *tuple_merge(Tuple *left, Tuple *right);

/* Primitive Relations */

extern Tuple *successor_generator(size_t n, void *userdata);
extern Tuple *natural_generator(size_t n, void *userdata);
extern Tuple *integer_generator(size_t n, void *userdata);

/* Arithmetic Relations */

extern Tuple *addition_generator(size_t n, void *userdata);
extern Tuple *subtraction_generator(size_t n, void *userdata);
extern Tuple *multiplication_generator(size_t n, void *userdata);
extern Tuple *division_generator(size_t n, void *userdata);
extern void cantor_to_integer_pair(size_t n, int64_t *x, int64_t *y);
extern InfiniteRelation *create_addition_relation(void);
extern InfiniteRelation *create_subtraction_relation(void);
extern InfiniteRelation *create_multiplication_relation(void);
extern InfiniteRelation *create_division_relation(void);

#ifdef __cplusplus
}
#endif

#endif // FFI_H
