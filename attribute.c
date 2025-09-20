#include "attribute.h"
#include "set.h"    /* for ATTR_SET printing */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Attribute *attribute_create(const char *name, AttributeType type, void *value) {
    Attribute *attr = malloc(sizeof(Attribute));
    if (!attr) return NULL;
    attr->name = strdup(name);
    attr->type = type;
    attr->value = value;
    return attr;
}

void attribute_destroy(Attribute *attr) {
    if (!attr) return;
    free(attr->name);
    /* TODO: not freeing attr->value here — that’s user’s job, 
       because value might belong to a Set or something else */
    free(attr);
}

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
