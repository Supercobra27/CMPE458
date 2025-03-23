#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "parser.h"
#include "dynamic_array.h"

#define SEMANTIC_RULE_COUNT 1
#define MAX_ARGS_OPERATOR (size_t)2
#define CHILD_TYPE(node, i) (*(node)).items[i].type
#define CHILD_ITEM(node, i) (*(node)).items[i]
void ProcessNode(ASTNode *ctx, Array *symbol_table);

// Scope tracking functions
void InitializeScopeStack();
void EnterScope();
void ExitScope();
char* GetCurrentScope();
bool ScopesConflict(const char* scope1, const char* scope2);
void CleanupScopeStack();

// Symbol table entry
typedef struct _symEntry {
    ASTNodeType type;
    ASTNode *symNode;
    char *scope;    // String representation of the scope (e.g., "0.1.0")
}symEntry;

typedef Array* ScopeStackType;

#endif /* SEMANTIC_H */