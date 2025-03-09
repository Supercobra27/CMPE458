#ifndef PARSE_TOKENS_H
#define PARSE_TOKENS_H
#include "tokens.h"
#include <stddef.h>

typedef enum _ParseToken {
    // Terminal tokens (these enum values must directly correspond to the token values in tokens.h)
    PT_TOKEN_ERROR,
    PT_IDENTIFIER,

    /* Values */
    PT_INTEGER_CONST,
    PT_FLOAT_CONST,
    PT_STRING_CONST,
    
    /* Keywords */
    PT_INT_KEYWORD,
    PT_FLOAT_KEYWORD,
    PT_STRING_KEYWORD,
    PT_PRINT_KEYWORD,
    PT_READ_KEYWORD,
    PT_IF_KEYWORD,
    PT_THEN_KEYWORD,
    PT_ELSE_KEYWORD,
    PT_WHILE_KEYWORD,
    PT_REPEAT_KEYWORD,
    PT_UNTIL_KEYWORD,
    PT_FACTORIAL_KEYWORD,

    /* Punctuators */
    PT_SEMICOLON,
    PT_LEFT_BRACE,
    PT_RIGHT_BRACE,
    PT_LEFT_PAREN,
    PT_RIGHT_PAREN,
    PT_EOF,

    /* Terminal Operators */
    PT_EQUAL_EQUAL, // "=="
    PT_BANG_EQUAL, // "!="
    PT_GREATER_THAN_EQUAL, // ">="
    PT_LESS_THAN_EQUAL, // "<="
    PT_LESS_THAN_LESS_THAN, // "<<"
    PT_GREATER_THAN_GREATER_THAN, // ">>"
    PT_AMPERSAND_AMPERSAND, // "&&"
    PT_PIPE_PIPE, // "||"
    PT_EQUAL, // "="
    PT_PLUS, // "+"
    PT_MINUS, // "-"
    PT_STAR, // "*"
    PT_FORWARD_SLASH, // "/"
    PT_PERCENT, // "%"
    PT_TILDE, // "~"
    PT_AMPERSAND, // "&"
    PT_PIPE, // "|"
    PT_CARET, // "^"
    PT_BANG, // "!"
    PT_LESS_THAN, // "<"
    PT_GREATER_THAN, // ">"

    
    // Non-terminal tokens
    PT_PROGRAM,
    PT_SCOPE,
    PT_STATEMENT_LIST,
    PT_STATEMENT,
    PT_EMPTY_STATEMENT,
    PT_DECLARATION,
    PT_EXPRESSION_STATEMENT,
    PT_EXPRESSION_EVAL,
    PT_PRINT_STATEMENT,
    PT_READ_STATEMENT,
    PT_BLOCK,
    PT_CONDITIONAL,
    PT_WHILE_LOOP,
    PT_REPEAT_UNTIL_LOOP,

    PT_OPTIONAL_ELSE_BLOCK,
    PT_STATEMENT_END,
    PT_TYPE_KEYWORD, // not in AST
    PT_EXPRESSION,
    PT_BLOCK_BEGIN,
    PT_BLOCK_END,

    /* Expressions */ // These also don't directly appear in the AST, they are used for grouping expressions with the same precedence levels.
    PT_ASSIGNMENTEX_R12,
    PT_ASSIGNMENT_REST,
    PT_OREX_L11,
    PT_ANDEX_L10,
    PT_BITOREX_L9,
    PT_BITXOREX_L8,
    PT_BITANDEX_L7,
    PT_RELATIONEX_L6,
    PT_SHIFTEX_L5,
    PT_SUMEX_L4,
    PT_PRODUCTEX_L3,
    PT_UNARYPREFIXEX_R2,
    PT_FACTOR,
    PT_FACTORIAL_CALL,


    // Operator tokens (non-terminals) (these won't appear in the AST, they are used for grouping operators with the same precedence levels)
    PT_ASSIGNMENT_OPERATOR,
    PT_OR_OPERATOR,
    PT_AND_OPERATOR,
    PT_BITOR_OPERATOR,
    PT_BITXOR_OPERATOR,
    PT_BITAND_OPERATOR,
    PT_RELATIONAL_OPERATOR,
    PT_SHIFT_OPERATOR,
    PT_SUM_OPERATOR,
    PT_PRODUCT_OPERATOR,
    PT_UNARY_PREFIX_OPERATOR,

    // Actual operation nodes (once promoted) that will be present in the AST.
    PT_ASSIGN_EQUAL,
    PT_LOGICAL_OR,
    PT_LOGICAL_AND,
    PT_BITWISE_OR,
    PT_BITWISE_XOR,
    PT_BITWISE_AND,
    PT_COMPARE_EQUAL,
    PT_COMPARE_NOT_EQUAL,
    PT_COMPARE_LESS_EQUAL,
    PT_COMPARE_LESS,
    PT_COMPARE_GREATER_EQUAL,
    PT_COMPARE_GREATER,
    PT_SHIFT_LEFT,
    PT_SHIFT_RIGHT,
    PT_ADD,
    PT_SUBTRACT,
    PT_MULTIPLY,
    PT_DIVIDE,
    PT_MODULO,
    PT_BITWISE_NOT,
    PT_LOGICAL_NOT,
    PT_NEGATE,

    // Used by to indicate the end of a Null-terminated array of ParseToken.
    PT_NULL,
} ParseToken;

