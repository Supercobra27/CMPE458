#include "../../include/grammar.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


/*
ASTNode *parse_cfg_recursive_descent(ASTNodeType result_type, const CFG_GrammarRule *grammar, const size_t grammar_size, const size_t lhs_rule_index, const Token *tokens, size_t *index)
{
    // assert grammar does not contain indirect left-recursion. (direct left recursion will be handled).
    assert(grammar != NULL);
    assert(grammar_size > 0);
    assert(lhs_rule_index < grammar_size);
    assert(tokens != NULL);
    assert(index != NULL);

    // Initialize the ASTNode
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = result_type;
    ASTNodeType promoted_type = AST_NULL; // to be added in later.
    
    // Get the rule to parse
    const CFG_GrammarRule *g_rule = &grammar[lhs_rule_index];
    assert(g_rule != NULL);

    // Loop through possible rules to parse until one is successful.
    int rule_success = 0;
    for (ProductionRule *p_rule = g_rule->rules; !rule_success && p_rule != NULL; ++p_rule)
    {
        // Reset the parse index to the start.
        size_t rule_index_in_tokens = *index;
        // Check for direct left-recursion.
        if (p_rule->tokens[0] == g_rule->lhs)
        {
            // deal with this later
        }
        // we are not checking for indirect left-recursion. now assuming that this rule is right-recursive. If this assumption is false then there will be infinite recursion.
        else
        {
            // now try and parse each token according to p_rule.
            rule_success = 1;
            int token_success = 1;
            size_t node_num_children = 0;
            for (size_t p_rule_parse_token_index = 0; node_num_children < MAX_AST_CHILDREN && token_success && p_rule->tokens[p_rule_parse_token_index] != PT_NULL; ++p_rule_parse_token_index)
            {
                ASTNode *child_node = NULL;
                size_t token_index_in_p_rule_tokens = *index;
                // if the expected token is a terminal (base case).
                if (p_rule->tokens[token_index_in_p_rule_tokens] < FIRST_NONTERMINAL_ParseToken)
                {
                    //  And it matches, then if not AST_IGNORE, create the new node. Otherwise, this token is not successful.
                    token_success = (p_rule->tokens[token_index_in_p_rule_tokens] == tokens[token_index_in_p_rule_tokens].type);
                    if (token_success && p_rule->ast_types[token_index_in_p_rule_tokens] != AST_IGNORE)
                    {
                        if (p_rule->promote_index == p_rule_parse_token_index)
                        {
                            // promote the token to the parent node.
                            promoted_type = p_rule->ast_types[token_index_in_p_rule_tokens];
                        }
                        child_node = malloc(sizeof(ASTNode));
                        child_node->type = p_rule->ast_types[token_index_in_p_rule_tokens];
                        if (ASTNodeType_HAS_TOKEN(child_node->type))
                            child_node->token = tokens[token_index_in_p_rule_tokens];
                        node->children[node_num_children++] = child_node;
                    }
                }
                // the token is a non-terminal (have to recursively call)
                else
                {
                    // recursively call the function to parse the non-terminal.
                    ASTNode *non_terminal_node = parse_cfg_recursive_descent(p_rule->ast_types[token_index_in_p_rule_tokens], grammar, grammar_size, p_rule->tokens[token_index_in_p_rule_tokens] - FIRST_NONTERMINAL_ParseToken, tokens, &token_index_in_p_rule_tokens);
                    if (non_terminal_node == NULL)
                    {
                        // something wrong happened while parsing the non-terminal. program should exit.
                        exit(1);
                    }
                    // if the non-terminal was successfully parsed, and it matches the expected ASTNodeType, then process it accordingly (add it as a child node, or ignore, or add it's children, or promote it ). Otherwise, this rule is not successful.
                    if (non_terminal_node->type == p_rule->ast_types[token_index_in_p_rule_tokens])
                    {

                    }
                    
                }
            }
            if (node_num_children == 4)
            {
                // this rule is not successful, too many children.
                rule_success = 0;
            }
            else
            {
                // this rule is successful.
                *index = rule_index_in_tokens;
            }
        }
    }

}
*/


size_t is_direct_left_recursive(const CFG_GrammarRule *rule)
{
    assert(rule != NULL);
    for (size_t i = 0; i < rule->num_rules; ++i)
    {
        if (rule->rules[i].tokens[0] == rule->lhs)
        {
            return i;
        }
    }
    return (size_t)-1;
}


size_t is_indirect_left_recursive(const CFG_GrammarRule *grammar, const size_t grammar_size, const ParseToken lhs_token)
{
    assert(grammar != NULL);
    assert(grammar_size > 0);
    assert(ParseToken_IS_NONTERMINAL(lhs_token));
    // need bool array to keep track of visited non-terminals.
    struct elem
    {
        size_t token;
        size_t rule;
    };
    // can only add each non-terminal at most once to the queue (since we only add unvisited non-terminals, and once visited, they are never visited again).
    struct elem queue[grammar_size];
    bool visited[grammar_size];
    memset(visited, 0, grammar_size * sizeof(bool));
    // struct elem *queue = malloc(grammar_size * sizeof(struct elem));
    // bool *visited = calloc(grammar_size, sizeof(bool));
    size_t queue_back = 0, queue_front = 0;
    size_t result = (size_t)-1;
    const size_t target = lhs_token - ParseToken_FIRST_NONTERMINAL;
    // add all left-most non-terminals from production ruls of lhs_token to the queue. So we can keep track of which rules are left-recursive.
    for (size_t i = 0; i < grammar[target].num_rules; ++i)
    {
        if (ParseToken_IS_NONTERMINAL(grammar[target].rules[i].tokens[0]) && !visited[grammar[target].rules[i].tokens[0] - ParseToken_FIRST_NONTERMINAL] && grammar[target].rules[i].tokens[0] != lhs_token)
        {
            queue[queue_back++] = (struct elem){grammar[target].rules[i].tokens[0] - ParseToken_FIRST_NONTERMINAL, i};
            visited[grammar[target].rules[i].tokens[0] - ParseToken_FIRST_NONTERMINAL] = 1;
        }
    }
    // while the queue is not empty, keep adding unvisited left-most non-terminals to the queue.
    while (queue_front < queue_back)
    {
        struct elem e = queue[queue_front++];
        if (e.token == target)
        {
            result = e.rule;
            break;
        }
        for (size_t i = 0; i < grammar[e.token].num_rules; ++i)
        {
            if (ParseToken_IS_NONTERMINAL(grammar[e.token].rules[i].tokens[0]) && !visited[grammar[e.token].rules[i].tokens[0] - ParseToken_FIRST_NONTERMINAL])
            {
                queue[queue_back++] = (struct elem){grammar[e.token].rules[i].tokens[0] - ParseToken_FIRST_NONTERMINAL, e.rule};
                visited[grammar[e.token].rules[i].tokens[0] - ParseToken_FIRST_NONTERMINAL] = 1;
            }
        }
    }
    free(visited);
    free(queue);
    return result;
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
    node->error = PARSE_ERROR_NONE;

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
            break;
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
            temp = parse_cfg_recursive_descent_parse_tree(grammar, grammar_size, token, tokens, &index);
            node->children[i] = *temp;
            free(temp);
            if (temp->children[i].error != PARSE_ERROR_NONE)
                break;
        }
    }
    return node;
}