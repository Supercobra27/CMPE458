#ifndef PARSE_TOKENS_H
#define PARSE_TOKENS_H

typedef enum _ParseToken {
    // Used by to indicate the end of a Null-terminated array of ParseToken.
    PT_NULL,
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
} ParseToken;

#define ParseToken_FIRST_TERMINAL PT_TOKEN_ERROR
#define ParseToken_START_NONTERMINAL PT_PROGRAM
#define ParseToken_FIRST_NONTERMINAL PT_PROGRAM
#define ParseToken_MAX PT_NEGATE
#define ParseToken_COUNT_TERMINAL (ParseToken_FIRST_NONTERMINAL - ParseToken_FIRST_TERMINAL)
#define ParseToken_COUNT_NONTERMINAL (ParseToken_MAX - ParseToken_FIRST_NONTERMINAL + 1)
#define ParseToken_IS_TERMINAL(token) (PT_NULL < (token) && (token) < ParseToken_FIRST_NONTERMINAL)
#define ParseToken_IS_NONTERMINAL(token) (ParseToken_FIRST_NONTERMINAL <= (token) && (token) <= ParseToken_MAX)
const char *ParseToken_to_string(const ParseToken t);

typedef enum _ParseErrorType {
    PARSE_ERROR_NONE,
    PARSE_ERROR_WRONG_TOKEN,
    PARSE_ERROR_NO_RULE_MATCHES,
    PARSE_ERROR_CHILD_ERROR,
    PARSE_ERROR_PREVIOUS_TOKEN_FAILED_TO_PARSE,
} ParseErrorType;

const char *ParseErrorType_to_string(const ParseErrorType error);

#endif /* PARSE_TOKENS_H */