//
// Created by Youssef
//

/* tokens.h */
#ifndef TOKENS_H
#define TOKENS_H

/* Token types that need to be recognized by the lexer
 * TODO: Add more token types as per requirements:
 * - Keywords or reserved words (if, repeat, until)
 * - Identifiers
 * - String literals
 * - More operators
 * - Delimiters
 */
typedef enum
{
    TOKEN_EOF,
    TOKEN_NUMBER,     // e.g., "123", "456"
    TOKEN_OPERATOR,   // e.g., "+", "-"
    TOKEN_KEYWORD,    // e.g., "if", "else", "while", "factorial", "repeat", "until", "int", "string",
    TOKEN_IDENTIFIER, //
    TOKEN_STRING_LITERAL,
    // TOKEN_PUNCTUATOR, // ispunct()
    TOKEN_ERROR,
} TokenType;

/* Error types for lexical analysis
 * TODO: Add more error types as needed for your language - as much as you like !!
 */
typedef enum
{
    ERROR_NONE,
    ERROR_INVALID_CHAR,
    ERROR_INVALID_NUMBER,
    ERROR_UNTERMINATED_STRING,
    ERROR_STRING_TOO_LONG,
} ErrorType;

/* Details for positions of tokens in a file. */
typedef struct _TokenPosition
{
    int line;      // Line number of the token in the file.
    int pos_start; // Start position of the token in the line.
    int pos_end;   // End position of the token in the line.
} TokenPosition;

/* Token structure to store token information
 * TODO: Add more fields if needed for your implementation
 * Hint: You might want to consider adding line and column tracking if you want to debug your lexer properly.
 * Don't forget to update the token fields in lexer.c as well
 */
typedef struct
{
    TokenType type;
    char lexeme[100];       // Actual text of the token
    TokenPosition position; // Position of the token in the input
    ErrorType error;        // Error type if the token is an TOKEN_ERROR.
    // union
    // {
    //     ErrorType error;       // Error type if the token is an TOKEN_ERROR.
    //     OperatorType operator; // Operator type if the token is an TOKEN_OPERATOR.
    //     KeywordType keyword;   // Keyword type if the token is an TOKEN_KEYWORD.
    //     NumberType number;     // Number type if the token is an TOKEN_NUMBER.

    // } type2; // Secondary type information.
} Token;

#endif /* TOKENS_H */
