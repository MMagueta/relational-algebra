/**
 * @file tuple.c
 * @brief Tuple implementation for the Relational Algebra Engine.
 *
 * Handles creation, destruction, and comparison of tuples, which are sets of attributes
 * and serve as the elements of relations in the relational model.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tuple.h"

/**
 * @brief Compare two attributes by name.
 *
 * @param a Pointer to first Attribute.
 * @param b Pointer to second Attribute.
 * @return <0, 0, >0 as a <, ==, > b by name.
 */
static int attribute_cmp_name(const void *a, const void *b) {
  const Attribute *attr_a = a;
  const Attribute *attr_b = b;
  return strcmp(attr_a->name, attr_b->name);
}

/**
 * @brief Free an attribute by destroying it.
 *
 * @param a Pointer to the Attribute to free.
 */
static void attribute_free(void *a) { attribute_destroy((Attribute *)a); }

/**
 * @brief Create a new Tuple.
 *
 * @return Pointer to new Tuple, or NULL on failure.
 */
Tuple *tuple_create(void) { return set_create(attribute_cmp_name, attribute_free); }

/**
 * @brief Destroy a Tuple and free its memory.
 *
 * @param t Pointer to the Tuple to destroy.
 */
void tuple_destroy(Tuple *t) { set_destroy(t); }

/**
 * @brief Add an Attribute to a Tuple.
 *
 * @param t Pointer to the Tuple.
 * @param attr Pointer to the Attribute to add.
 * @return 1 if added, 0 if already present, -1 on error.
 */
int tuple_add_attribute(Tuple *t, Attribute *attr) { return set_add(t, attr); }

/**
 * @brief Print an Attribute (used as callback for set_foreach).
 *
 * @param element Pointer to the Attribute.
 * @param userdata Unused.
 */
static void print_attr_cb(void *element, void *userdata) {
  (void)userdata;
  printf("  ");
  attribute_print((Attribute *)element);
}

/**
 * @brief Print a Tuple to stdout.
 *
 * @param t Pointer to the Tuple to print.
 */
void tuple_print(const Tuple *t) {
  printf("Tuple {\n");
  set_foreach(t, print_attr_cb, NULL);
  printf("}\n");
}

typedef struct {
  const char *name;
  Attribute *found;
} FindAttributeContext;

/**
 * @brief Callback to find an Attribute by name (used in tuple_find_attribute).
 *
 * @param element Pointer to the Attribute.
 * @param userdata Pointer to FindAttributeContext.
 */
static void find_attr_cb(void *element, void *userdata) {
  Attribute *attr = (Attribute *)element;
  FindAttributeContext *ctx = (FindAttributeContext *)userdata;
  if (strcmp(attr->name, ctx->name) == 0) {
    ctx->found = attr;
  }
}

/**
 * @brief Find an Attribute in a Tuple by name.
 *
 * @param t Pointer to the Tuple.
 * @param name Name of the Attribute to find.
 * @return Pointer to the found Attribute, or NULL if not found.
 */
Attribute *tuple_find_attribute(Tuple *t, const char *name) {
  FindAttributeContext ctx = {.name = name, .found = NULL};
  set_foreach(t, find_attr_cb, &ctx);
  return ctx.found;
}

/**
 * @brief Check if two Attributes are equal (name and value).
 *
 * @param a Pointer to first Attribute.
 * @param b Pointer to second Attribute.
 * @return 1 if equal, 0 otherwise.
 */
static int attribute_equals(const Attribute *a, const Attribute *b) {
  if (a->type != b->type)
    return 0;
  if (strcmp(a->name, b->name) != 0)
    return 0;

  switch (a->type) {
  case ATTR_INT:
    return *(int *)a->value == *(int *)b->value;
  case ATTR_DOUBLE:
    return *(double *)a->value == *(double *)b->value;
  case ATTR_STRING:
    return strcmp((char *)a->value, (char *)b->value) == 0;
  case ATTR_SET:
    return set_size((Set *)a->value) == set_size((Set *)b->value);
  default:
    return 0;
  }
}

typedef struct {
  Tuple *other;
  int matched;
} TupleEqualsContext;

/**
 * @brief Callback to check if an Attribute matches one in another Tuple.
 *
 * @param element Pointer to the Attribute.
 * @param userdata Pointer to TupleEqualsContext.
 */
static void check_attribute_cb(void *element, void *userdata) {
  Attribute *attr_a = (Attribute *)element;
  TupleEqualsContext *ctx = (TupleEqualsContext *)userdata;

  Attribute *attr_b = tuple_find_attribute(ctx->other, attr_a->name);
  if (!attr_b || !attribute_equals(attr_a, attr_b)) {
    ctx->matched = 0;
  }
}

/**
 * @brief Check if two Tuples are equal (all attributes match).
 *
 * @param a Pointer to first Tuple.
 * @param b Pointer to second Tuple.
 * @return 1 if equal, 0 otherwise.
 */
int tuple_equals(Tuple *a, Tuple *b) {
  if (set_size(a) != set_size(b))
    return 0;
  TupleEqualsContext ctx = {.other = b, .matched = 1};
  set_foreach(a, check_attribute_cb, &ctx);
  return ctx.matched;
}
