/* parser.c */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "../../include/tokens.h"
#include "../../include/parse_tokens.h"
#include "../../include/grammar.h"
#include "../../include/parser.h"


/*
TODO: Delete everything below, and implement just one function to parse according to a grammar rule.

We have to define the grammar rules for parsing in a data structure. Grammar is an array(indexed by ParseToken enum) of GrammarRule, GrammarRule is an array of ProductionRule. ProductionRule is an array of ParseToken. TODO: incorporate promotion_index as needed into the ProductionRules.

parsing will be recursive descent parsing. Need 1 function to accomplish this that handles two cases:
- right-recursive parsing (standard)
- left-recursive parsing function (only for operator grammar that does not have indirect left recursion https://www.geeksforgeeks.org/removing-direct-and-indirect-left-recursion-in-a-grammar/)

Errors will all be of the form:
- at the token level: just say missing/wrong token (e.g. trying to parse a PT_THEN_KEYWORD_KEYWORD and the token is TOKEN_ELSE, then say "expected THEN, got ELSE") Also, do we consume this token, or leave it for the rest of the parser?
- at the grammar level: say what is missing/unable to parse (e.g. trying to parse a PT_IF_THEN_ELSE and the first token is PT_IF_KEYWORD, but the next token is not a PT_EXPRESSION, then say "expected expression after IF, got something else").

*/

// TODO 1: Add more parsing function declarations for:
// - if statements: if (condition) { ... }
// - while loops: while (condition) { ... }
// - repeat-until: repeat { ... } until (condition)
// - print statements: print x;
// - blocks: { statement1; statement2; }
// - factorial function: factorial(x)


/*
static const char *parse_error(ParseErrorType error)
{
    // TODO 2: Add more error types for:
    // - Missing parentheses
    // - Missing condition
    // - Missing block braces
    // - Invalid operator
    // - Function call errors
    return "Unknown error";
}
*/


void ParseTreeNode_print(ParseTreeNode *node, int level)
{
    for (int i = 0; i < level; ++i)
    {
        printf("  ");
    }
    if (node->token != NULL)
    {
        printf("%d\n", node->token->type);
    }
    else
    {
        printf("%d\n", node->type);
    }
    for (size_t i = 0; i < node->num_children; ++i)
    {
        ParseTreeNode_print(node->children + i, level + 1);
    }
}

/**
 * WARNING: This can go into infinite recursion if the grammar has left-recursion or indirect left-recursion.
 */
int ParseToken_starts_with(const CFG_GrammarRule *grammar, const size_t grammar_size, const ParseToken token_to_check, const ParseToken starts_with)
{
    assert(grammar != NULL);
    assert(grammar_size >= ParseToken_COUNT_NONTERMINAL);
    if (starts_with == PT_NULL || token_to_check == starts_with)
    {
        return 1;
    }
    else if (ParseToken_IS_NONTERMINAL(token_to_check))
    {
        const CFG_GrammarRule *g_rule = grammar + token_to_check - ParseToken_FIRST_NONTERMINAL;
        for (const ProductionRule *p_rule = g_rule->rules; p_rule < g_rule->rules + g_rule->num_rules; ++p_rule)
        {
            // direct left-recursive rule. skip this rule and check the next one.
            if (p_rule->tokens[0] == token_to_check) continue;

            // this is where infinite recursion can happen if the grammar has indirect left-recursion.
            if (ParseToken_starts_with(grammar, grammar_size, p_rule->tokens[0], starts_with))
            {
                return 1;
            }
        }
    }
    return 0;
}

ParseTreeNode *parse_cfg_recursive_descent_parse_tree(const CFG_GrammarRule *grammar, const size_t grammar_size, const ParseToken token, const Token *tokens, size_t *index)
{
    assert(tokens != NULL);
    assert(index != NULL);
    assert(grammar != NULL);
    assert(grammar_size > 0);

    ParseTreeNode *node = malloc(sizeof(ParseTreeNode));
    node->type = token;
    node->children = NULL;
    node->num_children = 0;
    node->token = NULL;
    node->error = AST_ERROR_NONE;

    // terminal is only consumed if there is a match.
    if (ParseToken_IS_TERMINAL(token))
    {
        if (token == (ParseToken)tokens[*index].type)
        {
            node->token = tokens + *index;
            ++(*index);
        }
        else if (token != PT_NULL)
            node->error = PARSE_ERROR_WRONG_TOKEN;
        return node;
    }

    // non-terminal now 
    const CFG_GrammarRule *g_rule = grammar + token - ParseToken_COUNT_NONTERMINAL;

    // Since the grammar is prefix-free (deterministic), there must can be at most 1 left-recursive rule. 
    // look for a rule that matches the tokens. If a token matches, then the rule must work (the rules are assumed to be deterministic, i.e. prefix-free).
    bool rule_match = 0;
    const ProductionRule *p_rule = g_rule->rules, *left_recursive_rule = NULL;
    for (; !rule_match && p_rule < g_rule->rules + g_rule->num_rules; ++p_rule)
    {
        if (p_rule->tokens[0] == PT_NULL)
        {
            rule_match = 1;
        }
        else if (p_rule->tokens[0] == token)
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
        else if (ParseToken_starts_with(grammar, grammar_size, p_rule->tokens[0], tokens[*index].type))
        {
            rule_match = 1;
        }
    }

    if (!rule_match)
    {
        node->error = PARSE_ERROR_NO_RULE_MATCHES;
        return node;
    }

    // now parse according to the rule.
    while (p_rule->tokens[node->num_children] != PT_NULL) ++node->num_children;
    // allocate memory for the children (if no children, then this was an empty string rule that consumes no input).
    if (node->num_children)
        node->children = malloc(node->num_children * sizeof(ParseTreeNode));
    // parse children (if any). 
    // TODO: make this for loop into a function that parses according to a sequence of ParseTokens with ability to specify how to handle errors.
    ParseTreeNode *temp;
    for (size_t i = 0; i < node->num_children; ++i)
    {
        temp = parse_cfg_recursive_descent_parse_tree(grammar, grammar_size, p_rule->tokens[i], tokens, index);
        node->children[i] = *temp;
        free(temp);
        // TODO: add customizability for how to handle parsing errors.
        if (node->children[i].error != PARSE_ERROR_NONE)
        {
            node->error = PARSE_ERROR_NO_RULE_MATCHES;
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
    while (node->error == PARSE_ERROR_NONE && ParseToken_starts_with(grammar, grammar_size, left_recursive_rule->tokens[1], tokens[*index].type))
    {
        temp = malloc(sizeof(ParseTreeNode));
        temp->type = token;
        temp->children = malloc((left_recursive_rule_num_children) * sizeof(ParseTreeNode));
        temp->num_children = left_recursive_rule_num_children;
        temp->children[0] = *node;
        free(node);
        node = temp;
        // TODO: replace this with function idea mentioned above.
        for (size_t i = 1; i < node->num_children; ++i)
        {
            temp = parse_cfg_recursive_descent_parse_tree(grammar, grammar_size, token, tokens, index);
            node->children[i] = *temp;
            free(temp);
            if (node->children[i].error != PARSE_ERROR_NONE)
            {
                node->error = PARSE_ERROR_NO_RULE_MATCHES;
                break;
            }
        }
    }
    return node;
}