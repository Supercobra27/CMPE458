#ifndef GRAMMAR_H
#define GRAMMAR_H
#include "parse_tokens.h"
#include "ast_node.h"

typedef struct _ProductionRule
{
    // Null-terminated array of tokens
    ParseToken *tokens;
    // Null-terminated array of ASTNodeType indicating how to convert each ParseToken into an AST node.
    ASTNodeType *ast_types;
} ProductionRule;

typedef struct _GrammarRule
{
    // left-hand-side non-terminal for this grammar rule (e.g. PT_STATEMENT_LIST -> PT_STATEMENT PT_STATEMENT_LIST | PT_EPSILON, where PT_STATEMENT_LIST is the left-hand-side)
    ParseToken lhs;
    // Null-terminated Array of production rules
    ProductionRule *rules;
} GrammarRule;


#endif /* GRAMMAR_H */