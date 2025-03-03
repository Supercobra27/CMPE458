#ifndef GRAMMAR_H
#define GRAMMAR_H
#include "parse_tokens.h"
#include "ast_node.h"
#include <stddef.h>

typedef struct _ProductionRule
{
    // Null-terminated array of tokens
    ParseToken *tokens;
    // Null-terminated array of ASTNodeType indicating how to convert each ParseToken into an AST node.
    ASTNodeType *ast_types;
    // This indicates the index of the token that should be promoted to replace parent node. Setting this index to (size_t)-1 indicates that no promotion should occur. It is undefined behavior if promote_index is set to the index of a token that is has an ASTNodeType of AST_FROM_CHILDREN. If promote_index is set to the index of a token that has ASTNodeType of AST_IGNORE, the token will be ignored and nothing will be promoted.
    size_t promote_index;
} ProductionRule;

typedef struct _GrammarRule
{
    // left-hand-side non-terminal for this grammar rule (e.g. PT_STATEMENT_LIST -> PT_STATEMENT PT_STATEMENT_LIST | PT_EPSILON, where PT_STATEMENT_LIST is the left-hand-side)
    ParseToken lhs;
    // Null-terminated Array of production rules
    ProductionRule *rules;
} GrammarRule;


#endif /* GRAMMAR_H */