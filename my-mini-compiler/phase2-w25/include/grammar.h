#ifndef GRAMMAR_H
#define GRAMMAR_H
#include <stddef.h>
#include <stdbool.h>

#include "parse_tokens.h"
#include "ast_node.h"

typedef struct _ProductionRule
{
    // PT_NULL terminated array of tokens
    ParseToken *tokens;
    // AST_NULL terminated array of ASTNodeType indicating how to convert each ParseToken into an AST node.
    ASTNodeType *ast_types;
    // This indicates the index of the token whose ASTNodeType should be used to replace the parent node's ASTNodeType.
    size_t promote_index;
} ProductionRule;

typedef struct _CFG_GrammarRule
{
    // left-hand-side non-terminal for this grammar rule (e.g. PT_STATEMENT_LIST -> PT_STATEMENT PT_STATEMENT_LIST | PT_EPSILON, where PT_STATEMENT_LIST is the left-hand-side)
    ParseToken lhs;
    // Number of production rules for this non-terminal
    size_t num_rules;
    // Array of `num_rules` production rules
    ProductionRule *rules;
} CFG_GrammarRule;

/**
 * Check if the given grammar rule has indirect left recursion.
 * 
 * This is simply whether lhs_token has a production rule starting with lhs_token.
 * 
 * @return The index of the first ProductionRule that causes direct left recursion, or -1 if there is no direct left recursion.
 */
size_t is_direct_left_recursive(const CFG_GrammarRule *rule);

/**
 * Check if lhs_token in the given grammar is indirect left recursive.
 * 
 * This is true for a produciton rule if the left-most token in the production rule is a non-terminal, and that non-terminal has a production rule which starts with a non-terminal, and this chain continues until the original nonterminal is encountered again. Example: A -> B | c, B -> C | d, C -> A | e. A is indirectly left recursive through B then C, the same can also be said about B and C since they are part of the loop.
 * 
 * @return The index of the first ProductionRule that causes direct left recursion, or -1 if there is no direct left recursion.
 */
size_t is_indirect_left_recursive(const CFG_GrammarRule *grammar, const size_t grammar_size, const ParseToken lhs_token);


#endif /* GRAMMAR_H */