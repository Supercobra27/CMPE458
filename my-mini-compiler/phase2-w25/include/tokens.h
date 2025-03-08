//
// Modified by Hendrix Gryspeerdt, Simon John, Monica Saad, and Ryan Silverberg.
//

/* tokens.h */
#ifndef TOKENS_H
#define TOKENS_H

/* Token types that need to be recognized by the lexer */
typedef enum _TokenType
{
    TOKEN_IDENTIFIER,     
    TOKEN_ERROR,

    /* Values */
    TOKEN_INTEGER_CONST, 
    TOKEN_FLOAT_CONST,
    TOKEN_STRING_CONST, 

    /* Keywords */ 
    TOKEN_INT_KEYWORD,
    TOKEN_FLOAT_KEYWORD,
    TOKEN_STRING_KEYWORD,
    TOKEN_PRINT_KEYWORD,
    TOKEN_IF_KEYWORD,
    TOKEN_THEN_KEYWORD,
    TOKEN_ELSE_KEYWORD,
    TOKEN_WHILE_KEYWORD,
    TOKEN_REPEAT_KEYWORD,
    TOKEN_UNTIL_KEYWORD,
    TOKEN_FACTORIAL_KEYWORD,

    /* Punctuators */
    TOKEN_SEMICOLON,
    TOKEN_LEFT_BRACE,
    TOKEN_RIGHT_BRACE,
    TOKEN_LEFT_PAREN,
    TOKEN_RIGHT_PAREN,
    TOKEN_EOF,

    /* Operators */
    TOKEN_SINGLE_EQUALS,
    TOKEN_DOUBLE_PIPE,
    TOKEN_DOUBLE_AMPERSAND,
    TOKEN_SINGLE_PIPE,
    TOKEN_CARET,
    TOKEN_SINGLE_AMPERSAND,
    TOKEN_LESS_THAN_EQUALS,
    TOKEN_LESS_THAN,
    TOKEN_GREATER_THAN_EQUALS,
    TOKEN_GREATER_THAN,
    TOKEN_DOUBLE_EQUALS,
    TOKEN_BANG_EQUALS,
    TOKEN_DOUBLE_LESS_THAN,
    TOKEN_DOUBLE_GREATER_THAN,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_FORWARD_SLASH,
    TOKEN_PERCENT,
    TOKEN_TILDE,
    TOKEN_BANG,

} TokenType;

static const char *token_type_to_string(TokenType type)
{
    // need to change to work within ranges or its a huge case statement
    switch (type)
    {
    case TOKEN_IDENTIFIER:
        return "TOKEN_IDENTIFIER";
        break;
    case TOKEN_ERROR:
        return "TOKEN_ERROR";
        break;
    /* Values */
    case TOKEN_INTEGER_CONST:
        return "TOKEN_INTEGER_CONST";
        break;
    case TOKEN_FLOAT_CONST:
        return "TOKEN_FLOAT_CONST";
        break;
    case TOKEN_STRING_CONST:
        return "TOKEN_STRING_CONST";
        break;
    /* Keywords */
    case TOKEN_INT_KEYWORD:
        return "TOKEN_INT_KEYWORD";
        break;
    case TOKEN_FLOAT_KEYWORD:
        return "TOKEN_FLOAT_KEYWORD";
        break;
    case TOKEN_STRING_KEYWORD:
        return "TOKEN_STRING_KEYWORD";
        break;
    case TOKEN_PRINT_KEYWORD:
        return "TOKEN_PRINT_KEYWORD";
        break;
    case TOKEN_IF_KEYWORD:
        return "TOKEN_IF_KEYWORD";
        break;
    case TOKEN_THEN_KEYWORD:
        return "TOKEN_THEN_KEYWORD";
        break;
    case TOKEN_ELSE_KEYWORD:
        return "TOKEN_ELSE_KEYWORD";
        break;
    case TOKEN_WHILE_KEYWORD:
        return "TOKEN_WHILE_KEYWORD";
        break;
    case TOKEN_REPEAT_KEYWORD:
        return "TOKEN_REPEAT_KEYWORD";
        break;
    case TOKEN_UNTIL_KEYWORD:
        return "TOKEN_UNTIL_KEYWORD";
        break;
    case TOKEN_FACTORIAL_KEYWORD:
        return "TOKEN_FACTORIAL_KEYWORD";
        break;
    /* Punctuators */
    case TOKEN_SEMICOLON:
        return "TOKEN_SEMICOLON";
        break;
    case TOKEN_LEFT_BRACE:
        return "TOKEN_LEFT_BRACE";
        break;
    case TOKEN_RIGHT_BRACE:
        return "TOKEN_RIGHT_BRACE";
        break;
    case TOKEN_LEFT_PAREN:
        return "TOKEN_LEFT_PAREN";
        break;
    case TOKEN_RIGHT_PAREN:
        return "TOKEN_RIGHT_PAREN";
        break;
    case TOKEN_EOF:
        return "TOKEN_EOF";
        break;
    /* Operators */
    case TOKEN_SINGLE_EQUALS:
        return "TOKEN_SINGLE_EQUALS";
        break;
    case TOKEN_DOUBLE_PIPE:
        return "TOKEN_DOUBLE_PIPE";
        break;
    case TOKEN_DOUBLE_AMPERSAND:
        return "TOKEN_DOUBLE_AMPERSAND";
        break;
    case TOKEN_SINGLE_PIPE:
        return "TOKEN_SINGLE_PIPE";
        break;
    case TOKEN_CARET:
        return "TOKEN_CARET";
        break;
    case TOKEN_SINGLE_AMPERSAND:
        return "TOKEN_SINGLE_AMPERSAND";
        break;
    case TOKEN_LESS_THAN_EQUALS:
        return "TOKEN_LESS_THAN_EQUALS";
        break;
    case TOKEN_LESS_THAN:
        return "TOKEN_LESS_THAN";
        break;
    case TOKEN_GREATER_THAN_EQUALS:
        return "TOKEN_GREATER_THAN_EQUALS";
        break;
    case TOKEN_GREATER_THAN:
        return "TOKEN_GREATER_THAN";
        break;
    case TOKEN_DOUBLE_EQUALS:
        return "TOKEN_DOUBLE_EQUALS";
        break;
    case TOKEN_BANG_EQUALS:
        return "TOKEN_BANG_EQUALS";
        break;
    case TOKEN_DOUBLE_LESS_THAN:
        return "TOKEN_DOUBLE_LESS_THAN";
        break;
    case TOKEN_DOUBLE_GREATER_THAN:
        return "TOKEN_DOUBLE_GREATER_THAN";
        break;
    case TOKEN_PLUS:
        return "TOKEN_PLUS";
        break;
    case TOKEN_MINUS:
        return "TOKEN_MINUS";
        break;
    case TOKEN_STAR:
        return "TOKEN_STAR";
        break;
    case TOKEN_FORWARD_SLASH:
        return "TOKEN_FORWARD_SLASH";
        break;
    case TOKEN_PERCENT:
        return "TOKEN_PERCENT";
        break;
    case TOKEN_TILDE:
        return "TOKEN_TILDE";
        break;
    case TOKEN_BANG:
        return "TOKEN_BANG";
        break;
    default:
        return "UNKNOWN";
        break;
    }

}

