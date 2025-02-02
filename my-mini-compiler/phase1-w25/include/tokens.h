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

/*
 - Change to have integer and FP number token seperation
*/
typedef enum
{
    TOKEN_EOF,
    TOKEN_INTEGER,
    TOKEN_FLOAT,
    TOKEN_OPERATOR,
    TOKEN_ASSIGNMENT_OPERATOR,
    TOKEN_ARITHMETIC_OPERATOR,
    TOKEN_LOGICAL_OPERATOR,
    TOKEN_BITWISE_OPERATOR,
    TOKEN_KEYWORD,        // e.g., "if", "else", "while", "factorial", "repeat_until", "int", "string",
    TOKEN_IDENTIFIER,     //
    TOKEN_STRING_LITERAL, // basic will not include escape characters "^\"[^\"]*\"". next step will be to include escape characters.
    // TOKEN_PUNCTUATOR,     // "(", ")", "{", "}", ";"
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
    ERROR_CONSECUTIVE_OPERATOR
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
    ErrorType error;        // Error type if any
} Token;

#endif /* TOKENS_H */
