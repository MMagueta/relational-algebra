#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <stddef.h>

typedef enum { ATTR_INT, ATTR_RATIONAL, ATTR_STRING, ATTR_SET, ATTR_UNKNOWN } AttributeType;

typedef struct {
  char *name;
  AttributeType type;
  void *value;
} Attribute;

Attribute *attribute_create(const char *name, AttributeType type, void *value);
void attribute_destroy(Attribute *attr);
void attribute_print(const Attribute *attr);
#endif // ATTRIBUTE_H
