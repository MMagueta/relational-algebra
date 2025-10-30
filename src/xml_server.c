/**
 * @file xml_server.c
 * @brief XML socket server for the Relational Algebra Engine FFI.
 *
 * Provides a network interface to create and manipulate relations through XML commands.
 * The server maintains a schema (set of relations) in memory and processes commands
 * to create relations, add tuples, query, and perform other operations.
 */

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "attribute.h"
#include "relation.h"
#include "set.h"
#include "tuple.h"
#include "xml_server.h"

#define MAX_BUFFER 8192
#define MAX_RESPONSE 16384

// Schema: a set of relations
typedef struct {
  Set *relations; // Set of Relation*
} Schema;

// Compare relations by name
static int relation_cmp(const void *a, const void *b) {
  const Relation *r1 = (const Relation *)a;
  const Relation *r2 = (const Relation *)b;
  return strcmp(r1->name, r2->name);
}

// Don't free relations in schema set (we manage them separately)
static void relation_no_free(void *r) { (void)r; }

// Create a new schema
Schema *schema_create(void) {
  Schema *s = malloc(sizeof(Schema));
  if (!s)
    return NULL;
  s->relations = set_create(relation_cmp, relation_no_free);
  return s;
}

// Context for finding relation by name
typedef struct {
  const char *name;
  Relation *found;
} FindRelationContext;

static void find_relation_cb(void *element, void *userdata) {
  Relation *r = (Relation *)element;
  FindRelationContext *ctx = (FindRelationContext *)userdata;
  if (strcmp(r->name, ctx->name) == 0) {
    ctx->found = r;
  }
}

// Find a relation in schema by name
Relation *schema_find_relation(Schema *s, const char *name) {
  FindRelationContext ctx = {.name = name, .found = NULL};
  set_foreach(s->relations, find_relation_cb, &ctx);
  return ctx.found;
}

// Add a relation to schema
int schema_add_relation(Schema *s, Relation *r) { return set_add(s->relations, r); }

// Context for destroying all relations
static void destroy_relation_cb(void *element, void *userdata) {
  (void)userdata;
  relation_destroy((Relation *)element);
}

// Destroy schema and all relations
void schema_destroy(Schema *s) {
  if (!s)
    return;
  set_foreach(s->relations, destroy_relation_cb, NULL);
  set_destroy(s->relations);
  free(s);
}

// Simple XML parser helpers
static char *xml_find_tag(const char *xml, const char *tag, char *out, size_t out_size) {
  char start_tag[64], end_tag[64];
  snprintf(start_tag, sizeof(start_tag), "<%s>", tag);
  snprintf(end_tag, sizeof(end_tag), "</%s>", tag);

  const char *start = strstr(xml, start_tag);
  if (!start)
    return NULL;
  start += strlen(start_tag);

  const char *end = strstr(start, end_tag);
  if (!end)
    return NULL;

  size_t len = end - start;
  if (len >= out_size)
    len = out_size - 1;

  strncpy(out, start, len);
  out[len] = '\0';
  return out;
}

// Parse attribute type from string
static AttributeType parse_attr_type(const char *type_str) {
  if (strcmp(type_str, "int") == 0)
    return ATTR_INT;
  if (strcmp(type_str, "string") == 0)
    return ATTR_STRING;
  if (strcmp(type_str, "rational") == 0)
    return ATTR_RATIONAL;
  return ATTR_UNKNOWN;
}

// Build XML response
static void build_response(char *response, size_t max_size, const char *status, const char *message,
                           const char *data) {
  snprintf(response, max_size,
           "<?xml version=\"1.0\"?>\n"
           "<response>\n"
           "  <status>%s</status>\n"
           "  <message>%s</message>\n"
           "%s"
           "</response>\n",
           status, message, data ? data : "");
}

