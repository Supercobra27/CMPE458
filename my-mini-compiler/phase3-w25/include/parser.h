/* parser.h */
#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include "grammar.h"
#include "tokens.h"

/**
 * @param token must remain a valid pointer for the lifetime of the ParseTreeNode.
 * @param rule must remain a valid pointer for the lifetime of the ParseTreeNode.
 * @param children must remain a valid pointer to an block of memory of size `capacity * sizeof(ParseTreeNode)` for the lifetime of the ParseTreeNode, or NULL if capacity is 0. The first `count` elements of the array must be valid ParseTreeNode.
 * @param count must be less than or equal to `capacity`.
 */
typedef struct _ParseTreeNode {
    ParseToken type;
    ParseErrorType error;
    const Token *token; // Token associated with this node. NULL iff ParseToken_IS_NONTERMINAL(type).
    const ProductionRule *rule; // Rule used to parse this node. NULL iff ParseToken_IS_TERMINAL(type).
    size_t finalized_promo_index;
    size_t count;
    size_t capacity;
    struct _ParseTreeNode *children; // Array of `count` children. `capacity` is the allocated size of the array.
} ParseTreeNode;

// same as ParseTreeNode but the only difference is that everything is const except for `finalized_promo_index` and `children`.
typedef struct _ParseTreeNodeWithPromo {
    ParseToken const type;
    ParseErrorType const error;
    const Token *const token; // Token associated with this node. NULL iff ParseToken_IS_NONTERMINAL(type).
    const ProductionRule *const rule; // Rule used to parse this node. NULL iff ParseToken_IS_TERMINAL(type).
    size_t finalized_promo_index;
    size_t const count;
    size_t const capacity;
    struct _ParseTreeNodeWithPromo *const children; // Array of `count` children. `capacity` is the allocated size of the array.
} ParseTreeNodeWithPromo;

// Abstract Syntax Tree Node structure
typedef struct ASTNode {
    ASTNodeType type;           // Type of node
    ASTErrorType error;         // Error type
    Token token;               // Token associated with this node, TOKEN_NULL if none.
    size_t count;
    size_t capacity;
    struct ASTNode *items; // Array of child nodes
} ASTNode;
 

void ParseTreeNode_free_children(ParseTreeNode *node);
void ParseTreeNode_print_simple(ParseTreeNode *node, int level, void (*print_node)(ParseTreeNode*));


/**
 * Parse a deterministic CFG_GrammarRule using the given tokens and return the resulting ParseTreeNode.
 * 
 * WARNING: 
 * - This can go into infinite recursion when parsing indirect-left-recursive grammar rules. (direct left recursion will be handled by the parser). Ensure that the grammar does not have indirect left recursion using `is_indirect_left_recursive` function on each grammar rule.
 * - This function assumes that the grammar is deterministic (no common prefixes in the production rules for a given non-terminal). This is ensured by `check_cfg_grammar` in `grammar.h`.
 * 
 * @param node The node to parse into. This node must have its type set to the token desired to be parsed, all other fields are ignored (ensure that children array is deallocated prior to parsing as otherwise there will be a memory leak).
 * @param index The index of the current token to parse, upon termination, this index will point to the next token to parse (if parsing fails, it will point to the first token that could not be parsed).
 * @param input The tokens coming from the Lexer to use for parsing. This array must be terminated with TokenType of TOKEN_EOF.
 * @param grammar An array of `grammar_size` context-free grammar rules to follow to parse `node`.
 * @param grammar_size The size of the grammar array.
 * @return true if the node was successfully parsed and node->error is PARSE_ERROR_NONE, false otherwise.
 */
bool parse_cfg_recursive_descent_parse_tree(ParseTreeNode *const node, size_t *const index, const Token *const input, const CFG_GrammarRule *const grammar, const size_t grammar_size);

void ASTNode_free_children(ASTNode *const node);

/**
 * Convert a ParseTreeNode to an ASTNode.
 * 
 * WARNING: Ensure that the memory at address `ast_node->items` is deallocated prior to calling this function as otherwise there will be a memory leak.
 * 
 * @param ast_node The ASTNode to construct from the ParseTreeNode. If `parse_node->rule->promote_index` is specified, then `ast_node->type` will be set by a promoted child, otherwise it will be left unchanged. Other fields will be filled in by the contents of `parse_node`.
 * @param parse_node The ParseTreeNode to convert to an ASTNode. This node and its children must have a valid pointer to the ProductionRule used to parse it.
 * @param grammar The grammar rules to use to convert the ParseTreeNode to an ASTNode.
 * @param grammar_size The size of the grammar array.
 * @return true if the ASTNode and its children were successfully constructed, false otherwise.
 */
bool ASTNode_from_ParseTreeNode(ASTNode *const ast_node, ParseTreeNodeWithPromo *const parse_node);


#endif /* PARSER_H */