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
bool parse_cfg_recursive_descent_parse_tree(ParseTreeNode *node, size_t *index, const Token *const input, const CFG_GrammarRule *grammar, const size_t grammar_size);
/* way this could be implemented to handle non-deterministic grammars (where multiple production rules for a given non-terminal possibly have common prefixes):
// start a count of the number of children that were successfully parsed in previous rules (helpful for when there are multiple rules with the same starting tokens).
// for each production rule,
    // if the rule is left-recursive, then make sure that it isn't the second left-recursive rule. keep track of this rule and continue looking for a non-left-recursive rule.
    // not left-recursive: try and parse it and build the node:
        // keep a count of the number of children that need to be parsed and malloc/realloc space for them at node->children.
        // start a count of the number of children that were successfully parsed in this rule.
        // for each child token to be parsed:
            // see if it was already parsed and stored in the node->children, if so, just increment the count of successfully parsed children.
            // if it was not parsed already, recursively parse it and store the result in node->children, if parsing fails, break the loop over children.
        // if not all the children were parsed, then this rule failed to parse. 
        // update the cound of the number of children that were successfully parsed in previous rules.
        // if all children were parsed, then this rule was successful, break the loop over rules.
// if we didn't parse all the children, set the error and return the node.
// if we didn't find a left-recursive production rule, then return the node.
// if we found a left-recursive rule, then then try and parse the rest of the left-recursive rule repeatedly until it fails. When it fails, just stop and return what worked so far.
*/


#endif /* PARSER_H */