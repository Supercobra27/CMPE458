/* parser.h */
#ifndef PARSER_H
#define PARSER_H

#include "parse_tokens.h"
#include "ast_node.h"

// Parser functions
void ParseTreeNode_print(ParseTreeNode *node, int level);


/**
 * Parse a deterministic CFG_GrammarRule using the given tokens and return the resulting ParseTreeNode.
 * 
 * WARNING: This can go into infinite recursion when parsing indirect-left-recursive grammar rules. (direct left recursion will be handled by the parser). Ensure that the grammar does not have indirect left recursion using `is_indirect_left_recursive` function on each grammar rule.
 * 
 * @param grammar The context free grammar representing the rules to parse.
 * @param grammar_size The size of the grammar array.
 * @param lhs_token The index of the rule to parse in the grammar array.
 * @param tokens The tokens coming from the Lexer to use for parsing. This array must be terminated with TokenType of TOKEN_EOF.
 * @param index The index of the current token to parse, upon termination, this index will point to the next token to parse (if parsing fails, it will point to the first token that could not be parsed).
 */
ParseTreeNode *parse_cfg_recursive_descent_parse_tree(const CFG_GrammarRule *grammar, const size_t grammar_size, const size_t lhs_token, const Token *tokens, size_t *index);


#endif /* PARSER_H */