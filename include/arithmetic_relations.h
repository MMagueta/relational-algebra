/**
 * @file arithmetic_relations.h
 * @brief Primitive relations for arithmetic operations.
 * 
 * Defines arithmetic operations as infinite relations that can be used
 * in joins and other relational operations.
 * 
 * Each operation is modeled as a relation over the domain:
 * - Addition:       ADD = {(x, y, z) | x + y = z, x,y,z ∈ ℤ}
 * - Subtraction:    SUB = {(x, y, z) | x - y = z, x,y,z ∈ ℤ}
 * - Multiplication: MUL = {(x, y, z) | x * y = z, x,y,z ∈ ℤ}
 * - Division:       DIV = {(x, y, z) | x / y = z, x,y ∈ ℤ, y ≠ 0, z ∈ ℚ}
 */

#ifndef ARITHMETIC_RELATIONS_H
#define ARITHMETIC_RELATIONS_H

#include "infinite_relation.h"
#include "tuple.h"
#include <stdint.h>

/**
 * Generator for addition relation: ADD = {(x, y, x+y) | x,y ∈ ℤ}
 * Uses Cantor pairing to enumerate all (x,y) pairs.
 * 
 * Tuple schema: (operand1: INT, operand2: INT, result: INT)
 */
Tuple *addition_generator(size_t n, void *userdata);

/**
 * Generator for subtraction relation: SUB = {(x, y, x-y) | x,y ∈ ℤ}
 * Uses Cantor pairing to enumerate all (x,y) pairs.
 * 
 * Tuple schema: (operand1: INT, operand2: INT, result: INT)
 */
Tuple *subtraction_generator(size_t n, void *userdata);

/**
 * Generator for multiplication relation: MUL = {(x, y, x*y) | x,y ∈ ℤ}
 * Uses Cantor pairing to enumerate all (x,y) pairs.
 * 
 * Tuple schema: (operand1: INT, operand2: INT, result: INT)
 */
Tuple *multiplication_generator(size_t n, void *userdata);

/**
 * Generator for division relation: DIV = {(x, y, x/y) | x,y ∈ ℤ, y ≠ 0}
 * Uses Cantor pairing to enumerate all (x,y) pairs, skipping y=0.
 * 
 * Tuple schema: (dividend: INT, divisor: INT, quotient: DOUBLE)
 */
Tuple *division_generator(size_t n, void *userdata);

/**
 * Helper: Convert size_t to a pair of integers using Cantor pairing.
 * This allows enumeration of ℤ × ℤ from ℕ.
 */
void cantor_to_integer_pair(size_t n, int64_t *x, int64_t *y);

/**
 * Create standard arithmetic relations as infinite relations.
 * These can be used directly in joins with other relations.
 */
InfiniteRelation *create_addition_relation(void);
InfiniteRelation *create_subtraction_relation(void);
InfiniteRelation *create_multiplication_relation(void);
InfiniteRelation *create_division_relation(void);

#endif // ARITHMETIC_RELATIONS_H
