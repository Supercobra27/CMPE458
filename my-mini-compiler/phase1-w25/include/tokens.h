//
// Created by Youssef
// Modified by Hendrix Gryspeerdt, Simon John, Monica Saad, and Ryan Silverberg.
//

/* tokens.h */
#ifndef TOKENS_H
#define TOKENS_H

/* Token types that need to be recognized by the lexer */
typedef enum
{
    TOKEN_EOF,
    TOKEN_INTEGER, // regex: ^[0-9]+$
    TOKEN_FLOAT,
    TOKEN_OPERATOR,
    TOKEN_KEYWORD,        // regex: ^(if|else|while|factorial|repeat|until|int|string)$
    TOKEN_IDENTIFIER,     // regex: ^[a-zA-Z_][a-zA-Z0-9_]*$
    TOKEN_STRING_LITERAL, // regex: ^"[ -~]*"$
    TOKEN_PUNCTUATOR,     // ispunct()
    TOKEN_ERROR,
} TokenType;

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