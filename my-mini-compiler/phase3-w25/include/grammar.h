#ifndef GRAMMAR_H
#define GRAMMAR_H
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#include "parse_tokens.h"
#include "ast_node.h"

typedef struct _ProductionRule
{
    // PT_NULL terminated array of tokens
    const ParseToken *tokens;
    // AST_NULL terminated array of ASTNodeType indicating how to convert each ParseToken into an AST node.
    const ASTNodeType *ast_types;
    // This indicates the index of the token whose ASTNodeType should be used to replace the parent node's ASTNodeType.
    // if `promote_index > len(tokens)`, the parent node will be not be replaced by any of the child nodes.
    // if `promote_index == len(tokens)`, the parent node will be replaced by AST_NULL.
    // if `promote_index < len(tokens)`, the parent node will be replaced by the ASTNodeType of the child node at `tokens[promote_index]` (special if ASTNodeType is AST_NULL). 
    size_t promote_index;
    // TODO: what should be done if ast_types[promote_index] is AST_NULL and `promote_index < len(tokens)` (either promote the AST_NULL, or pick a different promote index)?
    // solution to be implemented for semantic analysis: add another field to indicate what to do if `ast_types[promote_index] == AST_NULL`.
    // if `ast_types[promote_index] == AST_NULL`, then if `promotion_alternate_if_AST_NULL[promote_index] == promote_index`, then promote `AST_NULL`, otherwise promote the ASTNodeType at `promotion_alternate_if_AST_NULL[promote_index]`. If there is a cycle (e.g. `promotion_alternate_if_AST_NULL[0] == 1` and `promotion_alternate_if_AST_NULL[1] == 0`), then promote `AST_NULL`.
    // size_t *promotion_alternate_if_AST_NULL;
} ProductionRule;

typedef struct _CFG_GrammarRule
{
    // left-hand-side non-terminal for this grammar rule (e.g. PT_STATEMENT_LIST -> PT_STATEMENT PT_STATEMENT_LIST | PT_EPSILON, where PT_STATEMENT_LIST is the left-hand-side)
    ParseToken lhs;
    // Number of production rules for this non-terminal
    size_t num_rules;
    // Array of `num_rules` production rules
    const ProductionRule *rules;
} CFG_GrammarRule;

typedef struct _CFG_GrammarCheckResult
{
    bool missing_or_mismatched_rules;
    bool contains_improperly_terminated_production_rules;
    bool is_prefix_free;
    bool contains_direct_left_recursion;
    bool contains_direct_left_recursive_rule_as_last_rule;
    bool contains_indirect_left_recursion;
} CFG_GrammarCheckResult;

typedef struct _ParseTreeNode {
    ParseToken type;
    ParseErrorType error;
    const Token *token; // Token in the case of a terminal node.
    const ProductionRule *rule; // Rule used to parse this node. NULL if this is a terminal node.
    size_t count;
    size_t capacity;
    struct _ParseTreeNode *children; // Array of `count` children. `capacity` is the allocated size of the array.
} ParseTreeNode;
 
/**
 * Check if the given grammar rule has indirect left recursion.
 * 
 * This is simply whether lhs_token has a production rule starting with lhs_token.
 * 
 * @return The index of the first ProductionRule that causes direct left recursion, or -1 if there is no direct left recursion.
 */
size_t find_direct_left_recursive(const CFG_GrammarRule *rule);

/**
 * Check if lhs_token in the given grammar is indirect left recursive.
 * 
 * This is true for a produciton rule if the left-most token in the production rule is a non-terminal, and that non-terminal has a production rule which starts with a non-terminal, and this chain continues until the original nonterminal is encountered again. Example: A -> B | c, B -> C | d, C -> A | e. A is indirectly left recursive through B then C, the same can also be said about B and C since they are part of the loop.
 * 
 * @return The index of the first ProductionRule that causes direct left recursion, or -1 if there is no direct left recursion.
 */
size_t find_indirect_left_recursive(const CFG_GrammarRule grammar[ParseToken_COUNT_NONTERMINAL], const ParseToken lhs_token);


