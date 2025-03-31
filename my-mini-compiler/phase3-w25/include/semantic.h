#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "parser.h"
#include "dynamic_array.h"

#define SEMANTIC_RULE_COUNT 1
#define MAX_ARGS_OPERATOR (size_t)2
#define CHILD_TYPE(node, i) (*(node)).items[i].type
#define CHILD_ITEM(node, i) (*(node)).items[i]
/**
 * Semantically verify the given ASTNode `ctx` and populate the symbol table `symbol_table`.
 * 
 * @param ctx The ASTNode to semantically verify. Must be of ASTNodeType `AST_PROGRAM`, otherwise undefined behavior.
 */
Array* ProcessProgram(ASTNode *head, Array *symbol_table, FILE *stream);

// Symbol table entry
typedef struct _symEntry {
    ASTNodeType type;
    const ASTNode *symNode;
    char *scope;    // String representation of the scope (e.g., "0.1.0"), must be freed when this entry is freed.
}symEntry;

typedef Array* ScopeStackType;

#endif /* SEMANTIC_H */