// Handle CREATE_RELATION command
static void handle_create_relation(Schema *schema, const char *xml, char *response,
                                   size_t response_size) {
  char name[256];

  if (!xml_find_tag(xml, "name", name, sizeof(name))) {
    build_response(response, response_size, "error", "Missing relation name", NULL);
    return;
  }

  // Check if relation already exists
  if (schema_find_relation(schema, name)) {
    build_response(response, response_size, "error", "Relation already exists", NULL);
    return;
  }

  Relation *r = relation_create(name);
  if (!r) {
    build_response(response, response_size, "error", "Failed to create relation", NULL);
    return;
  }

  schema_add_relation(schema, r);
  build_response(response, response_size, "success", "Relation created", NULL);
}

// Handle ADD_TUPLE command
static void handle_add_tuple(Schema *schema, const char *xml, char *response,
                             size_t response_size) {
  char relation_name[256];
  if (!xml_find_tag(xml, "relation", relation_name, sizeof(relation_name))) {
    build_response(response, response_size, "error", "Missing relation name", NULL);
    return;
  }

  Relation *r = schema_find_relation(schema, relation_name);
  if (!r) {
    build_response(response, response_size, "error", "Relation not found", NULL);
    return;
  }

  Tuple *t = tuple_create();

  // Parse attributes
  const char *attr_start = strstr(xml, "<attributes>");
  if (!attr_start) {
    tuple_destroy(t);
    build_response(response, response_size, "error", "Missing attributes", NULL);
    return;
  }

  const char *attr_end = strstr(attr_start, "</attributes>");
  if (!attr_end) {
    tuple_destroy(t);
    build_response(response, response_size, "error", "Malformed attributes", NULL);
    return;
  }

  // Parse each <attribute> tag
  const char *current = attr_start;
  while ((current = strstr(current, "<attribute>")) && current < attr_end) {
    current += strlen("<attribute>");

    char attr_name[128], attr_type[32], attr_value[256];
    const char *attr_tag_end = strstr(current, "</attribute>");
    if (!attr_tag_end)
      break;

    size_t attr_len = attr_tag_end - current;
    char attr_xml[1024];
    if (attr_len >= sizeof(attr_xml))
      attr_len = sizeof(attr_xml) - 1;
    strncpy(attr_xml, current, attr_len);
    attr_xml[attr_len] = '\0';

    if (!xml_find_tag(attr_xml, "name", attr_name, sizeof(attr_name)) ||
        !xml_find_tag(attr_xml, "type", attr_type, sizeof(attr_type)) ||
        !xml_find_tag(attr_xml, "value", attr_value, sizeof(attr_value))) {
      tuple_destroy(t);
      build_response(response, response_size, "error", "Malformed attribute", NULL);
      return;
    }

    AttributeType type = parse_attr_type(attr_type);
    void *value = NULL;

    switch (type) {
    case ATTR_INT: {
      int *v = malloc(sizeof(int));
      *v = atoi(attr_value);
      value = v;
      break;
    }
    case ATTR_STRING:
      value = strdup(attr_value);
      break;
    case ATTR_RATIONAL: {
      double *v = malloc(sizeof(double));
      *v = atof(attr_value);
      value = v;
      break;
    }
    default:
      tuple_destroy(t);
      build_response(response, response_size, "error", "Unsupported attribute type", NULL);
      return;
    }

    Attribute *attr = attribute_create(attr_name, type, value);
    tuple_add_attribute(t, attr);

    current = attr_tag_end + strlen("</attribute>");
  }

  int result = relation_add_tuple(r, t);
  if (result == 1) {
    build_response(response, response_size, "success", "Tuple added", NULL);
  } else if (result == 0) {
    tuple_destroy(t);
    build_response(response, response_size, "success", "Tuple already exists", NULL);
  } else {
    tuple_destroy(t);
    build_response(response, response_size, "error", "Failed to add tuple", NULL);
  }
}

// Context for building XML from tuples
typedef struct {
  char *buffer;
  size_t size;
  size_t offset;
} XmlBuildContext;