/* Error types for lexical analysis */
typedef enum
{
    ERROR_NONE,
    ERROR_INVALID_CHAR,
    ERROR_INVALID_NUMBER,
    ERROR_UNTERMINATED_STRING,
    ERROR_STRING_TOO_LONG,
    ERROR_UNTERMINATED_COMMENT,
} ErrorType;

static const char *error_type_to_error_message(ErrorType error)
{
    switch (error)
    {
    case ERROR_NONE:
        return "No error";
    case ERROR_INVALID_CHAR:
        return "error: invalid character";
    case ERROR_INVALID_NUMBER:
        return "error: invalid number format";
    case ERROR_UNTERMINATED_STRING:
        return "error: unterminated string literal";
    case ERROR_STRING_TOO_LONG:
        return "error: string literal too long";
    case ERROR_UNTERMINATED_COMMENT:
        return "error: unterminated comment, missing matching '!?'";
    default:
        return "Unknown error";
    }
}

/* Details for positions of tokens in a file. */
typedef struct _LexemePosition
{
    int line;      // Line number of the token in the file.
    int col_start; // Column number of the first character of the token in the line.
    int col_end;   // Column number of the last character of the token in the line.
} LexemePosition;

/* Token structure to store token information */
typedef struct _Token
{
    TokenType type;
    char lexeme[100];        // Actual text of the token
    LexemePosition position; // Position of this token's lexeme in the input
    ErrorType error;         // Error type if the token is an TOKEN_ERROR.
} Token;

#endif /* TOKENS_H */

// To be added to the Token struct to support token subtypes.
// union
// {
//     ErrorType error;       // Error type if the token is an TOKEN_ERROR.
//     OperatorType operator; // Operator type if the token is an TOKEN_OPERATOR.
//     KeywordType keyword;   // Keyword type if the token is an TOKEN_KEYWORD.
//     NumberType number;     // Number type if the token is an TOKEN_NUMBER.

// } type2; // Secondary type information.

// May be added to the TokenType enum to support categories of operators.
// TOKEN_ASSIGNMENT_OPERATOR,
// TOKEN_ARITHMETIC_OPERATOR,
// TOKEN_LOGICAL_OPERATOR,
// TOKEN_BITWISE_OPERATOR,

/* from phase 2 template
typedef enum {
    TOKEN_EOF,
    TOKEN_NUMBER,      // e.g., "123", "456"
    TOKEN_OPERATOR,    // +, -, *, /
    TOKEN_IDENTIFIER,  // Variable names
    TOKEN_EQUALS,      // =
    TOKEN_SEMICOLON,   // ;
    TOKEN_LPAREN,      // (
    TOKEN_RPAREN,      // )
    TOKEN_LBRACE,      // {
    TOKEN_RBRACE,      // }
    TOKEN_IF,          // if keyword
    TOKEN_INT,         // int keyword
    TOKEN_PRINT,       // print keyword
    TOKEN_ERROR
} TokenType;

typedef enum {
    ERROR_NONE,
    ERROR_INVALID_CHAR,
    ERROR_INVALID_NUMBER,
    ERROR_CONSECUTIVE_OPERATORS,
    ERROR_INVALID_IDENTIFIER,
    ERROR_UNEXPECTED_TOKEN
} ErrorType;

typedef struct {
    TokenType type;
    char lexeme[100];   // Actual text of the token
    int line;           // Line number in source file
    ErrorType error;    // Error type if any
} Token;

*/