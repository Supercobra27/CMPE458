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
    ASTNode *symNode;
    char *scope;
}symEntry;

typedef Array* scopeStack; // Wrapper

typedef struct _symEntry2 {
    ASTNodeType type;
    ASTNode *symNode;
    size_t scope; // Height of Stack
}symEntry2;

#endif /* SEMANTIC_H */