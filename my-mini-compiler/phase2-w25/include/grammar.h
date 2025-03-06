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

/**
 * Parse a CFG_GrammarRule using the given tokens and return the resulting ASTNode.
 * 
 * WARNING: This can go into infinite recursion when parsing indirect-left-recursive grammar rules. (direct left recursion will be handled by the parser).
 * 
 * @param result_type The type of ASTNode that should be returned
 * @param grammar The grammar containing the rules to parse
 * @param grammar_size the size of the grammar array
 * @param lhs_rule_index The index of the rule to parse in the grammar array
 * @param tokens The tokens coming from the Lexer to use for parsing. This array must be terminated with TOKEN_EOF.
 * @param index The index of the current token to parse, upon termination, this index will point to the next token to parse (will be unchanged if parsing fails or if the grammar rule consumes no tokens).
 */
// ASTNode *parse_cfg_recursive_descent(ASTNodeType result_type, CFG_GrammarRule *grammar, const size_t grammar_size, const size_t lhs_rule_index, Token *tokens, size_t *index);

/**
 * Parse a deterministic CFG_GrammarRule using the given tokens and return the resulting ParseTreeNode.
 * 
 * WARNING: This can go into infinite recursion when parsing indirect-left-recursive grammar rules. (direct left recursion will be handled by the parser).
 * 
 * @param grammar The context free grammar representing the rules to parse.
 * @param grammar_size The size of the grammar array.
 * @param lhs_token The index of the rule to parse in the grammar array.
 * @param tokens The tokens coming from the Lexer to use for parsing. This array must be terminated with TokenType of TOKEN_EOF.
 * @param index The index of the current token to parse, upon termination, this index will point to the next token to parse (will be unchanged if parsing fails or if the grammar rule consumes no tokens).
 */
ParseTreeNode *parse_cfg_recursive_descent_parse_tree(const CFG_GrammarRule *grammar, const size_t grammar_size, const size_t lhs_token, const Token *tokens, size_t *index);



#endif /* GRAMMAR_H */