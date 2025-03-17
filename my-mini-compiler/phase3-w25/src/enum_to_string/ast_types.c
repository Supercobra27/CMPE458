#include "../../include/ast_types.h"


const char *ASTNodeType_to_string(const ASTNodeType t)
{
    switch(t)
    {
        case AST_NULL:
            return "AST_NULL";
        case AST_IDENTIFIER:
        // Terminal Nodes
            return "AST_IDENTIFIER";
        case AST_INTEGER:
            return "AST_INTEGER";
        case AST_FLOAT:
            return "AST_FLOAT";
        case AST_STRING:
            return "AST_STRING";
        case AST_INT_TYPE:
            return "AST_INT_TYPE";
        case AST_FLOAT_TYPE:    
            return "AST_FLOAT_TYPE";
        case AST_STRING_TYPE:
            return "AST_STRING_TYPE";
        // Special Node Types
        case AST_SKIP:
            return "AST_SKIP";
        case AST_FROM_CHILDREN:
            return "AST_FROM_CHILDREN";
        case AST_FROM_PROMOTION:
            return "AST_FROM_PROMOTION";
        // Non-terminal Nodes
        case AST_PROGRAM:
            return "AST_PROGRAM";
        case AST_SCOPE:
            return "AST_SCOPE";
        case AST_DECLARATION:
            return "AST_DECLARATION";
        case AST_PRINT:
            return "AST_PRINT";
        case AST_READ:
            return "AST_READ";
        case AST_CODITIONAL:
            return "AST_CODITIONAL";
        case AST_WHILE_LOOP:
            return "AST_WHILE_LOOP";
        case AST_REPEAT_UNTIL_LOOP:
            return "AST_REPEAT_UNTIL_LOOP";
        case AST_EXPRESSION:
            return "AST_EXPRESSION";
        case AST_ASSIGN_EQUAL:
            return "AST_ASSIGN_EQUAL";
        case AST_LOGICAL_OR:
            return "AST_LOGICAL_OR";
        case AST_LOGICAL_AND:
            return "AST_LOGICAL_AND";
        case AST_BITWISE_OR:
            return "AST_BITWISE_OR";
        case AST_BITWISE_XOR:
            return "AST_BITWISE_XOR";
        case AST_BITWISE_AND:
            return "AST_BITWISE_AND";
        case AST_COMPARE_EQUAL:
            return "AST_COMPARE_EQUAL";
        case AST_COMPARE_NOT_EQUAL:
            return "AST_COMPARE_NOT_EQUAL";
        case AST_COMPARE_LESS_EQUAL:
            return "AST_COMPARE_LESS_EQUAL";
        case AST_COMPARE_LESS_THAN:
            return "AST_COMPARE_LESS_THAN";
        case AST_COMPARE_GREATER_EQUAL:
            return "AST_COMPARE_GREATER_EQUAL";
        case AST_COMPARE_GREATER_THAN:
            return "AST_COMPARE_GREATER_THAN";
        case AST_SHIFT_LEFT:
            return "AST_SHIFT_LEFT";
        case AST_SHIFT_RIGHT:
            return "AST_SHIFT_RIGHT";
        case AST_ADD:
            return "AST_ADD";
        case AST_SUBTRACT:
            return "AST_SUBTRACT";
        case AST_MULTIPLY:
            return "AST_MULTIPLY";
        case AST_DIVIDE:
            return "AST_DIVIDE";
        case AST_MODULO:
            return "AST_MODULO";
        case AST_BITWISE_NOT:
            return "AST_BITWISE_NOT";
        case AST_LOGICAL_NOT:
            return "AST_LOGICAL_NOT";
        case AST_NEGATE:
            return "AST_NEGATE";
        case AST_FACTORIAL:
            return "AST_FACTORIAL";
    }
    return "UNKNOWN";
}

const char *ASTErrorType_to_string(const ASTErrorType t)
{
    switch(t)
    {
        case AST_ERROR_NONE:
            return "AST_ERROR_NONE";
        case AST_ERROR_MISSING_TOKEN:
            return "AST_ERROR_MISSING_TOKEN";
        case AST_ERROR_CHILD_ERROR:
            return "AST_ERROR_CHILD_ERROR";
        case AST_ERROR_UNSPECIFIED_PRODUCTION_RULE:
            return "AST_ERROR_MISSING_PRODUCTION_RULE";
        case AST_ERROR_EXPECTED_PROMOTION:
            return "AST_ERROR_EXPECTED_PROMOTION";
    }
    return "UNKNOWN";
}