#define ParseToken_FIRST_TERMINAL PT_IDENTIFIER
#define ParseToken_START_NONTERMINAL PT_PROGRAM
#define ParseToken_MAX PT_NULL
#define ParseToken_FIRST_NONTERMINAL ParseToken_START_NONTERMINAL
#define ParseToken_COUNT_TERMINAL FIRST_NONTERMINAL_ParseToken
#define ParseToken_COUNT_NONTERMINAL (ParseToken_MAX - ParseToken_FIRST_NONTERMINAL)
#define ParseToken_IS_TERMINAL(token) ((token) < ParseToken_FIRST_NONTERMINAL)
#define ParseToken_IS_NONTERMINAL(token) ((token) >= ParseToken_FIRST_NONTERMINAL && (token) < ParseToken_MAX)

static const char *parse_token_to_string(ParseToken t)
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
    case PT_EXPRESSION_EVAL:
        return "PT_EXPRESSION_EVAL";
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

typedef enum _ParseErrorType {
    PARSE_ERROR_NONE,
    PARSE_ERROR_WRONG_TOKEN,
    PARSE_ERROR_NO_RULE_MATCHES,
    PARSE_ERROR_CHILD_ERROR,
    PARSE_ERROR_PREVIOUS_TOKEN_FAILED_TO_PARSE,
    PARSE_ERROR_MULTIPLE_LEFT_RECURSIVE_RULES, // not really a parsing error, more of an error about the structure of the grammar.
} ParseErrorType;

static const char *parse_error_type_to_string(ParseErrorType error)
{
    switch (error)
    {
    case PARSE_ERROR_NONE:
        return "PARSE_ERROR_NONE";
    case PARSE_ERROR_WRONG_TOKEN:
        return "PARSE_ERROR_WRONG_TOKEN";
    case PARSE_ERROR_NO_RULE_MATCHES:
        return "PARSE_ERROR_NO_RULE_MATCHES";
    case PARSE_ERROR_MULTIPLE_LEFT_RECURSIVE_RULES:
        return "PARSE_ERROR_MULTIPLE_LEFT_RECURSIVE_RULES";
    case PARSE_ERROR_CHILD_ERROR:
        return "PARSE_ERROR_CHILD_ERROR";
    case PARSE_ERROR_PREVIOUS_TOKEN_FAILED_TO_PARSE:
        return "PARSE_ERROR_PREVIOUS_TOKEN_FAILED_TO_PARSE";
    }   
    return "UNKNOWN";
}

typedef struct _ParseTreeNode {
    ParseToken type;
    size_t num_children;
    struct _ParseTreeNode *children; // Array of `num_children` children.
    const Token *token; // Token in the case of a terminal node.
    ParseErrorType error;
} ParseTreeNode;
    

#endif /* PARSE_TOKENS_H */