static void append_to_xml(XmlBuildContext *ctx, const char *str) {
  size_t len = strlen(str);
  if (ctx->offset + len < ctx->size) {
    strcpy(ctx->buffer + ctx->offset, str);
    ctx->offset += len;
  }
}

// Context for converting attributes to XML
typedef struct {
  XmlBuildContext *xml_ctx;
} AttrToXmlContext;

// Callback to convert attribute to XML
static void attr_to_xml_cb(void *attr_element, void *attr_userdata) {
  Attribute *attr = (Attribute *)attr_element;
  AttrToXmlContext *actx = (AttrToXmlContext *)attr_userdata;
  char attr_buf[512];

  const char *type_str = "?";
  char value_str[256] = "";

  switch (attr->type) {
  case ATTR_INT:
    type_str = "int";
    snprintf(value_str, sizeof(value_str), "%d", *(int *)attr->value);
    break;
  case ATTR_STRING:
    type_str = "string";
    snprintf(value_str, sizeof(value_str), "%s", (char *)attr->value);
    break;
  case ATTR_RATIONAL:
    type_str = "rational";
    snprintf(value_str, sizeof(value_str), "%f", *(double *)attr->value);
    break;
  default:
    break;
  }

  snprintf(attr_buf, sizeof(attr_buf),
           "      <attribute>\n"
           "        <name>%s</name>\n"
           "        <type>%s</type>\n"
           "        <value>%s</value>\n"
           "      </attribute>\n",
           attr->name, type_str, value_str);
  append_to_xml(actx->xml_ctx, attr_buf);
}

// Callback to convert tuple to XML
static void tuple_to_xml_cb(void *element, void *userdata) {
  XmlBuildContext *ctx = (XmlBuildContext *)userdata;
  append_to_xml(ctx, "      <tuple>\n");

  Tuple *t = (Tuple *)element;
  AttrToXmlContext actx = {.xml_ctx = ctx};
  set_foreach(t, attr_to_xml_cb, &actx);

  append_to_xml(ctx, "      </tuple>\n");
}

// Handle QUERY_RELATION command
static void handle_query_relation(Schema *schema, const char *xml, char *response,
                                  size_t response_size) {
  char relation_name[256];
  if (!xml_find_tag(xml, "relation", relation_name, sizeof(relation_name))) {
    build_response(response, response_size, "error", "Missing relation name", NULL);
    return;
  }

  Relation *r = schema_find_relation(schema, relation_name);
  if (!r) {
    build_response(response, response_size, "error", "Relation not found", NULL);
    return;
  }

  char data[MAX_RESPONSE];
  XmlBuildContext ctx = {.buffer = data, .size = sizeof(data), .offset = 0};

  append_to_xml(&ctx, "  <data>\n");
  append_to_xml(&ctx, "    <relation>\n");
  char name_buf[512];
  snprintf(name_buf, sizeof(name_buf), "      <name>%s</name>\n", r->name);
  append_to_xml(&ctx, name_buf);

  char card_buf[256];
  snprintf(card_buf, sizeof(card_buf), "      <cardinality>%zu</cardinality>\n",
           set_size(r->tuples));
  append_to_xml(&ctx, card_buf);

  append_to_xml(&ctx, "      <tuples>\n");
  set_foreach(r->tuples, tuple_to_xml_cb, &ctx);
  append_to_xml(&ctx, "      </tuples>\n");
  append_to_xml(&ctx, "    </relation>\n");
  append_to_xml(&ctx, "  </data>\n");

  build_response(response, response_size, "success", "Query executed", data);
}

// Callback for listing relations
static void list_rel_cb(void *element, void *userdata) {
  Relation *r = (Relation *)element;
  XmlBuildContext *lctx = (XmlBuildContext *)userdata;
  char buf[512];
  snprintf(buf, sizeof(buf), "      <relation name=\"%s\" size=\"%zu\"/>\n", r->name,
           set_size(r->tuples));
  append_to_xml(lctx, buf);
}

