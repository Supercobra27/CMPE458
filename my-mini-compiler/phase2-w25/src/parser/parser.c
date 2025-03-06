/* parser.c */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "../../include/lexer.h"
#include "../../include/tokens.h"
#include "../../include/parse_tokens.h"
#include "../../include/grammar.h"
#include "../../include/parser.h"

#include "../../include/dynamic_array.h"

/*
TODO: Delete everything below, and implement just one function to parse according to a grammar rule.

We have to define the grammar rules for parsing in a data structure. Grammar is an array(indexed by ParseToken enum) of GrammarRule, GrammarRule is an array of ProductionRule. ProductionRule is an array of ParseToken. TODO: incorporate promotion_index as needed into the ProductionRules.

parsing will be recursive descent parsing. Need 1 function to accomplish this that handles two cases:
- right-recursive parsing (standard)
- left-recursive parsing function (only for operator grammar that does not have indirect left recursion https://www.geeksforgeeks.org/removing-direct-and-indirect-left-recursion-in-a-grammar/)

Errors will all be of the form:
- at the token level: just say missing/wrong token (e.g. trying to parse a PT_THEN_KEYWORD and the token is TOKEN_ELSE, then say "expected THEN, got ELSE") Also, do we consume this token, or leave it for the rest of the parser?
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


// Main function for testing
int main()
{
    const CFG_GrammarRule grammar[ParseToken_COUNT_NONTERMINAL] = {
        (CFG_GrammarRule){
            .lhs = PT_PROGRAM,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_STATEMENT_LIST, PT_EOF, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_STATEMENT_LIST, AST_IGNORE, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_STATEMENT_LIST,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_STATEMENT, PT_STATEMENT_LIST, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_STATEMENT_LIST, AST_NULL},
                    .promote_index = -1},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_NULL},
                    .promote_index = -1}},
            .num_rules = 2U},
        (CFG_GrammarRule){
            .lhs = PT_STATEMENT,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_EMPTY_STATEMENT, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                    .promote_index = 0},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_DECLARATION, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_DECLARATION, AST_NULL},
                    .promote_index =  0},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_EXPRESSION_STATEMENT, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_EXPRESSION, AST_NULL},
                    .promote_index =  0},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_PRINT_STATEMENT, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_PRINT, AST_NULL},
                    .promote_index =  0},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_BLOCK, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_BLOCK, AST_NULL},
                    .promote_index =  0},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_CONDITIONAL, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_CODITIONAL, AST_NULL},
                    .promote_index =  0},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_WHILE_LOOP, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_WHILE_LOOP, AST_NULL},
                    .promote_index =  0},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_REPEAT_UNTIL_LOOP, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_REPEAT_UNTIL_LOOP, AST_NULL},
                    .promote_index =  0}},
            .num_rules = 8U},
        (CFG_GrammarRule){
            .lhs = PT_EMPTY_STATEMENT,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_STATEMENT_END, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                    .promote_index =  -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_DECLARATION,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_TYPE_KEYWORD, PT_IDENTIFIER, PT_STATEMENT_END, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_IDENTIFIER, AST_IGNORE, AST_NULL},
                    .promote_index =  -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_EXPRESSION_STATEMENT,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_EXPRESSION_EVAL, PT_STATEMENT_END, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_EXPRESSION, AST_IGNORE, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        // This is just used to wrap the expression in type AST_EXPRESSION.
        (CFG_GrammarRule){
            .lhs = PT_EXPRESSION_EVAL,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_EXPRESSION, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_PRINT_STATEMENT,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_PRINT_KEYWORD, PT_EXPRESSION_EVAL, PT_STATEMENT_END, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_EXPRESSION, AST_IGNORE, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_BLOCK,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_BLOCK_BEGIN, PT_STATEMENT_LIST, PT_BLOCK_END, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_STATEMENT_LIST, AST_IGNORE, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_CONDITIONAL,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_IF_KEYWORD, PT_EXPRESSION_EVAL, PT_THEN_KEYWORD, PT_BLOCK, PT_OPTIONAL_ELSE_BLOCK, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_EXPRESSION, AST_IGNORE, AST_BLOCK, AST_FROM_CHILDREN, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_WHILE_LOOP,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_WHILE_KEYWORD, PT_EXPRESSION_EVAL, PT_BLOCK, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_EXPRESSION, AST_BLOCK, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_REPEAT_UNTIL_LOOP,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_REPEAT_KEYWORD, PT_BLOCK, PT_UNTIL_KEYWORD, PT_EXPRESSION_EVAL, PT_STATEMENT_END, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_BLOCK, AST_IGNORE, AST_EXPRESSION, AST_IGNORE, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_OPTIONAL_ELSE_BLOCK,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_ELSE_KEYWORD, PT_BLOCK, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_BLOCK, AST_NULL},
                    .promote_index = -1},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_NULL},
                    .promote_index = -1}},
            .num_rules = 2U},

        (CFG_GrammarRule){
            .lhs = PT_STATEMENT_END,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_SEMICOLON, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_TYPE_KEYWORD,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_INT_KEYWORD, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_INT_TYPE, AST_NULL},
                    .promote_index = 0},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_FLOAT_KEYWORD, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FLOAT_TYPE, AST_NULL},
                    .promote_index = 0},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_STRING_KEYWORD, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_STRING_TYPE, AST_NULL},
                    .promote_index = 0}},
            .num_rules = 3U},
        (CFG_GrammarRule){
            .lhs = PT_EXPRESSION,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_ASSIGNMENTEX_R12, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_NULL},
                    .promote_index = 0}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_BLOCK_BEGIN,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_LEFT_BRACE, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_BLOCK_END,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_RIGHT_BRACE, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
            
        (CFG_GrammarRule){
            .lhs = PT_ASSIGNMENTEX_R12,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_OREX_L11, PT_ASSIGNMENT_OPERATOR, PT_ASSIGNMENTEX_R12, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_IGNORE, AST_FROM_PROMOTION, AST_NULL},
                    .promote_index = 1},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_OREX_L11, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_NULL},
                    .promote_index = 0}},
            .num_rules = 2U},
        (CFG_GrammarRule){
            .lhs = PT_OREX_L11,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_OREX_L11, PT_OR_OPERATOR, PT_ANDEX_L10, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_IGNORE, AST_FROM_PROMOTION, AST_NULL},
                    .promote_index = 1},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_ANDEX_L10, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_NULL},
                    .promote_index = 0}},
            .num_rules = 2U},
        (CFG_GrammarRule){
            .lhs = PT_ANDEX_L10,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_ANDEX_L10, PT_AND_OPERATOR, PT_BITOREX_L9, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_IGNORE, AST_FROM_PROMOTION, AST_NULL},
                    .promote_index = 1},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_BITOREX_L9, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_NULL},
                    .promote_index = 0}},
            .num_rules = 2U},
        (CFG_GrammarRule){
            .lhs = PT_BITOREX_L9,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_BITOREX_L9, PT_BITOR_OPERATOR, PT_BITXOREX_L8, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_IGNORE, AST_FROM_PROMOTION, AST_NULL},
                    .promote_index = 1},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_BITXOREX_L8, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_NULL},
                    .promote_index = 0}},
            .num_rules = 2U},
        (CFG_GrammarRule){
            .lhs = PT_BITXOREX_L8,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_BITXOREX_L8, PT_BITXOR_OPERATOR, PT_BITANDEX_L7, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_IGNORE, AST_FROM_PROMOTION, AST_NULL},
                    .promote_index = 1},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_BITANDEX_L7, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_NULL},
                    .promote_index = 0}},
            .num_rules = 2U},
        (CFG_GrammarRule){
            .lhs = PT_BITANDEX_L7,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_BITANDEX_L7, PT_BITAND_OPERATOR, PT_RELATIONEX_L6, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_IGNORE, AST_FROM_PROMOTION, AST_NULL},
                    .promote_index = 1},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_RELATIONEX_L6, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_NULL},
                    .promote_index = 0}},
            .num_rules = 2U},
        (CFG_GrammarRule){
            .lhs = PT_RELATIONEX_L6,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_RELATIONEX_L6, PT_RELATIONAL_OPERATOR, PT_SHIFTEX_L5, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_IGNORE, AST_FROM_PROMOTION, AST_NULL},
                    .promote_index = 1},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_SHIFTEX_L5, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_NULL},
                    .promote_index = 0}},
            .num_rules = 2U},
        (CFG_GrammarRule){
            .lhs = PT_SHIFTEX_L5,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_SHIFTEX_L5, PT_SHIFT_OPERATOR, PT_SUMEX_L4, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_IGNORE, AST_FROM_PROMOTION, AST_NULL},
                    .promote_index = 1},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_SUMEX_L4, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_NULL},
                    .promote_index = 0}},
            .num_rules = 2U},
        (CFG_GrammarRule){
            .lhs = PT_SUMEX_L4,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_SUMEX_L4, PT_SUM_OPERATOR, PT_PRODUCTEX_L3, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_IGNORE, AST_FROM_PROMOTION, AST_NULL},
                    .promote_index = 1},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_PRODUCTEX_L3, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_NULL},
                    .promote_index = 0}},
            .num_rules = 2U},
        (CFG_GrammarRule){
            .lhs = PT_PRODUCTEX_L3,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_PRODUCTEX_L3, PT_PRODUCT_OPERATOR, PT_UNARYPREFIXEX_R2, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_IGNORE, AST_FROM_PROMOTION, AST_NULL},
                    .promote_index = 1},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_UNARYPREFIXEX_R2, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_NULL},
                    .promote_index = 0}},
            .num_rules = 2U},
        (CFG_GrammarRule){
            .lhs = PT_UNARYPREFIXEX_R2,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_UNARY_PREFIX_OPERATOR, PT_UNARYPREFIXEX_R2, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_FROM_PROMOTION, AST_NULL},
                    .promote_index = 0},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_FACTOR, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_NULL},
                    .promote_index = 0}},
            .num_rules = 2U},
        (CFG_GrammarRule){
            .lhs = PT_FACTOR,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_INTEGER_CONST, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_INTEGER, AST_NULL},
                    .promote_index = 0},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_FLOAT_CONST, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FLOAT, AST_NULL},
                    .promote_index = 0},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_STRING_CONST, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_STRING, AST_NULL},
                    .promote_index = 0},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_IDENTIFIER, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IDENTIFIER, AST_NULL},
                    .promote_index = 0},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_FACTORIAL_CALL, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FACTORIAL, AST_NULL},
                    .promote_index = 0},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_LEFT_PAREN, PT_EXPRESSION, PT_RIGHT_PAREN, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_FROM_PROMOTION, AST_IGNORE, AST_NULL},
                    .promote_index = 1}},
            .num_rules = 6U},
        (CFG_GrammarRule){
            .lhs = PT_FACTORIAL_CALL,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_FACTORIAL_KEYWORD, PT_LEFT_PAREN, PT_EXPRESSION, PT_RIGHT_PAREN, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_IGNORE, AST_FROM_PROMOTION, AST_IGNORE, AST_NULL},
                    .promote_index = 2}},
            .num_rules = 1U},

        (CFG_GrammarRule){
            .lhs = PT_ASSIGNMENT_OPERATOR,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_ASSIGN_EQUAL, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_ASSIGN_EQUAL, AST_NULL},
                    .promote_index = 0}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_OR_OPERATOR,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_LOGICAL_OR, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_LOGICAL_OR, AST_NULL},
                    .promote_index = 0}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_AND_OPERATOR,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_LOGICAL_AND, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_LOGICAL_AND, AST_NULL},
                    .promote_index = 0}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_BITOR_OPERATOR,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_BITWISE_OR, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_BITWISE_OR, AST_NULL},
                    .promote_index = 0}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_BITXOR_OPERATOR,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_BITWISE_XOR, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_BITWISE_XOR, AST_NULL},
                    .promote_index = 0}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_BITAND_OPERATOR,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_BITWISE_AND, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_BITWISE_AND, AST_NULL},
                    .promote_index = 0}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_RELATIONAL_OPERATOR,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_COMPARE_LESS_EQUAL, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_COMPARE_LESS_EQUAL, AST_NULL},
                    .promote_index = 0},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_COMPARE_LESS, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_COMPARE_LESS_THAN, AST_NULL},
                    .promote_index = 0},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_COMPARE_GREATER_EQUAL, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_COMPARE_GREATER_EQUAL, AST_NULL},
                    .promote_index = 0},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_COMPARE_GREATER, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_COMPARE_GREATER_THAN, AST_NULL},
                    .promote_index = 0},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_COMPARE_EQUAL, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_COMPARE_EQUAL, AST_NULL},
                    .promote_index = 0},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_COMPARE_NOT_EQUAL, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_COMPARE_NOT_EQUAL, AST_NULL},
                    .promote_index = 0}},
            .num_rules = 6U},
        (CFG_GrammarRule){
            .lhs = PT_SHIFT_OPERATOR,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_SHIFT_LEFT, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_SHIFT_LEFT, AST_NULL},
                    .promote_index = 0},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_SHIFT_RIGHT, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_SHIFT_RIGHT, AST_NULL},
                    .promote_index = 0}},
            .num_rules = 2U},
        (CFG_GrammarRule){
            .lhs = PT_SUM_OPERATOR,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_ADD, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_ADD, AST_NULL},
                    .promote_index = 0},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_SUBTRACT, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_SUBTRACT, AST_NULL},
                    .promote_index = 0}},
            .num_rules = 2U},
        (CFG_GrammarRule){
            .lhs = PT_PRODUCT_OPERATOR,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_MULTIPLY, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_MULTIPLY, AST_NULL},
                    .promote_index = 0},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_DIVIDE, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_DIVIDE, AST_NULL},
                    .promote_index = 0},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_MODULO, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_MODULO, AST_NULL},
                    .promote_index = 0}},
            .num_rules = 3U},
        (CFG_GrammarRule){
            .lhs = PT_UNARY_PREFIX_OPERATOR,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_BITWISE_NOT, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_BITWISE_NOT, AST_NULL},
                    .promote_index = 0},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_LOGICAL_NOT, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_LOGICAL_NOT, AST_NULL},
                    .promote_index = 0},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_NEGATE, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_NEGATE, AST_NULL},
                    .promote_index = 0}},
            .num_rules = 3U},
        (CFG_GrammarRule){
            .lhs = PT_ASSIGN_EQUAL,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_SINGLE_EQUALS, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_LOGICAL_OR,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_DOUBLE_PIPE, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_LOGICAL_AND,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_DOUBLE_AMPERSAND, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_BITWISE_OR,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_SINGLE_PIPE, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_BITWISE_XOR,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_CARET, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_BITWISE_AND,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_SINGLE_AMPERSAND, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_COMPARE_EQUAL,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_DOUBLE_EQUALS, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_COMPARE_NOT_EQUAL,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_BANG_EQUALS, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_COMPARE_LESS_EQUAL,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_LESS_THAN_EQUALS, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_COMPARE_LESS,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_LESS_THAN, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_COMPARE_GREATER_EQUAL,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_GREATER_THAN_EQUALS, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_COMPARE_GREATER,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_GREATER_THAN, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_SHIFT_LEFT,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_DOUBLE_LESS_THAN, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_SHIFT_RIGHT,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_DOUBLE_GREATER_THAN, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_ADD,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_PLUS, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_SUBTRACT,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_MINUS, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_MULTIPLY,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_STAR, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_DIVIDE,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_FORWARD_SLASH, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_MODULO,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_PERCENT, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_BITWISE_NOT,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_TILDE, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_LOGICAL_NOT,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_BANG, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_NEGATE,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_MINUS, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
    };

    // Validate Grammar
    for (ParseToken t = ParseToken_FIRST_NONTERMINAL; ParseToken_IS_NONTERMINAL(t); ++t)
    {
        // ensure the grammar has a rule for each non-terminal in the correct index
        if (grammar[t-ParseToken_FIRST_NONTERMINAL].lhs != t)
        {
            printf("Grammar rule for %d is missing (index %d)\n", t, t-ParseToken_FIRST_NONTERMINAL);
            exit(1);
        }
        // printf("Checking for production rules for %d\n", t);
        // iterate through all production rules for each non-terminal to ensure there are no segmenation faults and all rule ParseToken strings are terminated with PT_NULL and all ASTNodeType strings are terminated with AST_NULL
        for (size_t i = 0; i < grammar[t-ParseToken_FIRST_NONTERMINAL].num_rules; ++i)
        {
            const ProductionRule *rule = &grammar[t-ParseToken_FIRST_NONTERMINAL].rules[i];
            size_t j;
            for (j = 0; rule->tokens[j] != PT_NULL && rule->ast_types[j] != AST_NULL; ++j);
            assert(rule->tokens[j] == PT_NULL && rule->ast_types[j] == AST_NULL);
        }
        // printf("Now checking for direct left recursion for %d\n", t);
        // check for direct left recursion
        if (is_direct_left_recursive(grammar + (t - ParseToken_FIRST_NONTERMINAL)) != (size_t)-1)
        {
            printf("Grammar has direct left recursion for %d\n", t);
            // TODO: validate that the left recursive rule is in the correct format: A -> A B1 B2 ... | C1 C2 ... | D1 D2 ... | ... where non of C1, C2, D1, D2, ... start with A
        }
        // check for indirect left recursion
        if (is_indirect_left_recursive(grammar, ParseToken_COUNT_NONTERMINAL, t) != (size_t)-1)
        {
            printf("Grammar has indirect left recursion for %d\n", t);
        }
    }
    


    // Test with both valid and invalid inputs
    const char *input = "int x;\n"   // Valid declaration
                        "x = 42;\n"; // Valid assignment;
    // TODO 8: Add more test cases and read from a file:
    const char *invalid_input = "int x;\n"
                                "x = 42;\n"
                                "int ;";

    printf("Parsing input:\n%s\n", invalid_input);
    Array *tokens = array_new(10, sizeof(Token));
    // init lexer and stuff
    
    // Tokenize the input
    size_t token_index = 0;
    ParseTreeNode *root = parse_cfg_recursive_descent_parse_tree(grammar, ParseToken_COUNT_NONTERMINAL, ParseToken_START_NONTERMINAL, (Token *)array_begin(tokens), &token_index);
    
    printf("\n Parse Tree:\n");
    
    ParseTreeNode_print(root, 0);

    // ParseTreeNode_free();
    return 0;
}
