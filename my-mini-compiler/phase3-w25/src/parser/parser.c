/* parser.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "../../include/tokens.h"
#include "../../include/parser.h"
#include "../../include/simple_dynamic_array.h"

/**
 * This is not actually used very much, but it is more for documentation purposes of what the initial values are for an empty node.
 * 
 * Sets the fields of the ParseTreeNode to their default values (all zero, finalized_promo_index = SIZE_MAX). 
 * 
 * Then allocate memory for the children array if capacity is non-zero.
 * @param node The node to initialize. (must not be NULL).
 * @param capacity The initial capacity of the children array.
 */
static inline void ParseTreeNode_init(ParseTreeNode *const node, size_t const capacity) {
    assert(node != NULL);
    node->type = PT_NULL;
    node->token = NULL;
    node->rule = NULL;
    node->finalized_promo_index = SIZE_MAX;
    node->error = PARSE_ERROR_NONE;
    node->children = NULL;
    node->capacity = 0;
    node->count = 0;
    if (capacity) {
        node->children = malloc(capacity * sizeof(ParseTreeNode));
        if (node->children == NULL) {
            perror("malloc");
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
        ParseTreeNode_free_children(node->children + i); 
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
 * WARNING: 
 * - this function naively populates the children of the node according to the production rule, ignore any left-recursion.
 * - if any of `node`, `node->rule`, `input`, `index`, `grammar` are NULL or invalid, then the behavior is undefined.
 * 
 * dependencies:
 * - `parse_cfg_recursive_descent_parse_tree` 
 * 
 * Populate `node->children` (starting from position `node->count` up to at most `node-capacity` children or when `node->rule->tokens` reaches `PT_NULL`, whichever comes first) from the input token stream according to the production rule (`node->rule`) and `grammar`. 
 * 
 * If a child cannot be parsed:
 * - `index` points to the token at the point of the error
 * - `node->error` is set to `PARSE_ERROR_CHILD_ERROR` 
 * - `node->children[node->count]` is initialized with the corresponding error
 * - any remaining `node->children` are left unchanged
 * 
 * In either success or failure:
 * - `node->count` is set to the number of children that were successfully parsed.
 * - `index` is advanced to the first token that is not consumed. 
 * - The populated `node->children` have their types set according to `node->rule->tokens`.
 */
static inline void initialize_children_by_rule(ParseTreeNode *const node, const Token *const input, size_t *const index, const CFG_GrammarRule *const grammar, const size_t grammar_size) {
    for (; node->count < node->capacity; ++node->count) {
        node->children[node->count].type = node->rule->tokens[node->count];
        parse_cfg_recursive_descent_parse_tree(node->children + node->count, index, input, grammar, grammar_size);
        if (node->children[node->count].error) {
            node->error = PARSE_ERROR_CHILD_ERROR;
            break;
        }
    }
}

// `node->count - 1` children already parsed
// 1 child failed to parse
// set remaining children's expected types and error to PARSE_ERROR_PREVIOUS_TOKEN_FAILED_TO_PARSE
static inline void default_error_recovery(ParseTreeNode *const node) {
    ++node->count; // increment count to accept the first child that failed to parse.
    for (; node->count < node->capacity; ++node->count) {
        ParseTreeNode_init(node->children + node->count, 0);
        node->children[node->count].type = node->rule->tokens[node->count];
        node->children[node->count].error = PARSE_ERROR_PREVIOUS_TOKEN_FAILED_TO_PARSE;
    }
}

bool parse_cfg_recursive_descent_parse_tree(ParseTreeNode *const node, size_t *const index, const Token *const input, const CFG_GrammarRule *const grammar, const size_t grammar_size)
{
    assert(node != NULL);
    assert(index != NULL);
    assert(input != NULL);
    assert(grammar != NULL);
    assert(grammar_size >= ParseToken_COUNT_NONTERMINAL);

    // Initialize the node to default values, leaving node->type as is.
    node->error = PARSE_ERROR_NONE;
    node->token = NULL;
    node->rule = NULL;
    node->finalized_promo_index = SIZE_MAX;
    node->capacity = 0;
    node->count = 0;
    node->children = NULL;

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
        } else if (ParseToken_can_start_with(p_rule->tokens[0], (ParseToken)input[*index].type, grammar, grammar_size)) {
            node->rule = p_rule;
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
    initialize_children_by_rule(node, input, index, grammar, grammar_size);
    if (node->error) {
        // this is where you could perform custom error recovery if desired. Such as continue advancing the input until a semi-colon is found for statements.
        // Would have to introduce a new error type for recoveries, such as PARSE_ERROR_CHILD_ERROR_RECOVERED indicating that an error occurred but was recovered from and the parent node may continue parsing while ignoring this error. However, this error type would also have to be propagated to the parent node.
        default_error_recovery(node);
        return false;
    }
    // if the rule is not left-recursive, then this node was successfully parsed.
    // the second check includes the case where the rule is of the form A -> ... | A | ...; in this case, there is nothing left to parse so the node is finished
    if (left_recursive_rule == NULL || left_recursive_rule->tokens[1] == PT_NULL)
        return true;

    size_t left_recursive_rule_num_children = 0;
    while (left_recursive_rule->tokens[left_recursive_rule_num_children] != PT_NULL) 
        ++left_recursive_rule_num_children;
    // Repeatedly parse the rest of the left-recursive rule until it can't be parsed anymore. When it can't be parsed further, just stop and return what worked so far.
    // This left-recursive parsing is a little bit precarious, it has really only been tested with program_grammar.
    // See `check_cfg_grammar` in `grammar.h` for more information on grammar validation.
    while (ParseToken_can_start_with(left_recursive_rule->tokens[1], (ParseToken)input[*index].type, grammar, grammar_size)) {
        // could get away from copying the whole node. but this is easier to understand.
        const ParseTreeNode temp = *node;
        node->children = malloc(left_recursive_rule_num_children * sizeof(ParseTreeNode));
        if (node->children == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        node->token = NULL;
        node->rule = left_recursive_rule;
        node->capacity = left_recursive_rule_num_children;
        node->children[0] = temp;
        node->count = 1;
        initialize_children_by_rule(node, input, index, grammar, grammar_size);
        if (node->error) {
            // this is where you could perform custom error recovery if desired. Such as continue parsing until a semi-colon is found for statements.
            default_error_recovery(node);
            return false;
        }
    }
    return true;
}

void ASTNode_free_children(ASTNode *const node) {
    if (node == NULL || node->items == NULL)
        return;
    for (size_t i = 0; i < node->count; ++i)
        ASTNode_free_children(node->items + i);
    free(node->items);
    node->items = NULL;
    node->count = 0;
    node->capacity = 0;
}

typedef struct _ASTPromo {
    size_t idx;
    ASTNodeType type;
    ASTErrorType error;
} ASTPromo;

/**
 * Warning: `absent` must not be NULL, otherwise this function will not work correctly.
 * 
 * @param p Pointer to the ParseTreeNodeWithPromo to determine its promotion type. This function will set p->finalized_promo_index to the index of the promotion that was used. If p->finalize_promo_index is already set (other than SIZE_MAX), then the function will return the corresponding promotion without recursing through children.
 * @param absent Array of p->count bools to keep track of child nodes which resolved to AST_NULL which could not be promoted.
 */
ASTPromo ASTNode_get_promo(ParseTreeNodeWithPromo *const p, bool *const absent) {
    ASTPromo promo = {p->rule->promote_index, AST_NULL, AST_ERROR_NONE};
    if (p->finalized_promo_index != SIZE_MAX) {
        promo.type = p->rule->ast_types[p->finalized_promo_index];
        promo.idx = p->finalized_promo_index;
        return promo;
    }
    while (true) {
        // this promo_idx is valid, it just means that the result is AST_NULL.
        if (promo.idx == p->count) {
            p->finalized_promo_index = promo.idx;
            promo.type = AST_NULL;
            return promo;
        }
        // promo index is invalid, or we have already tried to promote this child.
        if (promo.idx > p->count || absent[promo.idx]) {
            promo.error = AST_ERROR_EXPECTED_PROMOTION;
            return promo;
        }
        // promo.idx < p->count
        promo.type = p->rule->ast_types[promo.idx];
        if (promo.type != AST_FROM_PROMOTION) {
            p->finalized_promo_index = promo.idx;
            return promo;
        } else {
            bool child_absent[p->children[promo.idx].count];
            memset(child_absent, 0, sizeof(child_absent));
            ASTPromo child_promo = ASTNode_get_promo(p->children + promo.idx, child_absent);
            if (child_promo.error) {
                promo.error = child_promo.error;
                return promo;
            } else if (child_promo.type == AST_FROM_PROMOTION) {
                promo.error = AST_ERROR_EXPECTED_PROMOTION;
                return promo;
            } else if (child_promo.type == AST_NULL) {
                // try the alternate promotion index.
                absent[promo.idx] = true;
                // no alternates, then return AST_NULL.
                if (p->rule->promotion_alternate_if_AST_NULL == NULL) {
                    promo.error = AST_ERROR_EXPECTED_PROMOTION;
                    return promo;
                }
                promo.idx = p->rule->promotion_alternate_if_AST_NULL[promo.idx];
            } else {
                promo.type = child_promo.type;
                p->finalized_promo_index = promo.idx;
                return promo;
            }
        } 
    }

}

bool ASTNode_from_ParseTreeNode_impl(ASTNode *const a, ParseTreeNodeWithPromo *const p) {
    // we can be sure that the pointers are not NULL because the caller of this function has already checked for that.

    if (p->type == PT_NULL)
        return true;
    if (ParseToken_IS_TERMINAL(p->type)) {
        if (p->token == NULL) {
            a->error = AST_ERROR_MISSING_TOKEN;
            return false;
        }
        if (ASTNodeType_HAS_TOKEN(a->type))
            a->token = *p->token;
        if (p->error || a->token.error) {
            a->error = AST_ERROR_TOKEN_ERROR;
            return false;
        }
        return true;
    }
    // parse_node is a non-terminal.
    // take the rule used to parse the node and use it to construct the children.
    const ProductionRule *const rule = p->rule;
    if (rule == NULL) {
        a->error = AST_ERROR_UNSPECIFIED_PRODUCTION_RULE;
        return false;
    }
    
    // evaluate the type of the ASTNode if it is expecting a promotion.
    // a->type will be set to promo.type at the end of this function.
    ASTPromo promo = {SIZE_MAX, a->type, AST_ERROR_NONE};
    bool absent[p->count];
    memset(absent, 0, sizeof(absent));
    if (a->type == AST_FROM_PROMOTION ) {
        promo = ASTNode_get_promo(p, absent);
        if (promo.error) {
            a->error = promo.error;
            return false;
        }
    }
    if (promo.type == AST_NULL || promo.type == AST_SKIP) {
        a->type = AST_SKIP;
        return true;
    }
    // now add the children, appending to the dynamic array.
    for (size_t i = 0; i < p->count; ++i) {
        // if the type is explicitly AST_SKIP, or if the child would have been promoted but wasn't because it was absent, then skip it.
        if (rule->ast_types[i] == AST_SKIP || absent[i])
            continue;
        // need to add the children directly to the array.
        if (rule->ast_types[i] == AST_FROM_CHILDREN) {
            if (!ASTNode_from_ParseTreeNode_impl(a, p->children + i)) {
                a->error = AST_ERROR_CHILD_ERROR;
                return false;
            }
        } else if (i == promo.idx) {
            a->type = rule->ast_types[i];
            // a->type is set according to the promoted child here.
            if (!ASTNode_from_ParseTreeNode_impl(a, p->children + i)) {
                a->error = AST_ERROR_CHILD_ERROR;
                return false;
            }
        // push only a single child to the array.
        } else {
            da_push(a, ((ASTNode){.type = rule->ast_types[i], .error = AST_ERROR_NONE, .token = (Token){0}, .items = NULL, .count = 0, .capacity = 0}));
            if (!ASTNode_from_ParseTreeNode_impl(a->items + a->count - 1, p->children + i)) {
                a->error = AST_ERROR_CHILD_ERROR;
                return false;
            }
            // if it turned out that the child was a skip (determined by promotion), then remove it.
            if (a->items[a->count - 1].type == AST_SKIP || a->type == AST_NULL) {
                --(a->count);
            }
        }
    }

    switch (p->error) {
        case PARSE_ERROR_NONE:
        case PARSE_ERROR_WRONG_TOKEN: // handled in the terminal case.
            break;
        case PARSE_ERROR_CHILD_ERROR:
            a->error = AST_ERROR_CHILD_ERROR;
            break;
        case PARSE_ERROR_NO_RULE_MATCHES:
        case PARSE_ERROR_PREVIOUS_TOKEN_FAILED_TO_PARSE:
            a->error = AST_ERROR_UNSPECIFIED_PRODUCTION_RULE;
            break;
    }

    return a->error == AST_ERROR_NONE;
}

bool ASTNode_from_ParseTreeNode(ASTNode *const ast_node, ParseTreeNodeWithPromo *const parse_node) {
    assert(ast_node != NULL);
    assert(parse_node != NULL);
    // ast_node->type is already set to the desired type.
    // initialize the rest of the ASTNode to default values.
    memset(&(ast_node->error), 0, sizeof(ASTNode) - sizeof(ASTNodeType));
    // call the function given ast_node is initialized to default values.
    return ASTNode_from_ParseTreeNode_impl(ast_node, parse_node);
}