/**
 * Check an array of CFG_GrammarRule for the following properties:
 * - Prefix-freeness: true when no two production rules for a given non-terminal have the same starting token.
 * - Direct left recursion: true when no production rule for a given non-terminal starts with the non-terminal itself.
 * - Multiple
 * @param stream Stream to write error messages to. If NULL, no error messages are written.
 * @param grammar Array of CFG_GrammarRule to check.
 */
CFG_GrammarCheckResult check_cfg_grammar(FILE *stream, const CFG_GrammarRule grammar[ParseToken_COUNT_NONTERMINAL]);

static const CFG_GrammarRule program_grammar[ParseToken_COUNT_NONTERMINAL] = {
    {
        .lhs = PT_PROGRAM,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_SCOPE, PT_EOF, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_SCOPE, AST_IGNORE, AST_NULL},
                .promote_index = -1}},
        .num_rules = 1U},
    {
        .lhs = PT_SCOPE,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_STATEMENT_LIST, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_FROM_CHILDREN, AST_NULL},
                .promote_index = -1}},
        .num_rules = 1U},
    {
        .lhs = PT_STATEMENT_LIST,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_STATEMENT, PT_STATEMENT_LIST, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_FROM_CHILDREN, AST_NULL},
                .promote_index = -1},
            {
                .tokens = (ParseToken[]){PT_NULL},
                .ast_types = (ASTNodeType[]){AST_NULL},
                .promote_index = -1}},
        .num_rules = 2U},
    {
        .lhs = PT_STATEMENT,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_EMPTY_STATEMENT, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                .promote_index = 0},
            {
                .tokens = (ParseToken[]){PT_DECLARATION, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_DECLARATION, AST_NULL},
                .promote_index =  0},
            {
                .tokens = (ParseToken[]){PT_EXPRESSION_STATEMENT, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_EXPRESSION, AST_NULL},
                .promote_index =  0},
            {
                .tokens = (ParseToken[]){PT_PRINT_STATEMENT, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_PRINT, AST_NULL},
                .promote_index =  0},
            {
                .tokens = (ParseToken[]){PT_READ_STATEMENT, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_READ, AST_NULL},
                .promote_index =  0},
            {
                .tokens = (ParseToken[]){PT_BLOCK, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_NULL},
                .promote_index =  0},
            {
                .tokens = (ParseToken[]){PT_CONDITIONAL, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_CODITIONAL, AST_NULL},
                .promote_index =  0},
            {
                .tokens = (ParseToken[]){PT_WHILE_LOOP, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_WHILE_LOOP, AST_NULL},
                .promote_index =  0},
            {
                .tokens = (ParseToken[]){PT_REPEAT_UNTIL_LOOP, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_REPEAT_UNTIL_LOOP, AST_NULL},
                .promote_index =  0}},
        .num_rules = 9U},
    {
        .lhs = PT_EMPTY_STATEMENT,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_STATEMENT_END, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                .promote_index =  -1}},
        .num_rules = 1U},
    {
        .lhs = PT_DECLARATION,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_TYPE_KEYWORD, PT_IDENTIFIER, PT_STATEMENT_END, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_IDENTIFIER, AST_IGNORE, AST_NULL},
                .promote_index =  -1}},
        .num_rules = 1U},
    {
        .lhs = PT_EXPRESSION_STATEMENT,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_EXPRESSION_EVAL, PT_STATEMENT_END, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_EXPRESSION, AST_IGNORE, AST_NULL},
                .promote_index = -1}},
        .num_rules = 1U},
    // This is just used to wrap the expression in type AST_EXPRESSION.
    {
        .lhs = PT_EXPRESSION_EVAL,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_EXPRESSION, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_NULL},
                .promote_index = -1}},
        .num_rules = 1U},
    {
        .lhs = PT_PRINT_STATEMENT,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_PRINT_KEYWORD, PT_EXPRESSION_EVAL, PT_STATEMENT_END, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_IGNORE, AST_EXPRESSION, AST_IGNORE, AST_NULL},
                .promote_index = -1}},
        .num_rules = 1U},
    {
        .lhs = PT_READ_STATEMENT,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_READ_KEYWORD, PT_EXPRESSION_EVAL, PT_STATEMENT_END, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_IGNORE, AST_EXPRESSION, AST_IGNORE, AST_NULL},
                .promote_index = -1}},
        .num_rules = 1U},
    {
        .lhs = PT_BLOCK,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_BLOCK_BEGIN, PT_SCOPE, PT_BLOCK_END, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_IGNORE, AST_FROM_CHILDREN, AST_IGNORE, AST_NULL},
                .promote_index = 1}},
        .num_rules = 1U},
    {
        .lhs = PT_CONDITIONAL,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_IF_KEYWORD, PT_EXPRESSION_EVAL, PT_THEN_KEYWORD, PT_BLOCK, PT_OPTIONAL_ELSE_BLOCK, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_IGNORE, AST_EXPRESSION, AST_IGNORE, AST_SCOPE, AST_SCOPE, AST_NULL},
                .promote_index = -1}},
        .num_rules = 1U},
    {
        .lhs = PT_WHILE_LOOP,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_WHILE_KEYWORD, PT_EXPRESSION_EVAL, PT_BLOCK, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_IGNORE, AST_EXPRESSION, AST_SCOPE, AST_NULL},
                .promote_index = -1}},
        .num_rules = 1U},
    {
        .lhs = PT_REPEAT_UNTIL_LOOP,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_REPEAT_KEYWORD, PT_BLOCK, PT_UNTIL_KEYWORD, PT_EXPRESSION_EVAL, PT_STATEMENT_END, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_IGNORE, AST_SCOPE, AST_IGNORE, AST_EXPRESSION, AST_IGNORE, AST_NULL},
                .promote_index = -1}},
        .num_rules = 1U},
    {
        .lhs = PT_OPTIONAL_ELSE_BLOCK,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_ELSE_KEYWORD, PT_BLOCK, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_IGNORE, AST_SCOPE, AST_NULL},
                .promote_index = 1},
            {
                .tokens = (ParseToken[]){PT_NULL},
                .ast_types = (ASTNodeType[]){AST_NULL},
                .promote_index = -1}},
        .num_rules = 2U},

    {
        .lhs = PT_STATEMENT_END,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_SEMICOLON, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                .promote_index = -1}},
        .num_rules = 1U},
    {
        .lhs = PT_TYPE_KEYWORD,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_INT_KEYWORD, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_INT_TYPE, AST_NULL},
                .promote_index = 0},
            {
                .tokens = (ParseToken[]){PT_FLOAT_KEYWORD, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_FLOAT_TYPE, AST_NULL},
                .promote_index = 0},
            {
                .tokens = (ParseToken[]){PT_STRING_KEYWORD, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_STRING_TYPE, AST_NULL},
                .promote_index = 0}},
        .num_rules = 3U},
    {
        .lhs = PT_EXPRESSION,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_ASSIGNMENTEX_R12, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_NULL},
                .promote_index = 0}},
        .num_rules = 1U},
    {
        .lhs = PT_BLOCK_BEGIN,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_LEFT_BRACE, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                .promote_index = -1}},
        .num_rules = 1U},
    {
        .lhs = PT_BLOCK_END,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_RIGHT_BRACE, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                .promote_index = -1}},
        .num_rules = 1U},
            
    // TODO: PT_ASSIGNMENTEX_R12 was not deterministic (prefix-free), this fixes it. However, this fix does not work with the single promote_index, because the promote index would need to be different for each case as can be seen below.
    {
        .lhs = PT_ASSIGNMENTEX_R12,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_OREX_L11, PT_ASSIGNMENT_REST, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_FROM_PROMOTION, AST_NULL},
                .promote_index = 1/* want promote_index = 0 if PT_ASSIGNMENT_REST turns out to be AST_NULL*/,
                /*.promotion_alternate_if_AST_NULL = {0, 0}*/},
            /*{
                .tokens = (ParseToken[]){PT_OREX_L11, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_NULL},
                .promote_index = 0}*/},
        .num_rules = 1U},
    {
        .lhs = PT_ASSIGNMENT_REST,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_ASSIGNMENT_OPERATOR, PT_ASSIGNMENTEX_R12, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_IGNORE, AST_FROM_PROMOTION, AST_NULL},
                .promote_index = 0},
            {
                .tokens = (ParseToken[]){PT_NULL},
                .ast_types = (ASTNodeType[]){AST_NULL},
                .promote_index = 0}},
        .num_rules = 2U},
    {
        .lhs = PT_OREX_L11,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_OREX_L11, PT_OR_OPERATOR, PT_ANDEX_L10, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_IGNORE, AST_FROM_PROMOTION, AST_NULL},
                .promote_index = 1},
            {
                .tokens = (ParseToken[]){PT_ANDEX_L10, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_NULL},
                .promote_index = 0}},
        .num_rules = 2U},
    {
        .lhs = PT_ANDEX_L10,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_ANDEX_L10, PT_AND_OPERATOR, PT_BITOREX_L9, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_IGNORE, AST_FROM_PROMOTION, AST_NULL},
                .promote_index = 1},
            {
                .tokens = (ParseToken[]){PT_BITOREX_L9, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_NULL},
                .promote_index = 0}},
        .num_rules = 2U},
    {
        .lhs = PT_BITOREX_L9,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_BITOREX_L9, PT_BITOR_OPERATOR, PT_BITXOREX_L8, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_IGNORE, AST_FROM_PROMOTION, AST_NULL},
                .promote_index = 1},
            {
                .tokens = (ParseToken[]){PT_BITXOREX_L8, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_NULL},
                .promote_index = 0}},
        .num_rules = 2U},
    {
        .lhs = PT_BITXOREX_L8,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_BITXOREX_L8, PT_BITXOR_OPERATOR, PT_BITANDEX_L7, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_IGNORE, AST_FROM_PROMOTION, AST_NULL},
                .promote_index = 1},
            {
                .tokens = (ParseToken[]){PT_BITANDEX_L7, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_NULL},
                .promote_index = 0}},
        .num_rules = 2U},
    {
        .lhs = PT_BITANDEX_L7,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_BITANDEX_L7, PT_BITAND_OPERATOR, PT_RELATIONEX_L6, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_IGNORE, AST_FROM_PROMOTION, AST_NULL},
                .promote_index = 1},
            {
                .tokens = (ParseToken[]){PT_RELATIONEX_L6, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_NULL},
                .promote_index = 0}},
        .num_rules = 2U},
    {
        .lhs = PT_RELATIONEX_L6,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_RELATIONEX_L6, PT_RELATIONAL_OPERATOR, PT_SHIFTEX_L5, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_IGNORE, AST_FROM_PROMOTION, AST_NULL},
                .promote_index = 1},
            {
                .tokens = (ParseToken[]){PT_SHIFTEX_L5, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_NULL},
                .promote_index = 0}},
        .num_rules = 2U},
    {
        .lhs = PT_SHIFTEX_L5,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_SHIFTEX_L5, PT_SHIFT_OPERATOR, PT_SUMEX_L4, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_IGNORE, AST_FROM_PROMOTION, AST_NULL},
                .promote_index = 1},
            {
                .tokens = (ParseToken[]){PT_SUMEX_L4, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_NULL},
                .promote_index = 0}},
        .num_rules = 2U},
    {
        .lhs = PT_SUMEX_L4,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_SUMEX_L4, PT_SUM_OPERATOR, PT_PRODUCTEX_L3, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_IGNORE, AST_FROM_PROMOTION, AST_NULL},
                .promote_index = 1},
            {
                .tokens = (ParseToken[]){PT_PRODUCTEX_L3, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_NULL},
                .promote_index = 0}},
        .num_rules = 2U},
    {
        .lhs = PT_PRODUCTEX_L3,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_PRODUCTEX_L3, PT_PRODUCT_OPERATOR, PT_UNARYPREFIXEX_R2, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_IGNORE, AST_FROM_PROMOTION, AST_NULL},
                .promote_index = 1},
            {
                .tokens = (ParseToken[]){PT_UNARYPREFIXEX_R2, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_NULL},
                .promote_index = 0}},
        .num_rules = 2U},
    {
        .lhs = PT_UNARYPREFIXEX_R2,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_UNARY_PREFIX_OPERATOR, PT_UNARYPREFIXEX_R2, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_IGNORE, AST_FROM_PROMOTION, AST_NULL},
                .promote_index = 0},
            {
                .tokens = (ParseToken[]){PT_FACTOR, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_NULL},
                .promote_index = 0}},
        .num_rules = 2U},
    {
        .lhs = PT_FACTOR,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_INTEGER_CONST, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_INTEGER, AST_NULL},
                .promote_index = 0},
            {
                .tokens = (ParseToken[]){PT_FLOAT_CONST, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_FLOAT, AST_NULL},
                .promote_index = 0},
            {
                .tokens = (ParseToken[]){PT_STRING_CONST, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_STRING, AST_NULL},
                .promote_index = 0},
            {
                .tokens = (ParseToken[]){PT_IDENTIFIER, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_IDENTIFIER, AST_NULL},
                .promote_index = 0},
            {
                .tokens = (ParseToken[]){PT_FACTORIAL_CALL, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_FACTORIAL, AST_NULL},
                .promote_index = 0},
            {
                .tokens = (ParseToken[]){PT_LEFT_PAREN, PT_EXPRESSION, PT_RIGHT_PAREN, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_IGNORE, AST_FROM_PROMOTION, AST_IGNORE, AST_NULL},
                .promote_index = 1}},
        .num_rules = 6U},
    {
        .lhs = PT_FACTORIAL_CALL,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_FACTORIAL_KEYWORD, PT_LEFT_PAREN, PT_EXPRESSION, PT_RIGHT_PAREN, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_IGNORE, AST_IGNORE, AST_FROM_PROMOTION, AST_IGNORE, AST_NULL},
                .promote_index = 2}},
        .num_rules = 1U},

    {
        .lhs = PT_ASSIGNMENT_OPERATOR,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_ASSIGN_EQUAL, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_ASSIGN_EQUAL, AST_NULL},
                .promote_index = 0}},
        .num_rules = 1U},
    {
        .lhs = PT_OR_OPERATOR,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_LOGICAL_OR, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_LOGICAL_OR, AST_NULL},
                .promote_index = 0}},
        .num_rules = 1U},
    {
        .lhs = PT_AND_OPERATOR,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_LOGICAL_AND, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_LOGICAL_AND, AST_NULL},
                .promote_index = 0}},
        .num_rules = 1U},
    {
        .lhs = PT_BITOR_OPERATOR,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_BITWISE_OR, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_BITWISE_OR, AST_NULL},
                .promote_index = 0}},
        .num_rules = 1U},
    {
        .lhs = PT_BITXOR_OPERATOR,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_BITWISE_XOR, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_BITWISE_XOR, AST_NULL},
                .promote_index = 0}},
        .num_rules = 1U},
    {
        .lhs = PT_BITAND_OPERATOR,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_BITWISE_AND, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_BITWISE_AND, AST_NULL},
                .promote_index = 0}},
        .num_rules = 1U},
    {
        .lhs = PT_RELATIONAL_OPERATOR,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_COMPARE_LESS_EQUAL, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_COMPARE_LESS_EQUAL, AST_NULL},
                .promote_index = 0},
            {
                .tokens = (ParseToken[]){PT_COMPARE_LESS, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_COMPARE_LESS_THAN, AST_NULL},
                .promote_index = 0},
            {
                .tokens = (ParseToken[]){PT_COMPARE_GREATER_EQUAL, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_COMPARE_GREATER_EQUAL, AST_NULL},
                .promote_index = 0},
            {
                .tokens = (ParseToken[]){PT_COMPARE_GREATER, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_COMPARE_GREATER_THAN, AST_NULL},
                .promote_index = 0},
            {
                .tokens = (ParseToken[]){PT_COMPARE_EQUAL, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_COMPARE_EQUAL, AST_NULL},
                .promote_index = 0},
            {
                .tokens = (ParseToken[]){PT_COMPARE_NOT_EQUAL, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_COMPARE_NOT_EQUAL, AST_NULL},
                .promote_index = 0}},
        .num_rules = 6U},
    {
        .lhs = PT_SHIFT_OPERATOR,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_SHIFT_LEFT, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_SHIFT_LEFT, AST_NULL},
                .promote_index = 0},
            {
                .tokens = (ParseToken[]){PT_SHIFT_RIGHT, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_SHIFT_RIGHT, AST_NULL},
                .promote_index = 0}},
        .num_rules = 2U},
    {
        .lhs = PT_SUM_OPERATOR,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_ADD, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_ADD, AST_NULL},
                .promote_index = 0},
            {
                .tokens = (ParseToken[]){PT_SUBTRACT, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_SUBTRACT, AST_NULL},
                .promote_index = 0}},
        .num_rules = 2U},
    {
        .lhs = PT_PRODUCT_OPERATOR,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_MULTIPLY, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_MULTIPLY, AST_NULL},
                .promote_index = 0},
            {
                .tokens = (ParseToken[]){PT_DIVIDE, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_DIVIDE, AST_NULL},
                .promote_index = 0},
            {
                .tokens = (ParseToken[]){PT_MODULO, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_MODULO, AST_NULL},
                .promote_index = 0}},
        .num_rules = 3U},
    {
        .lhs = PT_UNARY_PREFIX_OPERATOR,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_BITWISE_NOT, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_BITWISE_NOT, AST_NULL},
                .promote_index = 0},
            {
                .tokens = (ParseToken[]){PT_LOGICAL_NOT, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_LOGICAL_NOT, AST_NULL},
                .promote_index = 0},
            {
                .tokens = (ParseToken[]){PT_NEGATE, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_NEGATE, AST_NULL},
                .promote_index = 0}},
        .num_rules = 3U},
    {
        .lhs = PT_ASSIGN_EQUAL,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_EQUAL, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                .promote_index = -1}},
        .num_rules = 1U},
    {
        .lhs = PT_LOGICAL_OR,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_PIPE_PIPE, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                .promote_index = -1}},
        .num_rules = 1U},
    {
        .lhs = PT_LOGICAL_AND,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_AMPERSAND_AMPERSAND, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                .promote_index = -1}},
        .num_rules = 1U},
    {
        .lhs = PT_BITWISE_OR,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_PIPE, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                .promote_index = -1}},
        .num_rules = 1U},
    {
        .lhs = PT_BITWISE_XOR,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_CARET, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                .promote_index = -1}},
        .num_rules = 1U},
    {
        .lhs = PT_BITWISE_AND,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_AMPERSAND, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                .promote_index = -1}},
        .num_rules = 1U},
    {
        .lhs = PT_COMPARE_EQUAL,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_EQUAL_EQUAL, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                .promote_index = -1}},
        .num_rules = 1U},
    {
        .lhs = PT_COMPARE_NOT_EQUAL,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_BANG_EQUAL, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                .promote_index = -1}},
        .num_rules = 1U},
    {
        .lhs = PT_COMPARE_LESS_EQUAL,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_LESS_THAN_EQUAL, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                .promote_index = -1}},
        .num_rules = 1U},
    {
        .lhs = PT_COMPARE_LESS,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_LESS_THAN, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                .promote_index = -1}},
        .num_rules = 1U},
    {
        .lhs = PT_COMPARE_GREATER_EQUAL,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_GREATER_THAN_EQUAL, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                .promote_index = -1}},
        .num_rules = 1U},
    {
        .lhs = PT_COMPARE_GREATER,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_GREATER_THAN, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                .promote_index = -1}},
        .num_rules = 1U},
    {
        .lhs = PT_SHIFT_LEFT,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_LESS_THAN_LESS_THAN, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                .promote_index = -1}},
        .num_rules = 1U},
    {
        .lhs = PT_SHIFT_RIGHT,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_GREATER_THAN_GREATER_THAN, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                .promote_index = -1}},
        .num_rules = 1U},
    {
        .lhs = PT_ADD,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_PLUS, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                .promote_index = -1}},
        .num_rules = 1U},
    {
        .lhs = PT_SUBTRACT,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_MINUS, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                .promote_index = -1}},
        .num_rules = 1U},
    {
        .lhs = PT_MULTIPLY,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_STAR, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                .promote_index = -1}},
        .num_rules = 1U},
    {
        .lhs = PT_DIVIDE,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_FORWARD_SLASH, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                .promote_index = -1}},
        .num_rules = 1U},
    {
        .lhs = PT_MODULO,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_PERCENT, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                .promote_index = -1}},
        .num_rules = 1U},
    {
        .lhs = PT_BITWISE_NOT,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_TILDE, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                .promote_index = -1}},
        .num_rules = 1U},
    {
        .lhs = PT_LOGICAL_NOT,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_BANG, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                .promote_index = -1}},
        .num_rules = 1U},
    {
        .lhs = PT_NEGATE,
        .rules = (ProductionRule[]){
            {
                .tokens = (ParseToken[]){PT_MINUS, PT_NULL},
                .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                .promote_index = -1}},
        .num_rules = 1U},
};

#endif /* GRAMMAR_H */