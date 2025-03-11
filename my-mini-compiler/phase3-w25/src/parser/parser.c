/* parser.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "../../include/tokens.h"
#include "../../include/parse_tokens.h"
#include "../../include/grammar.h"
#include "../../include/parser.h"

/**
 * Sets the fields of the ParseTreeNode to their default values (all zero). 
 *
 * ParseTreeNode_init_zero(node, 0) is equivalent to memset(node, 0, sizeof(ParseTreeNode)).
 * 
 * Then allocate memory for the children array if capacity is non-zero.
 * @param node The node to initialize. (must not be NULL).
 * @param capacity The initial capacity of the children array.
 */
static inline void ParseTreeNode_init_zero(ParseTreeNode *node, size_t capacity) {
    assert(node != NULL);
    node->type = PT_NULL;
    node->token = NULL;
    node->error = PARSE_ERROR_NONE;
    node->children = NULL;
    node->capacity = 0;
    node->count = 0;
    if (capacity) {
        node->children = calloc(capacity, sizeof(ParseTreeNode));
        if (node->children == NULL) {
            perror("calloc");
            exit(EXIT_FAILURE);
        }
        node->capacity = capacity;
    }
}

void ParseTreeNode_free_children(ParseTreeNode *node) {
    if (node == NULL || node->children == NULL)
        return; 
    // Recursively free all children 
    for (size_t i = 0; i < node->count; ++i)
        ParseTreeNode_free_children(node->children + i); // Recursively free each child
    // Free the children array
    free(node->children);
    node->children = NULL; 
    node->capacity = 0;
    node->count = 0;
}

void ParseTreeNode_print_simple(ParseTreeNode *node, int level, void (*print_node)(ParseTreeNode*)) {
    for (int i = 0; i < level; ++i)
        printf("  ");
    print_node(node);
    printf("\n");
    for (size_t i = 0; i < node->count; ++i)
        ParseTreeNode_print_simple(node->children + i, level + 1, print_node);
}

/**
 * WARNING: This can go into infinite recursion if the grammar has indirect left-recursion.
 * 
 * @return true if token t could form the prefix of a token sequence that starts with s. If `t == PT_NULL`, then 1 is returned.
 */
bool ParseToken_can_start_with(const ParseToken t, const ParseToken s, const CFG_GrammarRule *grammar, const size_t grammar_size)
{
    assert(grammar != NULL);
    assert(grammar_size >= ParseToken_COUNT_NONTERMINAL);
    if (t == s || t == PT_NULL) {
        return 1;
    } else if (ParseToken_IS_NONTERMINAL(t)) {
        const CFG_GrammarRule *const g_rule = grammar + t - ParseToken_FIRST_NONTERMINAL;
        for (const ProductionRule *p_rule = g_rule->rules, *const end = g_rule->rules + g_rule->num_rules; 
            p_rule < end; ++p_rule) {
            // direct left-recursive rule. Skip this rule to prevent infinite recursion and check the next one.
            if (p_rule->tokens[0] == t) 
                continue;
            // this is where infinite recursion can happen if the grammar has indirect left-recursion.
            if (ParseToken_can_start_with(p_rule->tokens[0], s, grammar, grammar_size)) {
                return 1;
            }
        }
    }
    return 0;
}



