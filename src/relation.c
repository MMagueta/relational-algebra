#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "relation.h"

/* Compare tuples by their attributes — for simplicity, we’ll compare by pointer equality here.
   Later we could define a real "tuple equal" check. */
static int tuple_cmp(const void *a, const void *b) {
  const Tuple *t1 = (const Tuple *)a;
  const Tuple *t2 = (const Tuple *)b;
  return (t1 == t2) ? 0 : ((t1 < t2) ? -1 : 1);
}

static void tuple_free(void *t) { tuple_destroy((Tuple *)t); }

Relation *relation_create(const char *name) {
  Relation *r = malloc(sizeof(Relation));
  if (!r)
    return NULL;
  r->name = strdup(name);
  r->tuples = set_create(tuple_cmp, tuple_free);
  return r;
}

void relation_destroy(Relation *r) {
  if (!r)
    return;
  free(r->name);
  set_destroy(r->tuples);
  free(r);
}

int relation_add_tuple(Relation *r, Tuple *t) { return set_add(r->tuples, t); }

static void print_tuple(void *element, void *userdata) {
  (void)userdata; /* unused */
  Tuple *t = (Tuple *)element;
  printf(" ");
  tuple_print(t);
}

void relation_print(const Relation *r) {
  printf("Relation %s {\n", r->name);
  set_foreach(r->tuples, print_tuple, NULL);
  printf("}\n");
}

typedef struct {
  Tuple *target;
  Tuple *found;
} FindTupleContext;

static void check_tuple_cb(void *element, void *userdata) {
  Tuple *candidate = (Tuple *)element;
  FindTupleContext *ctx = (FindTupleContext *)userdata;

  if (tuple_equals(candidate, ctx->target)) {
    ctx->found = candidate;
  }
}

Tuple *relation_find_tuple(Relation *r, Tuple *target) {
  FindTupleContext ctx = {.target = target, .found = NULL};
  set_foreach(r->tuples, check_tuple_cb, &ctx);
  return ctx.found;
}
