/**
 * @file arithmetic_relations.c
 * @brief Implementation of arithmetic primitive relations.
 */

#include "arithmetic_relations.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Cantor pairing for natural numbers
static void cantor_unpair_nat(size_t n, size_t *k1, size_t *k2) {
  size_t w = (size_t)floor((sqrt(8.0 * n + 1.0) - 1.0) / 2.0);
  size_t t = (w * w + w) / 2;
  *k2 = n - t;
  *k1 = w - *k2;
}

// Map natural number to integer using zigzag encoding
// 0 -> 0, 1 -> 1, 2 -> -1, 3 -> 2, 4 -> -2, ...
static int64_t nat_to_integer(size_t n) {
  if (n == 0)
    return 0;
  if (n % 2 == 1)
    return (int64_t)(n / 2) + 1;
  return -(int64_t)(n / 2);
}

void cantor_to_integer_pair(size_t n, int64_t *x, int64_t *y) {
  size_t nx, ny;
  cantor_unpair_nat(n, &nx, &ny);
  *x = nat_to_integer(nx);
  *y = nat_to_integer(ny);
}

Tuple *addition_generator(size_t n, void *userdata) {
  (void)userdata;

  int64_t x, y;
  cantor_to_integer_pair(n, &x, &y);

  Tuple *t = tuple_create();

  int64_t *op1 = malloc(sizeof(int64_t));
  int64_t *op2 = malloc(sizeof(int64_t));
  int64_t *result = malloc(sizeof(int64_t));

  *op1 = x;
  *op2 = y;
  *result = x + y;

  tuple_add_attribute(t, attribute_create("operand1", ATTR_INT, op1));
  tuple_add_attribute(t, attribute_create("operand2", ATTR_INT, op2));
  tuple_add_attribute(t, attribute_create("result", ATTR_INT, result));

  return t;
}

Tuple *subtraction_generator(size_t n, void *userdata) {
  (void)userdata;

  int64_t x, y;
  cantor_to_integer_pair(n, &x, &y);

  Tuple *t = tuple_create();

  int64_t *op1 = malloc(sizeof(int64_t));
  int64_t *op2 = malloc(sizeof(int64_t));
  int64_t *result = malloc(sizeof(int64_t));

  *op1 = x;
  *op2 = y;
  *result = x - y;

  tuple_add_attribute(t, attribute_create("operand1", ATTR_INT, op1));
  tuple_add_attribute(t, attribute_create("operand2", ATTR_INT, op2));
  tuple_add_attribute(t, attribute_create("result", ATTR_INT, result));

  return t;
}

Tuple *multiplication_generator(size_t n, void *userdata) {
  (void)userdata;

  int64_t x, y;
  cantor_to_integer_pair(n, &x, &y);

  Tuple *t = tuple_create();

  int64_t *op1 = malloc(sizeof(int64_t));
  int64_t *op2 = malloc(sizeof(int64_t));
  int64_t *result = malloc(sizeof(int64_t));

  *op1 = x;
  *op2 = y;
  *result = x * y;

  tuple_add_attribute(t, attribute_create("operand1", ATTR_INT, op1));
  tuple_add_attribute(t, attribute_create("operand2", ATTR_INT, op2));
  tuple_add_attribute(t, attribute_create("result", ATTR_INT, result));

  return t;
}

Tuple *integer_division_generator(size_t n, void *userdata) {
  (void)userdata;

  // We need to skip cases where divisor = 0
  // Keep incrementing until we find a valid pair
  size_t attempt = n;
  const size_t MAX_ATTEMPTS = 1000000;

  for (size_t i = 0; i < MAX_ATTEMPTS; i++) {
    int64_t x, y;
    cantor_to_integer_pair(attempt, &x, &y);

    if (y != 0) {
      Tuple *t = tuple_create();

      int64_t *dividend = malloc(sizeof(int64_t));
      int64_t *divisor = malloc(sizeof(int64_t));
      int64_t *quotient = malloc(sizeof(int64_t));

      *dividend = x;
      *divisor = y;
      *quotient = (int64_t)x / (int64_t)y;

      tuple_add_attribute(t, attribute_create("dividend", ATTR_INT, dividend));
      tuple_add_attribute(t, attribute_create("divisor", ATTR_INT, divisor));
      tuple_add_attribute(t, attribute_create("quotient", ATTR_INT, quotient));

      return t;
    }

    attempt++;
  }

  return NULL; // Should rarely happen
}

InfiniteRelation *create_addition_relation(void) {
  return infinite_relation_create_with_cardinality("ADD", addition_generator, NULL,
                                                   cardinality_infinite(CARD_ALEPH_0));
}

InfiniteRelation *create_subtraction_relation(void) {
  return infinite_relation_create_with_cardinality("SUB", subtraction_generator, NULL,
                                                   cardinality_infinite(CARD_ALEPH_0));
}

InfiniteRelation *create_multiplication_relation(void) {
  return infinite_relation_create_with_cardinality("MUL", multiplication_generator, NULL,
                                                   cardinality_infinite(CARD_ALEPH_0));
}

InfiniteRelation *create_division_relation(void) {
  return infinite_relation_create_with_cardinality("DIV", integer_division_generator, NULL,
                                                   cardinality_infinite(CARD_ALEPH_0));
}
