//
// Modified by Hendrix Gryspeerdt, Simon John, Monica Saad, and Ryan Silverberg.
//

/* tokens.h */
#ifndef TOKENS_H
#define TOKENS_H

/* Token types that need to be recognized by the lexer */
typedef enum _TokenType
{
    TOKEN_ERROR,
    TOKEN_IDENTIFIER,

    /* Values */
    TOKEN_INTEGER_CONST, 
    TOKEN_FLOAT_CONST,
    TOKEN_STRING_CONST, 

    /* Keywords */ 
    TOKEN_INT_KEYWORD, // "int"
    TOKEN_FLOAT_KEYWORD, // "float"
    TOKEN_STRING_KEYWORD, // "string"
    TOKEN_PRINT_KEYWORD, // "print"
    TOKEN_READ_KEYWORD, // "read"
    TOKEN_IF_KEYWORD, // "if"
    TOKEN_THEN_KEYWORD, // "then"
    TOKEN_ELSE_KEYWORD, // "else"
    TOKEN_WHILE_KEYWORD, // "while"
    TOKEN_REPEAT_KEYWORD, // "repeat"
    TOKEN_UNTIL_KEYWORD, // "until"
    TOKEN_FACTORIAL_KEYWORD, // "factorial"

    /* Punctuators */
    TOKEN_SEMICOLON, // ";"
    TOKEN_LEFT_BRACE, // "{"
    TOKEN_RIGHT_BRACE, // "}"
    TOKEN_LEFT_PAREN, // "("
    TOKEN_RIGHT_PAREN,  // ")"
    TOKEN_EOF, // End of input "\0"

    /* Operators */
    TOKEN_EQUAL_EQUAL, // "=="
    TOKEN_BANG_EQUAL, // "!="
    TOKEN_GREATER_THAN_EQUAL, // ">="
    TOKEN_LESS_THAN_EQUAL, // "<="
    TOKEN_LESS_THAN_LESS_THAN, // "<<"
    TOKEN_GREATER_THAN_GREATER_THAN, // ">>"
    TOKEN_AMPERSAND_AMPERSAND, // "&&"
    TOKEN_PIPE_PIPE, // "||"
    TOKEN_EQUAL, // "="
    TOKEN_PLUS, // "+"
    TOKEN_MINUS, // "-"
    TOKEN_STAR, // "*"
    TOKEN_FORWARD_SLASH, // "/"
    TOKEN_PERCENT, // "%"
    TOKEN_TILDE, // "~"
    TOKEN_AMPERSAND, // "&"
    TOKEN_PIPE, // "|"
    TOKEN_CARET, // "^"
    TOKEN_BANG, // "!"
    TOKEN_LESS_THAN, // "<"
    TOKEN_GREATER_THAN, // ">"

} TokenType;

#define TokenType_FIRST_OPERATOR TOKEN_EQUAL_EQUAL
#define TokenType_MAX TOKEN_GREATER_THAN

static const char *TokenType_to_string(TokenType type)
{
    // need to change to work within ranges or its a huge case statement
    switch (type)
    {
    case TOKEN_ERROR:
        return "TOKEN_ERROR";
    case TOKEN_IDENTIFIER:
        return "TOKEN_IDENTIFIER";
    /* Values */
    case TOKEN_INTEGER_CONST:
        return "TOKEN_INTEGER_CONST";
    case TOKEN_FLOAT_CONST:
        return "TOKEN_FLOAT_CONST";
    case TOKEN_STRING_CONST:
        return "TOKEN_STRING_CONST";
    /* Keywords */
    case TOKEN_INT_KEYWORD:
        return "TOKEN_INT_KEYWORD";
    case TOKEN_FLOAT_KEYWORD:
        return "TOKEN_FLOAT_KEYWORD";
    case TOKEN_STRING_KEYWORD:
        return "TOKEN_STRING_KEYWORD";
    case TOKEN_PRINT_KEYWORD:
        return "TOKEN_PRINT_KEYWORD";
    case TOKEN_IF_KEYWORD:
        return "TOKEN_IF_KEYWORD";
    case TOKEN_THEN_KEYWORD:
        return "TOKEN_THEN_KEYWORD";
    case TOKEN_ELSE_KEYWORD:
        return "TOKEN_ELSE_KEYWORD";
    case TOKEN_WHILE_KEYWORD:
        return "TOKEN_WHILE_KEYWORD";
    case TOKEN_REPEAT_KEYWORD:
        return "TOKEN_REPEAT_KEYWORD";
    case TOKEN_UNTIL_KEYWORD:
        return "TOKEN_UNTIL_KEYWORD";
    case TOKEN_FACTORIAL_KEYWORD:
        return "TOKEN_FACTORIAL_KEYWORD";
    /* Punctuators */
    case TOKEN_SEMICOLON:
        return "TOKEN_SEMICOLON";
    case TOKEN_LEFT_BRACE:
        return "TOKEN_LEFT_BRACE";
    case TOKEN_RIGHT_BRACE:
        return "TOKEN_RIGHT_BRACE";
    case TOKEN_LEFT_PAREN:
        return "TOKEN_LEFT_PAREN";
    case TOKEN_RIGHT_PAREN:
        return "TOKEN_RIGHT_PAREN";
    case TOKEN_EOF:
        return "TOKEN_EOF";
    /* Operators */
    case TOKEN_EQUAL_EQUAL:
        return "TOKEN_EQUAL_EQUAL";
    case TOKEN_BANG_EQUAL:
        return "TOKEN_BANG_EQUAL";
    case TOKEN_GREATER_THAN_EQUAL:
        return "TOKEN_GREATER_THAN_EQUAL";
    case TOKEN_LESS_THAN_EQUAL:
        return "TOKEN_LESS_THAN_EQUAL";
    case TOKEN_LESS_THAN_LESS_THAN:
        return "TOKEN_LESS_THAN_LESS_THAN";
    case TOKEN_GREATER_THAN_GREATER_THAN:
        return "TOKEN_GREATER_THAN_GREATER_THAN";
    case TOKEN_AMPERSAND_AMPERSAND:
        return "TOKEN_AMPERSAND_AMPERSAND";
    case TOKEN_PIPE_PIPE:
        return "TOKEN_PIPE_PIPE";
    case TOKEN_EQUAL:
        return "TOKEN_EQUAL";
    case TOKEN_PLUS:
        return "TOKEN_PLUS";
    case TOKEN_MINUS:
        return "TOKEN_MINUS";
    case TOKEN_STAR:
        return "TOKEN_BANG_EQUALS";
    case TOKEN_FORWARD_SLASH:
        return "TOKEN_FORWARD_SLASH";
    case TOKEN_PERCENT:
        return "TOKEN_PERCENT";
    case TOKEN_TILDE:
        return "TOKEN_TILDE";
    case TOKEN_AMPERSAND:
        return "TOKEN_AMPERSAND";
    case TOKEN_PIPE:
        return "TOKEN_PIPE";
    case TOKEN_CARET:
        return "TOKEN_CARET";
    case TOKEN_BANG:
        return "TOKEN_BANG";
    case TOKEN_READ_KEYWORD:
        return "TOKEN_READ_KEYWORD";
    case TOKEN_LESS_THAN:
        return "TOKEN_LESS_THAN";
    case TOKEN_GREATER_THAN:
        return "TOKEN_GREATER_THAN";
    }
    return "UNKNOWN";
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

static const char *ErrorType_to_error_message(ErrorType error)
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
    }
    return "Unknown error";
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
