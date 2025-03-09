#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../include/dynamic_array.h"
#include "../include/grammar.h"
#include "../include/lexer.h"
#include "../include/parser.h"

#define FILE_EXT ".cisc"

void ParseTreeNode_print_head(ParseTreeNode* node) {
    printf("%s", parse_token_to_string(node->type));
    if (node->error != PARSE_ERROR_NONE) 
        printf(" (%s)", parse_error_type_to_string(node->error));
    if (node->token != NULL)
    {
        if (node->token->type == TOKEN_ERROR || node->error != PARSE_ERROR_NONE)
        {
            printf(" -> ");
            print_token(*node->token);
        }
        else
            printf(" -> %s \"%s\"", token_type_to_string(node->token->type), node->token->lexeme);
            
    }
    printf("\n");
}

// Main function for testing
int main(int argc, char *argv[])
{
    // setbuf(stdout, NULL);  // Disable buffering
    // not sure if there is any other way to statically initialize this so that it's not right in the middle of the main function.
    const CFG_GrammarRule grammar[ParseToken_COUNT_NONTERMINAL] = {
        (CFG_GrammarRule){
            .lhs = PT_PROGRAM,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_SCOPE, PT_EOF, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_SCOPE, AST_IGNORE, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_SCOPE,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_STATEMENT_LIST, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_CHILDREN, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_STATEMENT_LIST,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_STATEMENT, PT_STATEMENT_LIST, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_FROM_CHILDREN, AST_NULL},
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
                    .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_NULL},
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
                    .tokens = (ParseToken[]){PT_BLOCK_BEGIN, PT_SCOPE, PT_BLOCK_END, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_FROM_CHILDREN, AST_IGNORE, AST_NULL},
                    .promote_index = 1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_CONDITIONAL,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_IF_KEYWORD, PT_EXPRESSION_EVAL, PT_THEN_KEYWORD, PT_BLOCK, PT_OPTIONAL_ELSE_BLOCK, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_EXPRESSION, AST_IGNORE, AST_SCOPE, AST_SCOPE, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_WHILE_LOOP,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_WHILE_KEYWORD, PT_EXPRESSION_EVAL, PT_BLOCK, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_EXPRESSION, AST_SCOPE, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_REPEAT_UNTIL_LOOP,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_REPEAT_KEYWORD, PT_BLOCK, PT_UNTIL_KEYWORD, PT_EXPRESSION_EVAL, PT_STATEMENT_END, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_SCOPE, AST_IGNORE, AST_EXPRESSION, AST_IGNORE, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_OPTIONAL_ELSE_BLOCK,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_ELSE_KEYWORD, PT_BLOCK, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_SCOPE, AST_NULL},
                    .promote_index = 1},
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
            
        // TODO: PT_ASSIGNMENTEX_R12 was not deterministic (prefix-free), this fixes it. However, this fix does not work with the single promote_index, because the promote index would need to be different for each case as can be seen below.
        (CFG_GrammarRule){
            .lhs = PT_ASSIGNMENTEX_R12,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_OREX_L11, PT_ASSIGNMENT_REST, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_FROM_PROMOTION, AST_NULL},
                    .promote_index = 1/* want promote_index = 0 if PT_ASSIGNMENT_REST turns out to be AST_NULL*/,
                    /*.promotion_alternate_if_AST_NULL = {0, 0}*/},
                /*(ProductionRule){
                    .tokens = (ParseToken[]){PT_OREX_L11, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_NULL},
                    .promote_index = 0}*/},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_ASSIGNMENT_REST,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_ASSIGNMENT_OPERATOR, PT_ASSIGNMENTEX_R12, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_FROM_PROMOTION, AST_NULL},
                    .promote_index = 0},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_NULL},
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
                    .tokens = (ParseToken[]){PT_EQUAL, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_LOGICAL_OR,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_PIPE_PIPE, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_LOGICAL_AND,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_AMPERSAND_AMPERSAND, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_BITWISE_OR,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_PIPE, PT_NULL},
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
                    .tokens = (ParseToken[]){PT_AMPERSAND, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_COMPARE_EQUAL,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_EQUAL_EQUAL, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_COMPARE_NOT_EQUAL,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_BANG_EQUAL, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_COMPARE_LESS_EQUAL,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_LESS_THAN_EQUAL, PT_NULL},
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
                    .tokens = (ParseToken[]){PT_GREATER_THAN_EQUAL, PT_NULL},
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
                    .tokens = (ParseToken[]){PT_LESS_THAN_LESS_THAN, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_IGNORE, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_SHIFT_RIGHT,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_GREATER_THAN_GREATER_THAN, PT_NULL},
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
    bool grammar_is_deterministic = true;
    bool grammar_contains_direct_left_recursion = false;
    bool grammar_contains_invalid_direct_left_recursive_rule = false;
    bool grammar_contains_indirect_left_recursion = false;
    for (ParseToken t = ParseToken_FIRST_NONTERMINAL; ParseToken_IS_NONTERMINAL(t); ++t)
    {
        // ensure the grammar has a rule for each non-terminal in the correct index
        if (grammar[t-ParseToken_FIRST_NONTERMINAL].lhs != t)
        {
            printf("Grammar rule for %d is missing (index %d)\n", t, t-ParseToken_FIRST_NONTERMINAL);
            return 1;
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
        // Check that each grammar rule is deterministic
        for (size_t i = 0; i < grammar[t-ParseToken_FIRST_NONTERMINAL].num_rules; ++i)
        {
            for (size_t j = 0; j < grammar[t-ParseToken_FIRST_NONTERMINAL].num_rules; ++j)
            {
                if (i != j)
                {
                    const ProductionRule *rule1 = &grammar[t-ParseToken_FIRST_NONTERMINAL].rules[i];
                    const ProductionRule *rule2 = &grammar[t-ParseToken_FIRST_NONTERMINAL].rules[j];
                    if (rule1->tokens[0] == rule2->tokens[0])
                    {
                        printf("Grammar rule for %d is not deterministic\n", t);
                        grammar_is_deterministic = false;
                    }
                }
            }
        }

        // printf("Now checking for direct left recursion for %d\n", t);
        // check for direct left recursion
        if (is_direct_left_recursive(grammar + (t - ParseToken_FIRST_NONTERMINAL)) != (size_t)-1)
        {
            printf("Grammar has direct left recursion for %d\n", t);
            grammar_contains_direct_left_recursion = true;
            // validate that the left recursive rule is in the correct format: A -> A B1 B2 ... | C1 C2 ... | D1 D2 ... | ... where non of C1, D1, ... start with A
            // check that the first rule is the left recursive rule.
            if (grammar[t-ParseToken_FIRST_NONTERMINAL].rules[0].tokens[0] != t)
            {
                grammar_contains_invalid_direct_left_recursive_rule = true;
                printf("Grammar has invalid direct left recursive rule for %d. First production must be left recursive.\n", t);
            }
            // check that none of the other rules are left recursive.
            for (size_t i = 1; i < grammar[t-ParseToken_FIRST_NONTERMINAL].num_rules; ++i)
            {
                if (grammar[t-ParseToken_FIRST_NONTERMINAL].rules[i].tokens[0] == t)
                {
                    grammar_contains_invalid_direct_left_recursive_rule = true;
                    printf("Grammar has invalid direct left recursive rule for %d. Rule %llu should not also be left recursive.\n", t, i);
                }
            }
        }
        // check for indirect left recursion
        if (is_indirect_left_recursive(grammar, ParseToken_COUNT_NONTERMINAL, t) != (size_t)-1)
        {
            printf("Grammar has indirect left recursion for %d\n", t);
            grammar_contains_indirect_left_recursion = true;
        }
    }

    printf("grammar_is_deterministic: %s\n" , grammar_is_deterministic ? "true" : "false");
    printf("grammar_contains_direct_left_recursion: %s\n" , grammar_contains_direct_left_recursion ? "true" : "false");
    printf("grammar_contains_invalid_direct_left_recursive_rule: %s\n", grammar_contains_invalid_direct_left_recursive_rule ? "true" : "false");
    printf("grammar_contains_indirect_left_recursion: %s\n" , grammar_contains_indirect_left_recursion ? "true" : "false");
    if (!grammar_is_deterministic || grammar_contains_invalid_direct_left_recursive_rule || grammar_contains_indirect_left_recursion)
    {
        printf("Grammar is invalid, cannot be parsed correctly\n");
        return 1;
    }
    printf("Grammar is valid\n");
    
    
    // input
    char *input = NULL;
    bool must_free_input = false;
    if (argc == 2)
    {
        // Input file extension check
        int file_len = strlen(argv[1]);
        char *file_name = argv[1];
        if (file_len < 5 || strncmp(file_name + file_len - 5, FILE_EXT, 5) != 0)
        {
            printf("Incorrect file extension, the correct extension is .cisc");
            return -1;
        }
        // Load file into input.
        FILE *file = fopen(file_name, "r");
        if (file == NULL)
        {
            printf("Error: Unable to open file %s\n", file_name);
            return -1;
        }
        fseek(file, 0, SEEK_END);
        size_t file_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        input = malloc(file_size + 1);
        if (input == NULL)
        {
            printf("Error: Unable to allocate memory for file %s\n", file_name);
            return -1;
        }
        must_free_input = true;

        size_t bytes_read = 0;
        char ch;
        while (bytes_read < file_size && (ch = fgetc(file)) != EOF && ch != '\0')
            input[bytes_read++] = ch;
        input[bytes_read] = '\0';
        fclose(file);
    }
    else
    {
        // Print usage for file input.
        printf("Usage: .\\my-mini-compiler.exe <Input File Name>.cisc\n");
        // Test with both valid and invalid inputs
        // input = "int x;\n"   // Valid declaration
        //         "x = 42;\n"; // Valid assignment;
        // input = "int x;\n"
        //         "x = 42;\n"
        //         "int ;";

        input = "1 + 2 + 3;\n";
    }

    // TODO: Add more test cases
    printf("Processing input:\n%s\n", input);

    Array *tokens = array_new(8, sizeof(Token));
    // Tokenize the input
    printf("Tokenizing:\n");
    init_lexer(input, 0);
    Token token = (Token){
        .error = ERROR_NONE, 
        .type = TOKEN_EOF, 
        .lexeme = "", 
        .position = (LexemePosition){.line = 0, .col_end = 0, .col_start = 0},};
    do
    {
        token = get_next_token();
        array_push(tokens, (Element *)&token);
        print_token(token);
        printf("\n");
    } while (token.type != TOKEN_EOF);

    

    // Parse the input
    printf("Parse Tree:\n");
    size_t token_index = 0;
    ParseTreeNode *root = parse_cfg_recursive_descent_parse_tree(grammar, ParseToken_COUNT_NONTERMINAL, ParseToken_START_NONTERMINAL, (Token *)array_begin(tokens), &token_index);

    
    ParseTreeRoot_print(root, ParseTreeNode_print_head);
    // ParseTreeNode_print_simple(root, 0, ParseTreeNode_print_head);

    ParseTreeNode_free(root, 1);
    array_free(tokens);
    if (must_free_input)
        free(input);
    return 0;
}