#include "../../include/parse_tokens.h"


const char *ParseToken_to_string(const ParseToken t)
{
    switch (t)
    {
    case PT_TOKEN_ERROR:
        return "PT_TOKEN_ERROR";
    case PT_IDENTIFIER:
        return "PT_IDENTIFIER";
    case PT_INTEGER_CONST:
        return "PT_INTEGER_CONST";
    case PT_FLOAT_CONST:
        return "PT_FLOAT_CONST";
    case PT_STRING_CONST:
        return "PT_STRING_CONST";
    case PT_INT_KEYWORD:    
        return "PT_INT_KEYWORD";
    case PT_FLOAT_KEYWORD:
        return "PT_FLOAT_KEYWORD";
    case PT_STRING_KEYWORD:
        return "PT_STRING_KEYWORD";
    case PT_SEMICOLON:
        return "PT_SEMICOLON";
    case PT_LEFT_BRACE:
        return "PT_LEFT_BRACE";
    case PT_RIGHT_BRACE:
        return "PT_RIGHT_BRACE";
    case PT_PRINT_KEYWORD:
        return "PT_PRINT_KEYWORD";
    case PT_READ_KEYWORD:
        return "PT_READ_KEYWORD";
    case PT_IF_KEYWORD:
        return "PT_IF_KEYWORD";
    case PT_THEN_KEYWORD:
        return "PT_THEN_KEYWORD";
    case PT_ELSE_KEYWORD:
        return "PT_ELSE_KEYWORD";
    case PT_WHILE_KEYWORD:
        return "PT_WHILE_KEYWORD";
    case PT_REPEAT_KEYWORD:
        return "PT_REPEAT_KEYWORD";
    case PT_UNTIL_KEYWORD:
        return "PT_UNTIL_KEYWORD";
    case PT_FACTORIAL_KEYWORD:
        return "PT_FACTORIAL_KEYWORD";
    case PT_LEFT_PAREN:
        return "PT_LEFT_PAREN";
    case PT_RIGHT_PAREN:
        return "PT_RIGHT_PAREN";
    case PT_EOF:
        return "PT_EOF";
    case PT_EQUAL:
        return "PT_EQUAL";
    case PT_PIPE_PIPE:
        return "PT_PIPE_PIPE";
    case PT_AMPERSAND_AMPERSAND:
        return "PT_AMPERSAND_AMPERSAND";
    case PT_PIPE:
        return "PT_PIPE";
    case PT_CARET:
        return "PT_CARET";
    case PT_AMPERSAND:
        return "PT_AMPERSAND";
    case PT_LESS_THAN_EQUAL:    
        return "PT_LESS_THAN_EQUAL";
    case PT_LESS_THAN:
        return "PT_LESS_THAN";
    case PT_GREATER_THAN_EQUAL:
        return "PT_GREATER_THAN_EQUAL";
    case PT_GREATER_THAN:
        return "PT_GREATER_THAN";
    case PT_EQUAL_EQUAL:
        return "PT_EQUAL_EQUAL";
    case PT_BANG_EQUAL:
        return "PT_BANG_EQUAL";
    case PT_LESS_THAN_LESS_THAN:
        return "PT_LESS_THAN_LESS_THAN";
    case PT_GREATER_THAN_GREATER_THAN:
        return "PT_GREATER_THAN_GREATER_THAN";
    case PT_PLUS:
        return "PT_PLUS";
    case PT_MINUS:
        return "PT_MINUS";
    case PT_STAR:
        return "PT_STAR";
    case PT_FORWARD_SLASH:
        return "PT_FORWARD_SLASH";
    case PT_PERCENT:
        return "PT_PERCENT";
    case PT_TILDE:
        return "PT_TILDE";
    case PT_BANG:
        return "PT_BANG";
    // Non-terminal tokens
    case PT_PROGRAM:
        return "PT_PROGRAM";
    case PT_SCOPE:
        return "PT_SCOPE";
    case PT_STATEMENT_LIST:
        return "PT_STATEMENT_LIST";
    case PT_STATEMENT:
        return "PT_STATEMENT";
    case PT_EMPTY_STATEMENT:
        return "PT_EMPTY_STATEMENT";
    case PT_DECLARATION:
        return "PT_DECLARATION";
    case PT_EXPRESSION_STATEMENT:
        return "PT_EXPRESSION_STATEMENT";
    case PT_PRINT_STATEMENT:
        return "PT_PRINT_STATEMENT";
    case PT_READ_STATEMENT:
        return "PT_READ_STATEMENT";
    case PT_BLOCK:
        return "PT_BLOCK";
    case PT_CONDITIONAL:
        return "PT_CONDITIONAL";
    case PT_WHILE_LOOP:
        return "PT_WHILE_LOOP";
    case PT_REPEAT_UNTIL_LOOP:
        return "PT_REPEAT_UNTIL_LOOP";
    case PT_OPTIONAL_ELSE_BLOCK:
        return "PT_OPTIONAL_ELSE_BLOCK";
    case PT_STATEMENT_END:
        return "PT_STATEMENT_END";
    case PT_TYPE_KEYWORD:
        return "PT_TYPE_KEYWORD";
    case PT_EXPRESSION:
        return "PT_EXPRESSION";
    case PT_BLOCK_BEGIN:
        return "PT_BLOCK_BEGIN";
    case PT_BLOCK_END:
        return "PT_BLOCK_END";
    case PT_ASSIGNMENTEX_R12:
        return "PT_ASSIGNMENTEX_R12";
    case PT_ASSIGNMENT_REST:
        return "PT_ASSIGNMENT_REST";
    case PT_OREX_L11:
        return "PT_OREX_L11";
    case PT_ANDEX_L10:
        return "PT_ANDEX_L10";
    case PT_BITOREX_L9:
        return "PT_BITOREX_L9";
    case PT_BITXOREX_L8:
        return "PT_BITXOREX_L8";
    case PT_BITANDEX_L7:
        return "PT_BITANDEX_L7";
    case PT_RELATIONEX_L6:
        return "PT_RELATIONEX_L6";
    case PT_SHIFTEX_L5:
        return "PT_SHIFTEX_L5";
    case PT_SUMEX_L4:
        return "PT_SUMEX_L4";
    case PT_PRODUCTEX_L3:
        return "PT_PRODUCTEX_L3";
    case PT_UNARYPREFIXEX_R2:
        return "PT_UNARYPREFIXEX_R2";
    case PT_FACTOR:
        return "PT_FACTOR";
    case PT_ASSIGNMENT_OPERATOR:
        return "PT_ASSIGNMENT_OPERATOR";
    case PT_OR_OPERATOR:
        return "PT_OR_OPERATOR";
    case PT_AND_OPERATOR:
        return "PT_AND_OPERATOR";
    case PT_BITOR_OPERATOR:
        return "PT_BITOR_OPERATOR";
    case PT_BITXOR_OPERATOR:
        return "PT_BITXOR_OPERATOR";
    case PT_BITAND_OPERATOR:
        return "PT_BITAND_OPERATOR";
    case PT_RELATIONAL_OPERATOR:
        return "PT_RELATIONAL_OPERATOR";
    case PT_SHIFT_OPERATOR:
        return "PT_SHIFT_OPERATOR";
    case PT_SUM_OPERATOR:
        return "PT_SUM_OPERATOR";
    case PT_PRODUCT_OPERATOR:
        return "PT_PRODUCT_OPERATOR";
    case PT_UNARY_PREFIX_OPERATOR:
        return "PT_UNARY_PREFIX_OPERATOR";
    case PT_ASSIGN_EQUAL:
        return "PT_ASSIGN_EQUAL";
    case PT_LOGICAL_OR:
        return "PT_LOGICAL_OR";
    case PT_LOGICAL_AND:
        return "PT_LOGICAL_AND";
    case PT_BITWISE_OR:
        return "PT_BITWISE_OR";
    case PT_BITWISE_XOR:
        return "PT_BITWISE_XOR";
    case PT_BITWISE_AND:
        return "PT_BITWISE_AND";
    case PT_COMPARE_EQUAL:
        return "PT_COMPARE_EQUAL";
    case PT_COMPARE_NOT_EQUAL:
        return "PT_COMPARE_NOT_EQUAL";
    case PT_COMPARE_LESS:
        return "PT_COMPARE_LESS";
    case PT_COMPARE_LESS_EQUAL:
        return "PT_COMPARE_LESS_EQUAL";
    case PT_COMPARE_GREATER:
        return "PT_COMPARE_GREATER";
    case PT_COMPARE_GREATER_EQUAL:
        return "PT_COMPARE_GREATER_EQUAL";
    case PT_SHIFT_LEFT:
        return "PT_SHIFT_LEFT";
    case PT_SHIFT_RIGHT:
        return "PT_SHIFT_RIGHT";
    case PT_ADD:
        return "PT_ADD";
    case PT_SUBTRACT:
        return "PT_SUBTRACT";
    case PT_MULTIPLY:
        return "PT_MULTIPLY";
    case PT_DIVIDE:
        return "PT_DIVIDE";
    case PT_MODULO:
        return "PT_MODULO";
    case PT_BITWISE_NOT:
        return "PT_BITWISE_NOT";
    case PT_LOGICAL_NOT:
        return "PT_LOGICAL_NOT";
    case PT_NEGATE:
        return "PT_NEGATE";
    case PT_FACTORIAL_CALL:
        return "PT_FACTORIAL_CALL";
    case PT_NULL:
        return "PT_NULL";
    }
    return "UNKNOWN";
}

const char *ParseErrorType_to_string(const ParseErrorType error)
{
    switch (error)
    {
    case PARSE_ERROR_NONE:
        return "PARSE_ERROR_NONE";
    case PARSE_ERROR_WRONG_TOKEN:
        return "PARSE_ERROR_WRONG_TOKEN";
    case PARSE_ERROR_NO_RULE_MATCHES:
        return "PARSE_ERROR_NO_RULE_MATCHES";
    case PARSE_ERROR_CHILD_ERROR:
        return "PARSE_ERROR_CHILD_ERROR";
    case PARSE_ERROR_PREVIOUS_TOKEN_FAILED_TO_PARSE:
        return "PARSE_ERROR_PREVIOUS_TOKEN_FAILED_TO_PARSE";
    }   
    return "UNKNOWN";
}