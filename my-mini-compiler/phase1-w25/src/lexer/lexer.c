/* lexer.c */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "../../include/tokens.h"
#include "../../include/dynamic_array.h"

// keywords
static const char *keywords[] = {
    "if", "else", "while", "factorial",
    "repeat_until", "int", "string"
};
static const int num_keywords = 7;

// to check if a string is a keyword
static int is_keyword(const char *str) {
    for (int i = 0; i < num_keywords; i++) {
        if (strcmp(str, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Line tracking
static int current_line;
// This is a dynamic array of type int to track the start position of each line.
static Array *line_start;
// Could save all tokens in a dynamic array if needed.

void init_lexer(int *position)
{
    current_line = 1;
    *position = 0;
    line_start = array_new(1, sizeof(int));
    array_push(line_start, (Element *)position);
}

/* Error messages for lexical errors */
const char *error_type_to_error_message(ErrorType error)
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
    case TOKEN_KEYWORD:
        return "KEYWORD";
    case TOKEN_IDENTIFIER:
        return "IDENTIFIER";
    case TOKEN_STRING_LITERAL:
        return "STRING_LITERAL";
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
    printf("Token type=%-10s, lexeme='%s', line=%-2d, column:%d-%d, error_message=\"%s\"\n",
           token_type_to_string(token.type), token.lexeme, token.position.line, token.position.pos_start, token.position.pos_end, error_type_to_error_message(token.error));
}

// Do we want to distinguish between errors and warnings?
void print_token_compiler_message(const char *input, Token token)
{
    const int line_start_pos = (int)array_get(line_start, token.position.line - 1);
    const int line_length = strchr(input + line_start_pos, '\n') - (input + line_start_pos);
    // tildes is supposed to be as long as the longest token lexeme so that it can always be chopped to the right length.
    static const char *tildes = "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
    printf(
        "%s:%d:%d: %s\n"
        "%.*s\n"
        "%*s%.*s\n",
        "[input file path here]", token.position.line, token.position.pos_start, error_type_to_error_message(token.error),
        line_length, input + line_start_pos,
        token.position.pos_start, "^", token.position.pos_end - token.position.pos_start, tildes);
}

/* Get next token from input */
Token get_next_token(const char *input, int *pos)
{
    char c;

    // Skip whitespace and track line numbers
    while ((c = input[*pos]) != '\0' && isspace(c))
    {
        (*pos)++;
        if (c == '\n')
        {
            current_line++;
            array_push(line_start, (Element *)pos);
        }
    }

    Token token = {
        .type = TOKEN_ERROR,
        .lexeme = "",
        .position = {current_line, (*pos) - (int)array_get(line_start, current_line - 1) + 1, (*pos) - (int)array_get(line_start, current_line - 1) + 1},
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
        token.position.pos_end += i - 1;
        token.lexeme[i] = '\0';
        token.type = TOKEN_NUMBER;
        return token;
    }

    // TODO: Add keyword and identifier handling here
    // Hint: You'll have to add support for keywords and identifiers, and then string literals

    // handle keywords and identifiers
    if (isalpha(c) || c == '_')
    {
        int i = 0;
        do
        {
            token.lexeme[i++] = c;
            (*pos)++;
            c = input[*pos];
        } while ((isalnum(c) || c == '_') && i < sizeof(token.lexeme) - 1);

        token.lexeme[i] = '\0';
        token.position.pos_end += i - 1;

        // Check if it's a keyword
        if (is_keyword(token.lexeme)) {
            token.type = TOKEN_KEYWORD;
        } else {
            token.type = TOKEN_IDENTIFIER;
        }

        return token;
    }



    // TODO: Add string literal handling here

    // handling string literals
    if (c == '"')
    {
        int i = 0;
        token.lexeme[i++] = c;  // store opening quote
        (*pos)++;

        while ((c = input[*pos]) != '\0' && c != '"' && i < sizeof(token.lexeme) - 2)
        {
            // handle newlines in string
            if (c == '\n')
            {
                current_line++;
                array_push(line_start, (Element *)pos);
            }

            token.lexeme[i++] = c; // add characters
            (*pos)++;
        }

        // check what the exit condition was
        if (c == '"')
        {
            // normal termination
            token.lexeme[i++] = c;  // store the closing quote
            token.lexeme[i] = '\0';
            (*pos)++;
            token.type = TOKEN_STRING_LITERAL;
            token.position.pos_end += i - 1;
            return token;
        }
        //buffer filled up, string too long
        else if (i >= sizeof(token.lexeme) - 2)
        {
            token.lexeme[i] = '\0';
            token.type = TOKEN_ERROR;
            token.error = ERROR_STRING_TOO_LONG;
            token.position.pos_end += i - 1;
            return token;
        }
        // if it wasn't the other two then it's unterminated
        else
        {
            token.lexeme[i] = '\0';
            token.type = TOKEN_ERROR;
            token.error = ERROR_UNTERMINATED_STRING;
            token.position.pos_end += i - 1;
            return token;
        }
    }

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
    init_lexer(&position);

    printf("Analyzing input:\n%s\n\n", input);

    // Get all tokens from input and print them.
    do
    {
        token = get_next_token(input, &position);
        print_token(token);
    } while (token.type != TOKEN_EOF);

    // Print line start positions.
    for (size_t i = 0; i < array_size(line_start); i++)
    {
        printf("Line %d starts at position %d\n", i + 1, (int)array_get(line_start, i));
    }
    array_free(line_start);

    // Get all tokens and print all possible compiler messages for tokens.
    init_lexer(&position);
    do
    {
        token = get_next_token(input, &position);
        print_token_compiler_message(input, token);
    } while (token.type != TOKEN_EOF);

    return 0;
}
