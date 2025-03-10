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
 * Sets all other fields to NULL or zero.
 */
static inline ParseTreeNode *ParseTreeNode_new(ParseToken type, size_t capacity)
{
    ParseTreeNode *node = malloc(sizeof(ParseTreeNode));
    if (node == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    node->type = type;
    node->token = NULL;
    node->error = PARSE_ERROR_NONE;
    node->children = NULL;
    node->count = 0;
    node->capacity = capacity;
    if (capacity)
    {
        node->children = malloc(capacity * sizeof(ParseTreeNode));
        if (node->children == NULL)
        {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
    }
    return node;
}

void ParseTreeNode_free(ParseTreeNode *node, bool free_children) {
    if (node == NULL) 
        return; // Safely handle NULL input

    // Recursively free all children if requested
    if (node->children != NULL) {
        if (free_children)
            for (size_t i = 0; i < node->count; ++i)
                ParseTreeNode_free(node->children[i], true); // Recursively free each child
        free(node->children);
        node->children = NULL; 
    }
    free(node);
}

void ParseTreeNode_print_simple(ParseTreeNode *node, int level, void (*print_node)(ParseTreeNode*))
{
    for (int i = 0; i < level; ++i)
    {
        printf("  ");
    }
    print_node(node);
    for (size_t i = 0; i < node->count; ++i)
    {
        ParseTreeNode_print_simple(node->children[i], level + 1, print_node);
    }
}

/**
 * WARNING: This can go into infinite recursion if the grammar has indirect left-recursion.
 */
bool ParseToken_can_start_with(const CFG_GrammarRule *grammar, const size_t grammar_size, const ParseToken token_to_check, const ParseToken starts_with)
{
    assert(grammar != NULL);
    assert(grammar_size >= ParseToken_COUNT_NONTERMINAL);
    if (starts_with == PT_NULL || token_to_check == starts_with || token_to_check == PT_NULL)
    {
        return 1;
    }
    else if (ParseToken_IS_NONTERMINAL(token_to_check))
    {
        const CFG_GrammarRule *g_rule = grammar + token_to_check - ParseToken_FIRST_NONTERMINAL;
        for (const ProductionRule *p_rule = g_rule->rules; p_rule < g_rule->rules + g_rule->num_rules; ++p_rule)
        {
            // direct left-recursive rule. Skip this rule to prevent infinite recursion and check the next one.
            if (p_rule->tokens[0] == token_to_check) continue;

            // this is where infinite recursion can happen if the grammar has indirect left-recursion.
            if (ParseToken_can_start_with(grammar, grammar_size, p_rule->tokens[0], starts_with))
            {
                return 1;
            }
        }
    }
    return 0;
}

ParseTreeNode *parse_cfg_recursive_descent_parse_tree(const CFG_GrammarRule *grammar, const size_t grammar_size, const ParseToken parse_token, const Token *tokens, size_t *index)
{
    assert(tokens != NULL);
    assert(index != NULL);
    assert(grammar != NULL);
    assert(grammar_size > 0);
    assert(parse_token != PT_NULL);

    ParseTreeNode *node = ParseTreeNode_new(parse_token, 0);

    // terminal is assigned to the node. However, if it doesn't match the input, the index is not advanced and a wrong token error is set.
    if (ParseToken_IS_TERMINAL(parse_token))
    {
        node->token = tokens + *index;
        if (parse_token == (ParseToken)tokens[*index].type)
            ++(*index);
        else
            node->error = PARSE_ERROR_WRONG_TOKEN;
        return node;
    }

    // non-terminal now 
    const CFG_GrammarRule *g_rule = grammar + parse_token - ParseToken_FIRST_NONTERMINAL;

    // the grammar must be deterministic (no common prefixes in the production rules for a given non-terminal), this ensures that the first rule that matches is the only rule that can match and that there is at most one left-recursive rule.

    // TODO: don't do this prefix check, just incorporate this as an outer loop to the for-loop below.
    const ProductionRule *p_rule = g_rule->rules, *left_recursive_rule = NULL;
    for (; p_rule < g_rule->rules + g_rule->num_rules; ++p_rule)
    {
        if (p_rule->tokens[0] == PT_NULL)
        {
            break;
        }
        if (p_rule->tokens[0] == parse_token)
        {
            // if left-recursive, rule must be of the form A -> A B1 B2 ... | C1 C2 C3 ... | D1 D2 D3 ...; where neither C1, nor D1, nor ..., start with A and "B1 B2 ..."!=(empty-string).
            if (left_recursive_rule != NULL)
            {
                node->error = PARSE_ERROR_MULTIPLE_LEFT_RECURSIVE_RULES;
                return node;
            }
            // if the rule was just A -> A | B, then this is not a deterministic grammar, handling this case like this just because it works, if proper validation was done during grammar creation then this check wouldn't be necessary.
            if (p_rule->tokens[1] != PT_NULL)
                left_recursive_rule = p_rule;
        }
        else if (ParseToken_can_start_with(grammar, grammar_size, p_rule->tokens[0], (ParseToken)tokens[*index].type))
        {
            break;
        }
    }

    if (p_rule == g_rule->rules + g_rule->num_rules)
    {
        node->error = PARSE_ERROR_NO_RULE_MATCHES;
        node->token = tokens + *index;
        return node;
    }

    // now parse according to the rule.
    while (p_rule->tokens[node->capacity] != PT_NULL) 
        ++node->capacity;
    // allocate memory for the children (if no children, then this was an empty string rule that consumes no input).
    if (node->capacity)
        node->children = malloc(node->capacity * sizeof(ParseTreeNode*));
    // parse children (if any). 
    // TODO: make this for loop into a function that parses according to a sequence of ParseTokens with ability to specify how to handle errors.
    ParseTreeNode *temp;
    for (; node->count < node->capacity; ++node->count)
    {
        node->children[node->count] = parse_cfg_recursive_descent_parse_tree(grammar, grammar_size, p_rule->tokens[node->count], tokens, index);
        // TODO: add customizability for how to handle parsing errors.
        if (node->children[node->count]->error != PARSE_ERROR_NONE)
        {
            node->error = PARSE_ERROR_CHILD_ERROR;
            // perform error recovery here if needed.
            // node->count successful children parsed
            // 1 child failed to parse
            // node->capacity - node->count - 1 children left to parse
            for (++node->count; node->count < node->capacity; ++node->count)
            {
                node->children[node->count] = ParseTreeNode_new(p_rule->tokens[node->count], 0);
                node->children[node->count]->error = PARSE_ERROR_PREVIOUS_TOKEN_FAILED_TO_PARSE;
            }
            break;
        }
    }
    // if the rule is not left-recursive, or there was a parsing error, then we are done.
    if (left_recursive_rule == NULL || node->error != PARSE_ERROR_NONE)
        return node;

    size_t left_recursive_rule_num_children = 0;
    while (left_recursive_rule->tokens[left_recursive_rule_num_children] != PT_NULL) ++left_recursive_rule_num_children;
    // Try repeatedly parsing the rest of the left-recursive rule until it fails. When it fails, just stop and return what worked so far.
    // might have to change this to rollback if parsing fails even when the first token matches.
    while (node->error == PARSE_ERROR_NONE && ParseToken_can_start_with(grammar, grammar_size, left_recursive_rule->tokens[1], (ParseToken)tokens[*index].type))
    {
        temp = ParseTreeNode_new(parse_token, left_recursive_rule_num_children);
        temp->children[temp->count++] = node;
        node = temp;
        // TODO: replace this with function idea mentioned above.
        for (; node->count < node->capacity; ++node->count)
        {
            node->children[node->count] = parse_cfg_recursive_descent_parse_tree(grammar, grammar_size, left_recursive_rule->tokens[node->count], tokens, index);
            if (node->children[node->count]->error != PARSE_ERROR_NONE)
            {
                // we can conclude no rule matches because ParseToken_can_start_with returned true and the grammar is deterministic with the specific left-recursive rule format.
                node->error = PARSE_ERROR_NO_RULE_MATCHES;
                // node->count successful children parsed
                // 1 child failed to parse
                // node->capacity - node->count - 1 children left to parse
                for (++node->count; node->count < node->capacity; ++node->count)
                {
                    node->children[node->count] = ParseTreeNode_new(p_rule->tokens[node->count], 0);
                    node->children[node->count]->error = PARSE_ERROR_PREVIOUS_TOKEN_FAILED_TO_PARSE;
                }
            }
        }
    }
    return node;
}