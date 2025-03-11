/* parser.h */
#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include "parse_tokens.h"
#include "ast_node.h"
#include "grammar.h"

void ParseTreeNode_free_children(ParseTreeNode *node);
void ParseTreeNode_print_simple(ParseTreeNode *node, int level, void (*print_node)(ParseTreeNode*));


/**
 * Parse a deterministic CFG_GrammarRule using the given tokens and return the resulting ParseTreeNode.
 * 
 * WARNING: This can go into infinite recursion when parsing indirect-left-recursive grammar rules. (direct left recursion will be handled by the parser). Ensure that the grammar does not have indirect left recursion using `is_indirect_left_recursive` function on each grammar rule.
 * 
 * @param node The node to parse into. This node must have its type set to the token desired to be parsed, all other fields are ignored (ensure that children array is deallocated prior to parsing as otherwise there will be a memory leak).
 * @param index The index of the current token to parse, upon termination, this index will point to the next token to parse (if parsing fails, it will point to the first token that could not be parsed).
 * @param input The tokens coming from the Lexer to use for parsing. This array must be terminated with TokenType of TOKEN_EOF.
 * @param grammar An array of `grammar_size` context-free grammar rules to follow to parse `node`.
 * @param grammar_size The size of the grammar array.
 * @return true if the node was successfully parsed and node->error is PARSE_ERROR_NONE, false otherwise.
 */
bool parse_cfg_recursive_descent_parse_tree(ParseTreeNode *const node, size_t *const index, const Token *const input, const CFG_GrammarRule *const grammar, const size_t grammar_size);
/* 
// for each production rule,
    // if the rule is left-recursive. keep track of this rule and skip to the next iteration to find a non-left-recursive rule.
    // not left-recursive: try and parse it and build the node:
        // get the minimum number of children expected to be parsed for this rule and allocate memory for them. If right recursive, then there could be any number of children, so they must be allocated dynamically.
        // start a count of the number of children that were successfully parsed in this rule.
        // for each child token to be parsed:
            // parse it and store the result in node->children, 
            // the child failed to parse,
                // set the node->error to the proper error type. 
                // 
                // break the loop over children.
// if we didn't parse all the children, set the error and return the node.
// if we didn't find a left-recursive production rule, then return the node.
// if we found a left-recursive rule, then then try and parse the rest of the left-recursive rule repeatedly until it fails. When it fails, just stop and return what worked so far.
*/

void ASTNode_free_children(ASTNode *const node);

/**
 * Convert a ParseTreeNode to an ASTNode.
 * 
 * WARNING: Ensure that the memory at address `ast_node->items` is deallocated prior to calling this function as otherwise there will be a memory leak.
 * 
 * @param ast_node The ASTNode to construct from the ParseTreeNode. The `ast_node->type` field will be unchanged, other fields will be filled in by the contents of `parse_node`.
 * @param parse_node The ParseTreeNode to convert to an ASTNode. Assumed to be constructed according to `grammar`, if not, then `UNDEFINED BEHAVIOR`.
 * @param grammar The grammar rules to use to convert the ParseTreeNode to an ASTNode.
 * @param grammar_size The size of the grammar array.
 * @return true if the ASTNode and its children were successfully constructed, false otherwise.
 */
bool ASTNode_from_ParseTreeNode(ASTNode *const ast_node, const ParseTreeNode *const parse_node, const CFG_GrammarRule *const grammar, const size_t grammar_size);


#endif /* PARSER_H */