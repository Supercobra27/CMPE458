//
// Modified by Hendrix Gryspeerdt, Simon John, Monica Saad, and Ryan Silverberg.
//

/* tokens.h */
#ifndef TOKENS_H
#define TOKENS_H

/* Token types that need to be recognized by the lexer */
typedef enum _TokenType
{
    TOKEN_NULL, // Not a parsed token, but used to denote the end of a Null-terminated array of TokenType.
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
const char *TokenType_to_string(TokenType type);

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

const char *ErrorType_to_error_message(ErrorType error);

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
    ErrorType error;         // Error type if the token is an TOKEN_ERROR.
    LexemePosition position; // Position of this token's lexeme in the input
    char lexeme[100];        // Actual text of the token
} Token;

#endif /* TOKENS_H */
