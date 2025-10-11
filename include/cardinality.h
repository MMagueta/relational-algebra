#ifndef CARDINALITY_H
#define CARDINALITY_H

#include <stdint.h>

/**
 * @file cardinality.h
 * @brief Cardinality representation for relations.
 *
 * Supports finite cardinalities and various infinite cardinals (Aleph0, etc.)
 */

typedef enum {
  CARD_FINITE,    /** Finite cardinality */
  CARD_ALEPH_0,   /** Countably infinite (ℵ₀) */
  CARD_ALEPH_1,   /** First uncountable cardinal (ℵ₁) */
  CARD_CONTINUUM, /** Cardinality of the continuum (2^ℵ₀) */
  CARD_UNKNOWN    /** Unknown cardinality */
} CardinalityType;

typedef struct {
  CardinalityType type;
  uint64_t finite_count; /** Only used when type = CARD_FINITE */
} Cardinality;

/**
 * @brief Create a finite cardinality.
 */
Cardinality cardinality_finite(uint64_t count);

/**
 * @brief Create an infinite cardinality of the given type.
 */
Cardinality cardinality_infinite(CardinalityType type);

/**
 * @brief Check if a cardinality is finite.
 */
int cardinality_is_finite(Cardinality c);

/**
 * @brief Check if a cardinality is infinite.
 */
int cardinality_is_infinite(Cardinality c);

/**
 * @brief Print a cardinality to stdout.
 */
void cardinality_print(Cardinality c);

/**
 * @brief Compute the cardinality of a Cartesian product.
 *
 * For finite × finite: multiply counts
 * For finite × infinite or infinite × finite: infinite (same type)
 * For infinite × infinite: infinite (max of the two types)
 */
Cardinality cardinality_product(Cardinality a, Cardinality b);

#endif // CARDINALITY_H
