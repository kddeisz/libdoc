#ifndef DOC_NODE_H
#define DOC_NODE_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"

enum doc_node_type {
  CONCAT,
  DEDENT,
  GROUP,
  HARD_LINE,
  IF_BREAK,
  INDENT,
  LINE,
  LITERAL,
  LITERAL_LINE,
  SOFT_LINE
};

typedef struct doc_node {
  enum doc_node_type type;
  size_t size;
  union contents {
    struct doc_node *child;
    struct doc_node **children;
    char *string;
  } contents;
} doc_node_t;

void doc_node_unmake(doc_node_t* doc);

doc_node_t* doc_concat(size_t size, doc_node_t** children);
doc_node_t* doc_concat_n(size_t size, ...);
doc_node_t* doc_dedent(doc_node_t* child);
doc_node_t* doc_group(doc_node_t* child);
doc_node_t* doc_hard_line();
doc_node_t* doc_if_break(doc_node_t* break_child, doc_node_t* flat_child);
doc_node_t* doc_if_break_break_child(doc_node_t* node);
doc_node_t* doc_if_break_flat_child(doc_node_t* node);
doc_node_t* doc_indent(doc_node_t* child);
doc_node_t* doc_join(doc_node_t* separator, size_t size, doc_node_t** content);
doc_node_t* doc_line();
doc_node_t* doc_literal(const char* string);
doc_node_t* doc_literal_n(size_t size, const char* string);
doc_node_t* doc_literal_line();
doc_node_t* doc_soft_line();

#endif
