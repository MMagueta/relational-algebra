#ifndef JOIN_H
#define JOIN_H

#include "cardinality.h"
#include "infinite_relation.h"
#include "relation.h"
#include "tuple.h"

/**
 * @file join.h
 * @brief Join operations for finite and infinite relations.
 */

/**
 * Join predicate function type.
 * Takes two tuples and returns 1 if they should be joined, 0 otherwise.
 */
typedef int (*JoinPredicateFn)(Tuple *left, Tuple *right, void *userdata);

/**
 * @brief Perform a nested loop join on two finite relations.
 *
 * @param left First relation
 * @param right Second relation
 * @param predicate Join condition
 * @param userdata Optional data passed to predicate
 * @param result_name Name for the result relation
 * @return New relation containing joined tuples
 */
Relation *relation_join(Relation *left, Relation *right, JoinPredicateFn predicate, void *userdata,
                        const char *result_name);

/**
 * @brief Join context for infinite relations.
 *
 * Since infinite joins can produce infinite results, we represent them
 * as infinite relations with a generator that performs the join on-demand.
 */
typedef struct {
  InfiniteRelation *left;
  InfiniteRelation *right;
  JoinPredicateFn predicate;
  void *userdata;
  Cardinality result_cardinality;
} InfiniteJoinContext;

/**
 * @brief Perform a nested loop join on two infinite relations.
 *
 * The result is an infinite relation that generates tuples on-demand.
 * Uses Cantor pairing to enumerate all combinations.
 *
 * @param left First infinite relation
 * @param right Second infinite relation
 * @param predicate Join condition
 * @param userdata Optional data passed to predicate
 * @param result_name Name for the result relation
 * @param result_cardinality Expected cardinality of result
 * @return New infinite relation representing the join
 */
InfiniteRelation *infinite_relation_join(InfiniteRelation *left, InfiniteRelation *right,
                                         JoinPredicateFn predicate, void *userdata,
                                         const char *result_name, Cardinality result_cardinality);

/**
 * @brief Merge two tuples into one (for join results).
 *
 * Creates a new tuple containing all attributes from both input tuples.
 * If there are duplicate attribute names, uses the left tuple's value.
 *
 * @param left First tuple
 * @param right Second tuple
 * @return New merged tuple
 */
Tuple *tuple_merge(Tuple *left, Tuple *right);

#endif // JOIN_H
