#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tuple.h"

static int attribute_cmp_name(const void *a, const void *b) {
  const Attribute *attr_a = a;
  const Attribute *attr_b = b;
  return strcmp(attr_a->name, attr_b->name);
}

static void attribute_free(void *a) { attribute_destroy((Attribute *)a); }

Tuple *tuple_create(void) { return set_create(attribute_cmp_name, attribute_free); }

void tuple_destroy(Tuple *t) { set_destroy(t); }

int tuple_add_attribute(Tuple *t, Attribute *attr) { return set_add(t, attr); }

static void print_attr_cb(void *element, void *userdata) {
  (void)userdata;
  printf("  ");
  attribute_print((Attribute *)element);
}

void tuple_print(const Tuple *t) {
  printf("Tuple {\n");
  set_foreach(t, print_attr_cb, NULL);
  printf("}\n");
}

typedef struct {
  const char *name;
  Attribute *found;
} FindAttributeContext;

static void find_attr_cb(void *element, void *userdata) {
  Attribute *attr = (Attribute *)element;
  FindAttributeContext *ctx = (FindAttributeContext *)userdata;
  if (strcmp(attr->name, ctx->name) == 0) {
    ctx->found = attr;
  }
}

Attribute *tuple_find_attribute(Tuple *t, const char *name) {
  FindAttributeContext ctx = {.name = name, .found = NULL};
  set_foreach(t, find_attr_cb, &ctx);
  return ctx.found;
}

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

static void check_attribute_cb(void *element, void *userdata) {
  Attribute *attr_a = (Attribute *)element;
  TupleEqualsContext *ctx = (TupleEqualsContext *)userdata;

  Attribute *attr_b = tuple_find_attribute(ctx->other, attr_a->name);
  if (!attr_b || !attribute_equals(attr_a, attr_b)) {
    ctx->matched = 0;
  }
}

int tuple_equals(Tuple *a, Tuple *b) {
  if (set_size(a) != set_size(b))
    return 0;
  TupleEqualsContext ctx = {.other = b, .matched = 1};
  set_foreach(a, check_attribute_cb, &ctx);
  return ctx.matched;
}
