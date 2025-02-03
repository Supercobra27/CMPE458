/* lexer.c */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "../../include/tokens.h"
#include "../../include/dynamic_array.h"
#include "../../include/operators.h"

#define FILE_EXT ".cisc"

// keywords
static const char *keywords[] = {
    "if", "else", "while", "factorial",
    "repeat_until", "int", "string"};
static const int num_keywords = 7;

// to check if a string is a keyword
static int is_keyword(const char *str)
{
    for (int i = 0; i < num_keywords; i++)
    {
        if (strcmp(str, keywords[i]) == 0)
        {
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

void init_lexer(int *position, int *line, Array **line_start_positions)
{
    *line = 1;
    *position = 0;
    *line_start_positions = array_new(1, sizeof(int));
    array_push(*line_start_positions, (Element *)position);
}

// Call this function to record a newline character.
// If the input[position] is a newline character, it will record the position of the next character.
// if the input[position] is not a newline character, nothing will happen.
void record_newline(const char *input, int position)
{
    if (input[position] != '\n')
        return;
    position++;
    array_push(line_start, (Element *)&position);
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
    case TOKEN_PUNCTUATOR:
        return "PUNCTUATOR";
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
           token_type_to_string(token.type), token.lexeme, token.position.line, token.position.col_start, token.position.col_end, error_type_to_error_message(token.error));
}

// Do we want to distinguish between errors and warnings?
void print_token_compiler_message(const char *input, Token token)
{
    const int line_start_pos = *(int *)array_get(line_start, token.position.line - 1);
    const int line_length = strchr(input + line_start_pos, '\n') - (input + line_start_pos);
    // tildes is supposed to be as long as the longest token lexeme so that it can always be chopped to the right length.
    static const char *tildes = "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
    printf(
        "%s:%d:%d: %s\n"
        "%.*s\n"
        "%*s%.*s\n",
        "[input file path here]", token.position.line, token.position.col_start, error_type_to_error_message(token.error),
        line_length, input + line_start_pos,
        token.position.col_start, "^", token.position.col_end - token.position.col_start, tildes);
}

/* Get next token from input */
Token get_next_token(const char *input, int *pos)
{
    char c, cn; // first char and the following for checking operations

    // Skip whitespace and track line numbers
    while ((c = input[*pos]) != '\0' && isspace(c))
    {
        if (c == '\n')
        {
            current_line++;
            record_newline(input, *pos);
        }
        (*pos)++;
    }

    Token token = {
        .type = TOKEN_ERROR,
        .lexeme = "",
        .position = {current_line, (*pos) - *(int *)array_get(line_start, current_line - 1) + 1, (*pos) - *(int *)array_get(line_start, current_line - 1) + 1},
        .error = ERROR_NONE};

    if (input[*pos] == '\0')
    {
        token.type = TOKEN_EOF;
        strcpy(token.lexeme, "EOF");
        return token;
    }

    c = input[*pos];
    cn = input[*pos + 1];

    // TODO: Add comment handling here
    // Handle comments
    if (c == '?' && cn == '?')
    {
        // Single-line comment
        while (input[*pos] != '\n' && input[*pos] != '\0')
        {
            (*pos)++;
        }
        return get_next_token(input, pos); // Skip and get next token
    }

    if (c == '?' && cn == '!')
    {
        // Multi-line comment
        (*pos) += 2; // Skip `?!`
        while (!(input[*pos] == '!' && input[*pos + 1] == '?') && input[*pos] != '\0')
        {
            if (input[*pos] == '\n')
            {
                current_line++;
                record_newline(input, *pos);
            }
            (*pos)++;
        }
        if (input[*pos] == '!' && input[*pos + 1] == '?')
        {
            (*pos) += 2; // Skip `!?`
        }
        return get_next_token(input, pos); // Skip and get next token
    }

    // Handle numbers
    if (isdigit(c))
    {
        int i = 0;

        /**
         * Need to do when keywords are added:
         * Length checking as it is a lexical error
         * Have we defined the max length
         * FP Numbers
         */

        do
        {
            if (c == '0' && i == 0 && cn != '.')
            { // handle initial zeroes
                token.lexeme[i++] = c;
                (*pos)++;
                break; // based on regex for first character
            }
            else if (isFloatingPrefix(c, cn))
            {
                token.lexeme[i++] = c;
                token.lexeme[i++] = cn;
                (*pos) += 2; // skip over the starter and dot so not to return error for unknown character
            }
            else
            {
                token.lexeme[i++] = c;
                (*pos)++;
            }
            c = input[*pos];
            cn = input[*pos + 1];
        } while (isdigit(c) && i < sizeof(token.lexeme) - 1);
        token.position.col_end += i - 1;
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
        token.position.col_end += i - 1;

        // Check if it's a keyword
        if (is_keyword(token.lexeme))
        {
            token.type = TOKEN_KEYWORD;
        }
        else
        {
            token.type = TOKEN_IDENTIFIER;
        }

        return token;
    }

    // TODO: Add string literal handling here

    // handling string literals
    if (c == '"')
    {
        int i = 0;
        token.lexeme[i++] = c; // store opening quote
        (*pos)++;

        // Keep reading until we find a closing quote, EOF, or exceed max length
        while ((c = input[*pos]) != '\0' && c != '"')
        {
            // Check if adding this character would exceed our limit
            // We need room for: current chars + this char + closing quote + null terminator
            if (i >= sizeof(token.lexeme) - 2)
            {

                // If we found a closing quote, consume it
                if (c == '"')
                {
                    (*pos)++;
                }

                // We've hit the maximum length, need to report error
                // Keep reading until we find the closing quote or EOF, but don't store
                while ((c = input[*pos]) != '\0' && c != '"')
                {
                    if (c == '\n')
                    {
                        current_line++;
                        record_newline(input, *pos);
                    }
                    (*pos)++;
                }

                token.lexeme[i] = '\0';
                token.type = TOKEN_ERROR;
                token.error = ERROR_STRING_TOO_LONG;
                token.position.col_end += i - 1;
                return token;
            }

            // handle newlines in string
            // if (c == '\n')
            // {
            //     current_line++;
            //     array_push(line_start, (Element *)pos);
            // }

            token.lexeme[i++] = c; // add characters
            (*pos)++;
        }

        // check if we terminated normally
        if (c == '"')
        {
            // normal termination
            token.lexeme[i++] = c; // store the closing quote
            token.lexeme[i] = '\0';
            (*pos)++;
            token.type = TOKEN_STRING_LITERAL;
            token.position.col_end += i - 1;
            return token;
        }

        // if we got here without returning, it must be unterminated
        token.lexeme[i] = '\0';
        token.type = TOKEN_ERROR;
        token.error = ERROR_UNTERMINATED_STRING;
        token.position.col_end += i - 1;
        return token;
    }

    // Handle operators
    if (isOperator(c))
    {
        token.type = TOKEN_OPERATOR;
        token.lexeme[0] = c;

        // If the following character is a valid logical operator (&&, ||)
        if (isLogicalOperator(input[*pos + 1]))
        {

            encapOperator(&token, &pos, &input, LOGICAL_OPERATOR_LENGTH);

            // If it is an invalid consecutive operator
            /*
            } else if (isInvalidOperator(input[*pos+1])) {

        // If it is an invalid consecutive operator
        /*
        } else if (isInvalidOperator(input[*pos+1])) {

            // I am pretty sure this logic only has to be done during parsing

                encapOperator(&token, &pos, &input, LOGICAL_OPERATOR_LENGTH);
                token.error = ERROR_CONSECUTIVE_OPERATOR;

        */
        }
        else
        {
            token.lexeme[1] = '\0';
        }
        (*pos)++;
        return token;
    }

    // TODO: Add punctuation handling here
    // Handle punctuation & delimiters
    const char *punctuation = ";{}(),";
    if (strchr(punctuation, c))
    {
        token.type = TOKEN_PUNCTUATOR;
        token.lexeme[0] = c;
        token.lexeme[1] = '\0';
        (*pos)++;
        return token;
    }

    // Handle invalid characters
    token.error = ERROR_INVALID_CHAR;
    token.lexeme[0] = c;
    token.lexeme[1] = '\0';
    (*pos)++;
    return token;
}

// This is a basic lexer that handles numbers (e.g., "123", "456"), basic operators (+ and -), consecutive operator errors, whitespace and newlines, with simple line tracking for error reporting.

int main(int argc, char *argv[])
{
    // Potential code for file name/extension checking, although when I run it for some reason although it does not change anything the code does not run properly so Work in Progress

    // Input file argument check
    argc = 2;              // force
    argv[1] = "test.cisc"; // force
    if (argc != 2)
    {
        printf("Usage: .\\my-mini-compiler.exe <Input File Name>.cisc");
        exit(-1);
    }

    // Input file extension check
    int file_len = strlen(argv[1]);
    char *file_name = argv[1];
    if (file_len < 5 || strncmp(file_name + file_len - 5, FILE_EXT, 5) != 0)
    {
        printf("Incorrect file extension, the correct extension is .cisc");
        exit(-1);
    }
    const char *input = "123 + 456 - 789\n1 ++ 2\n$$$$\n45+54\nif else while\nvariablename ifelse whilesomething\n"
                        "?? this is a comment\n(123+456);\n?! this is a multline comment\nstill comment\nendingcomment!?\n"
                        "\"this string literal is toooooooooo loooooooooooong some more chracters to fill up what needs"
                        " to be filled to make a really long string\n\"normal string literal\""; // Original Test Case

    /*
    For some reason while testing this, you can only add new test cases at the end?
    Whenever I tried to add at the beginning or the middle it just would not run.
    */

    int position = 0;
    Token token;
    init_lexer(&position, &current_line, &line_start);

    printf("Analyzing input:\n%s\n\n", input);

    // Get all tokens from input and print them.
    do
    {
        token = get_next_token(input, &position);
        print_token(token);
    } while (token.type != TOKEN_EOF);
    // printf("array_size: %u\n", array_size(line_start));
    // Print line start positions.
    for (size_t i = 0; i < array_size(line_start); i++)
    {
        printf("Line %d starts at position %d\n", i + 1, *(int *)array_get(line_start, i));
    }
    printf("%s\n", input + *(int *)array_get(line_start, 13 - 1));
    array_free(line_start);

    // Get all tokens and print all possible compiler messages for tokens.
    init_lexer(&position, &current_line, &line_start);
    do
    {
        token = get_next_token(input, &position);
        printf("array_size: %u\n", array_size(line_start));
        print_token_compiler_message(input, token);
    } while (token.type != TOKEN_EOF);

    array_free(line_start);
    return 0;
}