// Handle LIST_RELATIONS command
static void handle_list_relations(Schema *schema, const char *xml, char *response,
                                  size_t response_size) {
  (void)xml; // unused

  char data[MAX_RESPONSE];
  XmlBuildContext ctx = {.buffer = data, .size = sizeof(data), .offset = 0};

  append_to_xml(&ctx, "  <data>\n");
  append_to_xml(&ctx, "    <relations>\n");

  set_foreach(schema->relations, list_rel_cb, &ctx);

  append_to_xml(&ctx, "    </relations>\n");
  append_to_xml(&ctx, "  </data>\n");

  build_response(response, response_size, "success", "Relations listed", data);
}

// Process incoming XML command
static void process_command(Schema *schema, const char *xml, char *response,
                            size_t response_size) {
  char command[64];
  if (!xml_find_tag(xml, "command", command, sizeof(command))) {
    build_response(response, response_size, "error", "Missing command", NULL);
    return;
  }
  
  if (strcmp(command, "CREATE_RELATION") == 0) {
    handle_create_relation(schema, xml, response, response_size);
  } else if (strcmp(command, "ADD_TUPLE") == 0) {
    handle_add_tuple(schema, xml, response, response_size);
  } else if (strcmp(command, "QUERY_RELATION") == 0) {
    handle_query_relation(schema, xml, response, response_size);
  } else if (strcmp(command, "LIST_RELATIONS") == 0) {
    handle_list_relations(schema, xml, response, response_size);
  } else {
    build_response(response, response_size, "error", "Cannot discern command", NULL);
  }
}

// Handle client connection
static void handle_client(int client_fd, Schema *schema) {
  char buffer[MAX_BUFFER];
  char response[MAX_RESPONSE];

  while (1) {
    memset(buffer, 0, sizeof(buffer));
    ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

    if (bytes_read <= 0) {
      break; // Connection closed or error
    }

    printf("Received command:\n%s\n", buffer);

    memset(response, 0, sizeof(response));
    process_command(schema, buffer, response, sizeof(response));

    send(client_fd, response, strlen(response), 0);
    printf("Sent response:\n%s\n", response);
  }

  close(client_fd);
}

// Main server function
int start_xml_server(int port) {
  int server_fd, client_fd;
  struct sockaddr_in address;
  int opt = 1;
  socklen_t addrlen = sizeof(address);

  Schema *schema = schema_create();
  if (!schema) {
    fprintf(stderr, "Failed to create schema\n");
    return 1;
  }

  // Create socket
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket failed");
    schema_destroy(schema);
    return 1;
  }

  // Set socket options
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt))) {
    perror("setsockopt");
    close(server_fd);
    schema_destroy(schema);
    return 1;
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  // Bind socket
  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind failed");
    close(server_fd);
    schema_destroy(schema);
    return 1;
  }

  // Listen
  if (listen(server_fd, 3) < 0) {
    perror("listen");
    close(server_fd);
    schema_destroy(schema);
    return 1;
  }

  printf("XML Socket Server listening on port %d...\n", port);
  printf("\nSupported commands:\n");
  printf("  - CREATE_RELATION: Create a new relation\n");
  printf("  - ADD_TUPLE: Add a tuple to a relation\n");
  printf("  - QUERY_RELATION: Query all tuples in a relation\n");
  printf("  - LIST_RELATIONS: List all relations in schema\n");
  printf("\n");

  // Accept connections
  while (1) {
    if ((client_fd = accept(server_fd, (struct sockaddr *)&address, &addrlen)) < 0) {
      perror("accept");
      continue;
    }

    printf("New client connected\n");
    handle_client(client_fd, schema);
    printf("Client disconnected\n");
  }

  close(server_fd);
  schema_destroy(schema);
  return 0;
}