bool parse_cfg_recursive_descent_parse_tree(ParseTreeNode *const node, size_t *const index, const Token *const input, const CFG_GrammarRule *const grammar, const size_t grammar_size)
{
    assert(node != NULL);
    assert(index != NULL);
    assert(input != NULL);
    assert(grammar != NULL);
    assert(grammar_size > 0);

    // Initialize the node to default values, leaving node->type as is.
    node->error = PARSE_ERROR_NONE;
    node->token = NULL;
    node->children = NULL;
    node->capacity = 0;
    node->count = 0;

    // PT_NULL node does not consume any input and always succeeds.
    if (node->type == PT_NULL)
        return true;

    // terminal token is assigned to the node. However, if it doesn't match the input, the index is not advanced and a wrong token error is set.
    if (ParseToken_IS_TERMINAL(node->type)) {
        node->token = input + *index;
        if (node->type == (ParseToken)input[*index].type) {
            ++(*index);
            return true;
        } else {
            node->error = PARSE_ERROR_WRONG_TOKEN;
            return false;
        }
    }

    // non-terminal now 
    const CFG_GrammarRule *const g_rule = grammar + node->type - ParseToken_FIRST_NONTERMINAL;

    // the grammar must be deterministic (no common prefixes in the production rules for a given non-terminal), this ensures that the first rule that matches is the only rule that can match and that there is at most one left-recursive rule.

    // p_rule = a pointer to the first rule that can match the input token
    // left_recursive_rule = a pointer to the left-recursive production rule to this non-terminal if it exists. 
    // If multiple left-recursive rules exist, then left_recursive_rule is a pointer to the last one found prior to the first matching non-left-recursive rule. If the grammar is deterministic (which `check_cfg_grammar` in `grammar.h` ensures), then there can be at most one left-recursive rule.
    const ProductionRule *p_rule = g_rule->rules, *left_recursive_rule = NULL;
    for (; p_rule < g_rule->rules + g_rule->num_rules; ++p_rule) {
        if (p_rule->tokens[0] == node->type) {
            left_recursive_rule = p_rule;
        } else if (ParseToken_can_start_with(p_rule->tokens[0], (ParseToken)input[*index].type, grammar, grammar_size)){
            break;
        }
    }
    // p_rule and left_recursive_rule are now set to the correct values and are constant pointers form here onwards.
    // no rule matched the input token.
    if (p_rule == g_rule->rules + g_rule->num_rules) {
        node->error = PARSE_ERROR_NO_RULE_MATCHES;
        node->token = input + *index;
        return false;
    }
    // now parse according to p_rule.
    // allocate memory for the children (if no children, then this was an empty string rule that consumes no input).
    while (p_rule->tokens[node->capacity] != PT_NULL) 
        ++node->capacity;
    if (node->capacity) {
        node->children = malloc(node->capacity * sizeof(ParseTreeNode));
        if (node->children == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
    }
    // parse children (if any). 
    // TODO: make this for loop into a function that parses according to a sequence of ParseTokens with ability to specify how to handle errors.
    for (; node->count < node->capacity; ++node->count) {
        node->children[node->count].type = p_rule->tokens[node->count];
        parse_cfg_recursive_descent_parse_tree(node->children + node->count, index, input, grammar, grammar_size);
        if (node->children[node->count].error) {
            node->error = PARSE_ERROR_CHILD_ERROR;
            // this is where you could perform custom error recovery if desired. Such as see if it was just a semicolon missing or something.
            // node->count successful children parsed
            // 1 child failed to parse
            // set remaining children to error
            for (++node->count; node->count < node->capacity; ++node->count) {
                node->children[node->count].type = p_rule->tokens[node->count];
                node->children[node->count].error = PARSE_ERROR_PREVIOUS_TOKEN_FAILED_TO_PARSE;
                node->children[node->count].token = NULL;
                node->children[node->count].children = NULL;
                node->children[node->count].capacity = 0;
                node->children[node->count].count = 0;
            }
            // return false because the node failed to parse.
            return false;
        }
    }
    // if the rule is not left-recursive, then this node was successfully parsed.
    // the second check includes the case where the rule is of the form A -> ... | A | ...; in this case, there is nothing left to parse so the node is finished
    if (left_recursive_rule == NULL || left_recursive_rule->tokens[1] == PT_NULL)
        return true;

    size_t left_recursive_rule_num_children = 0;
    while (left_recursive_rule->tokens[left_recursive_rule_num_children] != PT_NULL) 
        ++left_recursive_rule_num_children;
    // Try repeatedly parsing the rest of the left-recursive rule until it fails. When it fails, just stop and return what worked so far.
    // might have to change this to rollback if parsing fails even when the first token matches. This is possible only if the second token of the left-recursive may start with the first token of a non-terminal token that may follow immediatly after this left-recursive rule. For example, A -> A B | C; where first(B)\subset follow(A). so if you had expression statements followed by a semicolon, and if you had semi-colon as a left-recursive operation, then if you parse the semicolon but fail to parse the remainder of the expression, then you should rollback the semicolon and end parsing before the semicolon.
    while (!node->error && ParseToken_can_start_with(left_recursive_rule->tokens[1], (ParseToken)input[*index].type, grammar, grammar_size))
    {
        // could get away from copying the whole thing. but this is easier to understand.
        ParseTreeNode temp = *node;
        node->children = malloc(left_recursive_rule_num_children * sizeof(ParseTreeNode));
        if (node->children == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        node->token = NULL;
        node->capacity = left_recursive_rule_num_children;
        node->children[0] = temp;
        node->count = 1;
        // TODO: replace this with function idea mentioned above.
        for (; node->count < node->capacity; ++node->count) {
            node->children[node->count].type = left_recursive_rule->tokens[node->count];
            parse_cfg_recursive_descent_parse_tree(node->children + node->count, index, input, grammar, grammar_size);
            if (node->children[node->count].error != PARSE_ERROR_NONE) {
                node->error = PARSE_ERROR_CHILD_ERROR;
                for (++node->count; node->count < node->capacity; ++node->count) {
                    node->children[node->count].type = left_recursive_rule->tokens[node->count];
                    node->children[node->count].error = PARSE_ERROR_PREVIOUS_TOKEN_FAILED_TO_PARSE;
                    node->children[node->count].token = NULL;
                    node->children[node->count].children = NULL;
                    node->children[node->count].capacity = 0;
                    node->children[node->count].count = 0;
                }
                break;
            }
        }
    }
    return node;
}

bool ASTNode_from_ParseTreeNode_impl(ASTNode *const ast_node, const ParseTreeNode *const parse_node, const CFG_GrammarRule *const grammar, const size_t grammar_size) {
    // we can be sure that the pointers are not NULL because the caller of this function has already checked for that.

    return false;
}

bool ASTNode_from_ParseTreeNode(ASTNode *const ast_node, const ParseTreeNode *const parse_node, const CFG_GrammarRule *const grammar, const size_t grammar_size) {
    assert(ast_node != NULL);
    assert(parse_node != NULL);
    assert(grammar != NULL);
    assert(grammar_size > ParseToken_COUNT_NONTERMINAL);
    // initialize the ASTNode to default values.
    memset(ast_node, 0, sizeof(ASTNode));
    // call the function given ast_node is initialized to default values.
    return ASTNode_from_ParseTreeNode_impl(ast_node, parse_node, grammar, grammar_size);
}