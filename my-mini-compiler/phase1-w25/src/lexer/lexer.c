/* lexer.c */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "../../include/tokens.h"

// Line tracking
static int current_line = 1;
// This needs to be a dynamic array to track the start position of each line.
static int line_start = 0;
// TODO: Add column tracking

/* Error messages for lexical errors */
const char *error_type_to_error_message(ErrorType error)
{
    switch (error)
    {
    case ERROR_NONE:
        return "No error";
    case ERROR_INVALID_CHAR:
        return "Invalid character";
    case ERROR_INVALID_NUMBER:
        return "Invalid number format";
    default:
        return "Unknown error";
    }
}

const char *token_type_to_string(TokenType type)
{
    switch (type)
    {
    case TOKEN_EOF:
        return "EOF";
    case TOKEN_NUMBER:
        return "NUMBER";
    case TOKEN_OPERATOR:
        return "OPERATOR";
    case TOKEN_ERROR:
        return "ERROR";
    default:
        return "UNKNOWN";
    }
}

/* Print token information
 *
 *  TODO Update your printing function accordingly
 */

void print_token(Token token)
{
    printf("Token type=%-10s, lexeme='%s', line=%-2d, pos:%d-%d, error_message=\"%s\"\n",
           token_type_to_string(token.type), token.lexeme, token.position.line, token.position.pos_start, token.position.pos_end, error_type_to_error_message(token.error));
}

/* Get next token from input */
Token get_next_token(const char *input, int *pos)
{
    char c;

    // Skip whitespace and track line numbers
    while ((c = input[*pos]) != '\0' && isspace(c))
    {
        if (c == '\n')
        {
            current_line++;
            line_start = (*pos) + 1;
        }
        (*pos)++;
    }

    Token token = {
        .type = TOKEN_ERROR,
        .lexeme = "",
        .position = {current_line, (*pos), (*pos)},
        .error = ERROR_NONE};

    if (input[*pos] == '\0')
    {
        token.type = TOKEN_EOF;
        strcpy(token.lexeme, "EOF");
        return token;
    }

    c = input[*pos];

    // TODO: Add comment handling here

    // Handle numbers
    if (isdigit(c))
    {
        int i = 0;
        do
        {
            token.lexeme[i++] = c;
            (*pos)++;
            c = input[*pos];
        } while (isdigit(c) && i < sizeof(token.lexeme) - 1);
        token.position.pos_end = (*pos) - 1;
        token.lexeme[i] = '\0';
        token.type = TOKEN_NUMBER;
        return token;
    }

    // TODO: Add keyword and identifier handling here
    // Hint: You'll have to add support for keywords and identifiers, and then string literals

    // TODO: Add string literal handling here

    // Handle operators
    if (c == '+' || c == '-')
    {
        token.type = TOKEN_OPERATOR;
        token.lexeme[0] = c;
        token.lexeme[1] = '\0';
        (*pos)++;
        return token;
    }

    // TODO: Add punctuation handling here

    // Handle invalid characters
    token.error = ERROR_INVALID_CHAR;
    token.lexeme[0] = c;
    token.lexeme[1] = '\0';
    (*pos)++;
    return token;
}

// This is a basic lexer that handles numbers (e.g., "123", "456"), basic operators (+ and -), consecutive operator errors, whitespace and newlines, with simple line tracking for error reporting.

int main()
{
    const char *input = "123 + 456 - 789\n1 ++ 2\n$$$$\n45+54"; // Test with multi-line input
    int position = 0;
    Token token;

    printf("Analyzing input:\n%s\n\n", input);

    do
    {
        token = get_next_token(input, &position);
        print_token(token);
    } while (token.type != TOKEN_EOF);

    return 0;
}
