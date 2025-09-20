/**
 * @file attribute.c
 * @brief Attribute implementation for the Relational Algebra Engine.
 *
 * Handles creation, destruction, and printing of attributes, which are name-value pairs
 * used as the basic building blocks of tuples in the relational model.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "attribute.h"
#include "set.h" /* for ATTR_SET printing */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "attribute.h"
#include "set.h" /* for ATTR_SET printing */

/**
 * @brief Create a new Attribute.
 *
 * @param name Name of the attribute (copied).
 * @param type Type of the attribute (see AttributeType).
 * @param value Pointer to the value (ownership is taken).
 * @return Pointer to the new Attribute, or NULL on failure.
 */
Attribute *attribute_create(const char *name, AttributeType type, void *value) {
  Attribute *attr = malloc(sizeof(Attribute));
  if (!attr)
    return NULL;
  attr->name = strdup(name);
  attr->type = type;
  attr->value = value;
  return attr;
}

/**
 * @brief Destroy an Attribute and free its memory.
 *
 * @param attr Pointer to the Attribute to destroy. Safe to pass NULL.
 */
void attribute_destroy(Attribute *attr) {
  if (!attr)
    return;
  free(attr->name); // always free the name string
  if (!attr->value) {
    free(attr);
    return;
  }
  switch (attr->type) {
  case ATTR_INT:
  case ATTR_DOUBLE:
    free(attr->value); // value points to malloced int or double
    break;
  case ATTR_STRING:
    free(attr->value); // value points to malloc'd string
    break;
  case ATTR_SET:
    set_destroy((Set *)attr->value); // value points to a Set*
    break;
  case ATTR_UNKNOWN:
  default:
    fprintf(stderr, "[PANIC] Cannot deallocate unknown type safely.");
    exit(EXIT_FAILURE);
    break;
  }
  free(attr);
}

/**
 * @brief Print an Attribute to stdout.
 *
 * @param attr Pointer to the Attribute to print.
 */
void attribute_print(const Attribute *attr) {
  printf("%s = ", attr->name);
  switch (attr->type) {
  case ATTR_INT:
    printf("%d", *(int *)attr->value);
    break;
  case ATTR_DOUBLE:
    printf("%f", *(double *)attr->value);
    break;
  case ATTR_STRING:
    printf("\"%s\"", (char *)attr->value);
    break;
  case ATTR_SET:
    printf("{set of %zu elements}", set_size((Set *)attr->value));
    break;
  default:
    printf("<unknown>");
  }
  printf("\n");
}
