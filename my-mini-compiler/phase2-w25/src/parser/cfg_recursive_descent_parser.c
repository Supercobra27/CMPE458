#include "../../include/grammar.h"
#include <assert.h>
#include <stdlib.h>

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

/**
 * WARNING: This can go into infinite recursion if the grammar has left-recursion or indirect left-recursion.
 */
int ParseToken_starts_with(const CFG_GrammarRule *grammar, const ParseToken token_to_check, const ParseToken starts_with)
{
    assert(grammar != NULL);
    assert(ParseToken_IS_TERMINAL(starts_with) || starts_with == PT_NULL);

    if (token_to_check == starts_with)
    {
        return 1;
    }
    else if (ParseToken_IS_NONTERMINAL(token_to_check))
    {
        const CFG_GrammarRule *g_rule = grammar + token_to_check - ParseToken_COUNT_NONTERMINAL;
        for (const ProductionRule *p_rule = g_rule->rules; p_rule != NULL; ++p_rule)
        {
            if (ParseToken_starts_with(grammar, p_rule->tokens[0], starts_with))
            {
                return 1;
            }
        }
    }
    return 0;
}

ParseTreeNode *parse_cfg_recursive_descent_parse_tree(const CFG_GrammarRule *grammar, const size_t grammar_size, const ParseToken lhs_token, Token *tokens, size_t *index)
{
    assert(grammar != NULL);
    assert(grammar_size > 0);
    assert(ParseToken_IS_NONTERMINAL(lhs_token));
    assert(tokens != NULL);
    assert(index != NULL);


    ParseTreeNode *node = malloc(sizeof(ParseTreeNode));
    node->type = grammar[lhs_token - ParseToken_COUNT_NONTERMINAL].lhs;
    node->children = NULL;
    node->token = NULL;

    CFG_GrammarRule *g_rule = grammar + lhs_token - ParseToken_COUNT_NONTERMINAL;
    assert(g_rule != NULL);

    // look for a rule that matches the tokens. If a token matches, then the rule must work (the rules are assumed to be deterministic, i.e. prefix-free).
    int rule_success = 0;
    ProductionRule *p_rule = g_rule->rules;
    for (; !rule_success && p_rule != NULL; ++p_rule)
    {
        if (p_rule->tokens[0] == g_rule->lhs)
        {
            // left recursive, deal with this later.
            rule_success = 0;
        }
        // check for terminal token base case or empty rule.
        else if (p_rule->tokens[0] == PT_NULL || ParseToken_starts_with(grammar, p_rule->tokens[0], tokens[*index].type))
        {
            rule_success = 1;
        }
    }

    if (!rule_success)
    {
        node->error = PARSE_ERROR_NO_RULE_MATCHES;
        return node;
    }

    // now parse the tokens according to the rule.
    node->num_children = 0;
    while (p_rule->tokens[node->num_children] != PT_NULL) ++node->num_children;
    node->children = malloc(node->num_children * sizeof(ParseTreeNode));

    for (size_t i = 0; i < node->num_children; ++i)
    {
        ParseToken token = p_rule->tokens[i];
        if (ParseToken_IS_TERMINAL(token) || token == PT_NULL)
        {
            if (token == tokens[*index].type)
            {
                node->children[i].type = token;
                node->children[i].children = NULL;
                node->children[i].token = tokens + *index;
                node->children[i].error = PARSE_ERROR_NONE;
                ++(*index);
            }
            else
            {
                node->children[i].type = token;
                node->children[i].children = NULL;
                node->children[i].token = tokens + *index;
                node->children[i].error = PARSE_ERROR_WRONG_TOKEN;
            }
        }
        else // non-terminal
        {
            ParseTreeNode *p = parse_cfg_recursive_descent_parse_tree(grammar, grammar_size, token, tokens, index);
            node->children[i] = *p;
            free(p);
        }
    }
    return node;
}