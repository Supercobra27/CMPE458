#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "parser.h"
#include "dynamic_array.h"

#define SEMANTIC_RULE_COUNT 1
#define MAX_ARGS_OPERATOR (size_t)2
#define CHILD_TYPE(node, i) (*(node)).items[i].type
#define CHILD_ITEM(node, i) (*(node)).items[i]
void ProcessNode(ASTNode *ctx, Array *symbol_table);

// TODO: Implement this struct to symbol table entry
typedef struct _symEntry {
    ASTNodeType type;
    ASTNode symNode;
};

#endif /* SEMANTIC_H */