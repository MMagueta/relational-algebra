/**
 * @file cardinality.c
 * @brief Implementation of cardinality operations.
 */
#include <stdio.h>

#include "cardinality.h"
Cardinality cardinality_finite(uint64_t count) {
  Cardinality c;
  c.type = CARD_FINITE;
  c.finite_count = count;
  return c;
}

Cardinality cardinality_infinite(CardinalityType type) {
  Cardinality c;
  c.type = type;
  c.finite_count = 0;
  return c;
}

int cardinality_is_finite(Cardinality c) { return c.type == CARD_FINITE; }

int cardinality_is_infinite(Cardinality c) {
  return c.type != CARD_FINITE && c.type != CARD_UNKNOWN;
}

void cardinality_print(Cardinality c) {
  switch (c.type) {
  case CARD_FINITE:
    printf("|R| = %lu", (unsigned long)c.finite_count);
    break;
  case CARD_ALEPH_0:
    printf("|R| = ℵ₀");
    break;
  case CARD_ALEPH_1:
    printf("|R| = ℵ₁");
    break;
  case CARD_CONTINUUM:
    printf("|R| = 2^ℵ₀");
    break;
  case CARD_UNKNOWN:
    printf("|R| = ?");
    break;
  }
}

Cardinality cardinality_product(Cardinality a, Cardinality b) {
  // If either is unknown, result is unknown
  if (a.type == CARD_UNKNOWN || b.type == CARD_UNKNOWN) {
    return cardinality_infinite(CARD_UNKNOWN);
  }

  // If both finite, multiply
  if (a.type == CARD_FINITE && b.type == CARD_FINITE) {
    return cardinality_finite(a.finite_count * b.finite_count);
  }

  // If one finite (non-zero) and one infinite, result is infinite
  if (a.type == CARD_FINITE && b.type != CARD_FINITE) {
    if (a.finite_count == 0)
      return cardinality_finite(0);
    return cardinality_infinite(b.type);
  }
  if (b.type == CARD_FINITE && a.type != CARD_FINITE) {
    if (b.finite_count == 0)
      return cardinality_finite(0);
    return cardinality_infinite(a.type);
  }

  // Both infinite: take the maximum
  CardinalityType max_type = a.type > b.type ? a.type : b.type;
  return cardinality_infinite(max_type);
}
