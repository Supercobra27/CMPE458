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

void ParseTreeNode_free(ParseTreeNode *node, bool free_children)
{
    if (node == NULL)
        return;
    if (free_children)
    {
        for (size_t i = 0; i < node->num_children; ++i)
        {
            ParseTreeNode_free(node->children + i, true);
        }
        free(node->children);
    }
    free(node);
}

// Stack structure for booleans
typedef struct BoolStack {
    bool* data;                     // Dynamic array of booleans
    size_t size;                       // Current number of elements
    size_t capacity;                   // Total allocated capacity
} BoolStack;

// Initialize the stack
BoolStack* create_stack(size_t initial_capacity) {
    BoolStack* stack = (BoolStack*)malloc(sizeof(BoolStack));
    stack->data = (bool*)malloc(initial_capacity * sizeof(bool));
    stack->size = 0;
    stack->capacity = initial_capacity;
    return stack;
}

// Push a value onto the stack
void push(BoolStack* stack, bool value) {
    if (stack->size >= stack->capacity) {
        stack->capacity *= 2;
        stack->data = (bool*)realloc(stack->data, stack->capacity * sizeof(bool));
        if (!stack->data) {
            fprintf(stderr, "Stack realloc failed\n");
            exit(1);
        }
    }
    stack->data[stack->size++] = value;
}

// Pop a value from the stack
void pop(BoolStack* stack) {
    if (stack->size > 0) {
        stack->size--;
    }
}

// Free the stack
void free_stack(BoolStack* stack) {
    free(stack->data);
    free(stack);
}

// Function to print the tree using a stack of booleans for the prefix
void print_tree(ParseTreeNode* node, BoolStack* stack, bool is_last, void (*print_node)(ParseTreeNode*)) {
    if (node == NULL) return;

    // Print the prefix based on the stack and whether this node is the last child.
    for (size_t i = 1; i < stack->size; i++) {
        printf("%s", stack->data[i] ? "| " : "  ");
    }
    if (stack->size > 0) {
        printf("%s", is_last ? "\\-" : "|-");
    }
    
    // Print the current node
    print_node(node);

    // Only process children if there are any
    if (node->num_children == 0) return;

    // Push the current node's "is_last" status (inverted) onto the stack
    // True means "not last" (pipe), False means "last" (spaces)
    push(stack, !is_last);

    // Recursively print all children
    for (size_t i = 0; i < node->num_children; i++) {
        bool is_child_last = (i == node->num_children - 1);
        print_tree(node->children + i, stack, is_child_last, print_node);
    }

    // Pop the current level off the stack
    pop(stack);
}

void ParseTreeRoot_print(ParseTreeNode *root, void (*print_node)(ParseTreeNode*)) 
{
    // Create a stack and print the tree
    BoolStack* stack = create_stack(ParseToken_MAX);
    print_tree(root, stack, true, print_node);       // Root is treated as "last" for simplicity
    free_stack(stack);
}

void ParseTreeNode_print_simple(ParseTreeNode *node, int level, void (*print_node)(ParseTreeNode*))
{
    for (int i = 0; i < level; ++i)
    {
        printf("  ");
    }
    print_node(node);
    for (size_t i = 0; i < node->num_children; ++i)
    {
        ParseTreeNode_print_simple(node->children + i, level + 1, print_node);
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

ParseTreeNode *parse_cfg_recursive_descent_parse_tree(const CFG_GrammarRule *grammar, const size_t grammar_size, const ParseToken token, const Token *tokens, size_t *index)
{
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
    assert(tokens != NULL);
    assert(index != NULL);
    assert(grammar != NULL);
    assert(grammar_size > 0);
    assert(token != PT_NULL);

    ParseTreeNode *node = malloc(sizeof(ParseTreeNode));
    node->type = token;
    node->children = NULL;
    node->num_children = 0;
    node->token = NULL;
    node->error = PARSE_ERROR_NONE;

    // terminal is assigned to the node. However, if it doesn't match the input, the index is not advanced.
    if (ParseToken_IS_TERMINAL(token))
    {
        node->token = tokens + *index;
        if (token == (ParseToken)tokens[*index].type)
            ++(*index);
        else
            node->error = PARSE_ERROR_WRONG_TOKEN;
        return node;
    }

    // non-terminal now 
    const CFG_GrammarRule *g_rule = grammar + token - ParseToken_FIRST_NONTERMINAL;

    // the grammar must be deterministic (no common prefixes in the production rules for a given non-terminal), this ensures that the first rule that matches is the only rule that can match and that there is at most one left-recursive rule.

    const ProductionRule *p_rule = g_rule->rules, *left_recursive_rule = NULL;
    for (; p_rule < g_rule->rules + g_rule->num_rules; ++p_rule)
    {
        if (p_rule->tokens[0] == PT_NULL)
        {
            break;
        }
        if (p_rule->tokens[0] == token)
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
    while (p_rule->tokens[node->num_children] != PT_NULL) 
        ++node->num_children;
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
            node->error = PARSE_ERROR_CHILD_ERROR;
            // perform error recovery here if needed.
            // i successful children parsed
            // 1 child failed to parse
            // node->num_children - i - 1 children left to parse
            for (size_t j = i + 1; j < node->num_children; ++j)
            {
                node->children[j].type = p_rule->tokens[j];
                node->children[j].num_children = 0;
                node->children[j].children = NULL;
                node->children[j].token = NULL;
                node->children[j].error = PARSE_ERROR_PREVIOUS_TOKEN_FAILED_TO_PARSE;
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
            temp = parse_cfg_recursive_descent_parse_tree(grammar, grammar_size, left_recursive_rule->tokens[i], tokens, index);
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