#ifndef PARSE_TOKENS_H
#define PARSE_TOKENS_H
#include "tokens.h"

typedef enum _ParseToken {
    // Terminal tokens (these enum values must directly correspond to the token values in tokens.h)
    PT_EOF,
    PT_IDENTIFIER,
    PT_ERROR, // placeholder for consistency

    /* Values */
    PT_INTEGER_CONST,
    PT_FLOAT_CONST,
    PT_STRING_CONST,

    /* Keywords */
    PT_INT_KEYWORD,
    PT_FLOAT_KEYWORD,
    PT_STRING_KEYWORD,
    PT_PRINT_KEYWORD,
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

    /* Terminal Operators */
    PT_SINGLE_EQUALS,
    PT_DOUBLE_PIPE,
    PT_DOUBLE_AMPERSAND,
    PT_SINGLE_PIPE,
    PT_CARET,
    PT_SINGLE_AMPERSAND,
    PT_LESS_THAN_EQUALS,
    PT_LESS_THAN,
    PT_GREATER_THAN_EQUALS,
    PT_GREATER_THAN,
    PT_DOUBLE_EQUALS,
    PT_BANG_EQUALS,
    PT_DOUBLE_LESS_THAN,
    PT_DOUBLE_GREATER_THAN,
    PT_PLUS,
    PT_MINUS,
    PT_STAR,
    PT_FORWARD_SLASH,
    PT_PERCENT,
    PT_TILDE,
    PT_BANG,

    
    // Non-terminal tokens
    PT_PROGRAM,
    PT_STATEMENT_LIST,
    PT_STATEMENT,
    PT_EMPTY_STATEMENT,
    PT_DECLARATION,
    PT_EXPRESSION_STATEMENT,
    PT_PRINT_STATEMENT,
    PT_BLOCK,
    PT_CONDITIONAL,
    PT_WHILE_LOOP,
    PT_REPEAT_UNTIL_LOOP,

    PT_OPTIONAL_ELSE_BLOCK,
    PT_STATEMENT_END,
    PT_TYPE_KEYWORD,
    PT_EXPRESSION,
    PT_BLOCK_BEGIN,
    PT_BLOCK_END,

    /* Expressions */
    PT_ASSIGNMENTEX_R12,
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

    PT_ASSIGN_EQUAL,
    PT_LOGICAL_OR,
    PT_LOGICAL_AND,
    PT_BITWISE_OR,
    PT_BITWISE_XOR,
    PT_BITWISE_AND,
    PT_COMPARE_EQUAL,
    PT_COMPARE_NOT_EQUAL,
    PT_COMPARE_LESS,
    PT_COMPARE_LESS_EQUAL,
    PT_COMPARE_GREATER,
    PT_COMPARE_GREATER_EQUAL,
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

    PT_IMMUTABLE,
    PT_MUTABLE,

    PT_FACTORIAL_CALL,
    PT_CONSTANT,
    // Used by to indicate the end of a Null-terminated array of ParseToken.
    PT_NULL,
} ParseToken;

#define FIRST_TERMINAL_ParseToken PT_EOF
#define FIRST_NONTERMINAL_ParseToken PT_PROGRAM
#define COUNT_TERMINAL_ParseToken FIRST_NONTERMINAL_ParseToken
#define COUNT_NONTERMINAL_ParseToken (PT_NULL - FIRST_NONTERMINAL_ParseToken)


typedef enum _ParseErrorType {
    PARSE_ERROR_WRONG_TOKEN,
} ParseErrorType;

typedef struct _ParseTreeNode {
    ParseToken type;
    ParseTreeNode *children; // Null-terminated array of children.
    Token *token; // Token in the case of a terminal node.
    ParseErrorType error;
} ParseTreeNode;
    

#endif /* PARSE_TOKENS_H */