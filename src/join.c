/**
 * @file join.c
 * @brief Implementation of join operations.
 */
#include "join.h"
#include "attribute.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Context for collecting join results during set iteration.
 */
typedef struct {
  Relation *result;
  Relation *right;
  JoinPredicateFn predicate;
  void *userdata;
} JoinIterContext;

/**
 * Helper to copy an attribute with its value.
 */
static Attribute *attribute_copy(const Attribute *attr) {
  void *value_copy = NULL;

  switch (attr->type) {
    case ATTR_INT: {
      int *v = malloc(sizeof(int));
      *v = *(int *)attr->value;
      value_copy = v;
      break;
    }
    case ATTR_RATIONAL: {
      int *v = malloc(sizeof(double));
      *v = *(int *)attr->value;
      value_copy = v;
      break;
    }
    case ATTR_STRING:
      value_copy = strdup((char *)attr->value);
      break;
    case ATTR_SET:
      // For now, we don't deep copy sets - would need recursive logic
      fprintf(stderr, "Warning: shallow copy of SET attribute\n");
      value_copy = attr->value;
      break;
    default:
      break;
  }

  return attribute_create(attr->name, attr->type, value_copy);
}

/**
 * Context for merging tuple attributes.
 */
typedef struct {
  Tuple *target;
  const char *prefix; // Prefix to add to attribute names (for disambiguation)
} MergeContext;

/**
 * Callback to copy attributes from source to target tuple.
 */
static void copy_attribute_cb(void *element, void *userdata) {
  Attribute *attr = (Attribute *)element;
  MergeContext *ctx = (MergeContext *)userdata;
  
  // Create new attribute name with prefix if needed
  char *new_name = NULL;
  if (ctx->prefix) {
    size_t len = strlen(ctx->prefix) + strlen(attr->name) + 2; // prefix + attr->name + '_' + '\0'
    new_name = malloc(len);
    snprintf(new_name, len, "%s_%s", ctx->prefix, attr->name);
  } else {
    new_name = strdup(attr->name);
  }
  
  // Check if attribute already exists (avoid duplicates)
  if (!tuple_find_attribute(ctx->target, new_name)) {
    Attribute *copy = attribute_copy(attr);
    free(copy->name); // Free the old name
    copy->name = new_name; // Use the new name
    tuple_add_attribute(ctx->target, copy);
  } else {
    free(new_name);
  }
}

Tuple *tuple_merge(Tuple *left, Tuple *right) {
  Tuple *merged = tuple_create();
  
  // Copy all attributes from left tuple with "left" prefix
  MergeContext left_ctx = {.target = merged, .prefix = "left"};
  set_foreach(left, copy_attribute_cb, &left_ctx);
  
  // Copy attributes from right tuple with "right" prefix
  MergeContext right_ctx = {.target = merged, .prefix = "right"};
  set_foreach(right, copy_attribute_cb, &right_ctx);
  
  return merged;
}

/**
 * Context for inner loop of nested loop join.
 */
typedef struct {
  Tuple *left;
  Relation *result;
  JoinPredicateFn predicate;
  void *userdata;
} InnerContext;

/**
 * Callback for inner loop of nested loop join.
 */
static void join_inner_cb(void *right_element, void *inner_userdata) {
  Tuple *right_tuple = (Tuple *)right_element;
  InnerContext *ictx = (InnerContext *)inner_userdata;
  
  // Test the join predicate
  if (ictx->predicate(ictx->left, right_tuple, ictx->userdata)) {
    // Merge tuples and add to result
    Tuple *merged = tuple_merge(ictx->left, right_tuple);
    relation_add_tuple(ictx->result, merged);
  }
}

/**
 * Callback for outer loop of nested loop join.
 */
static void join_outer_cb(void *element, void *userdata) {
  Tuple *left_tuple = (Tuple *)element;
  JoinIterContext *ctx = (JoinIterContext *)userdata;
  
  // Inner loop: iterate through right relation
  InnerContext inner_ctx = {
    .left = left_tuple,
    .result = ctx->result,
    .predicate = ctx->predicate,
    .userdata = ctx->userdata
  };
  
  set_foreach(ctx->right->tuples, join_inner_cb, &inner_ctx);
}

Relation *relation_join(Relation *left, Relation *right,
                        JoinPredicateFn predicate, void *userdata,
                        const char *result_name) {
  Relation *result = relation_create(result_name);
  
  JoinIterContext ctx = {
    .result = result,
    .right = right,
    .predicate = predicate,
    .userdata = userdata
  };
  
  // Nested loop join: for each tuple in left, check all tuples in right
  set_foreach(left->tuples, join_outer_cb, &ctx);
  
  return result;
}

/**
 * Cantor pairing function: maps (k1, k2) -> N bijectively.
 * Used to enumerate all pairs from two countably infinite sets.
 */
static size_t cantor_pair(size_t k1, size_t k2) {
  return (k1 + k2) * (k1 + k2 + 1) / 2 + k2;
}

/**
 * Inverse Cantor pairing: given n, find (k1, k2) such that cantor_pair(k1,k2) = n.
 */
static void cantor_unpair(size_t n, size_t *k1, size_t *k2) {
  size_t w = (size_t)floor((sqrt(8.0 * n + 1.0) - 1.0) / 2.0);
  size_t t = (w * w + w) / 2;
  *k2 = n - t;
  *k1 = w - *k2;
}

/**
 * Generator function for infinite join.
 * 
 * We enumerate pairs (i,j) using Cantor pairing and count only those
 * that satisfy the predicate. The nth result is the nth matching pair.
 */
static Tuple *infinite_join_generator(size_t n, void *userdata) {
  InfiniteJoinContext *ctx = (InfiniteJoinContext *)userdata;
  
  // We need to find the nth pair that satisfies the predicate
  size_t found_count = 0;
  size_t attempt = 0;
  const size_t max_attempts = 100000000; // Safety limit
  
  while (found_count <= n && attempt < max_attempts) {
    size_t i, j;
    cantor_unpair(attempt, &i, &j);
    
    // Generate tuples from both relations
    Tuple *left_tuple = infinite_relation_tuple_at(ctx->left, i);
    Tuple *right_tuple = infinite_relation_tuple_at(ctx->right, j);
    
    if (!left_tuple || !right_tuple) {
      if (left_tuple) tuple_destroy(left_tuple);
      if (right_tuple) tuple_destroy(right_tuple);
      attempt++;
      continue;
    }
    
    // Check predicate
    int matches = ctx->predicate(left_tuple, right_tuple, ctx->userdata);
    
    if (matches) {
      if (found_count == n) {
        // This is the nth match - return it
        Tuple *merged = tuple_merge(left_tuple, right_tuple);
        tuple_destroy(left_tuple);
        tuple_destroy(right_tuple);
        return merged;
      }
      found_count++;
    }
    
    tuple_destroy(left_tuple);
    tuple_destroy(right_tuple);
    attempt++;
  }
  
  return NULL; // No match found in reasonable attempts
}

InfiniteRelation *infinite_relation_join(InfiniteRelation *left,
                                         InfiniteRelation *right,
                                         JoinPredicateFn predicate,
                                         void *userdata,
                                         const char *result_name,
                                         Cardinality result_cardinality) {
  InfiniteJoinContext *ctx = malloc(sizeof(InfiniteJoinContext));
  ctx->left = left;
  ctx->right = right;
  ctx->predicate = predicate;
  ctx->userdata = userdata;
  ctx->result_cardinality = result_cardinality;
  
  return infinite_relation_create(result_name, infinite_join_generator, ctx);
}
