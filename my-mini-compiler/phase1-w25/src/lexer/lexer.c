/* lexer.c */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "../../include/tokens.h"
#include "../../include/dynamic_array.h"
#include "../../include/operators.h"

#define FILE_EXT ".cisc"

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
    case ERROR_CONSECUTIVE_OPERATOR:
        return "error: invalid consecutive operator";
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
    char c, cn; // first char and the following for checking operations

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
    cn = input[*pos+1];

    // TODO: Add comment handling here

    // Handle numbers
    if (isdigit(c))
    {
        int i = 0;
        do
        {
            if (c == '0' && i == 0 && cn != '.') { // handle initial zeroes
                token.lexeme[i++] = c;
                (*pos)++;
                break; // based on regex for first character
            } else if (isFloatingPrefix(c, cn)) {
                token.lexeme[i++] = c;
                token.lexeme[i++] = cn;
                (*pos) += 2; // skip over the starter and dot so not to return error for unknown character
            } else {
                token.lexeme[i++] = c;
                (*pos)++;
            }
            c = input[*pos];
            cn = input[*pos+1];
        } while (isdigit(c) && i < sizeof(token.lexeme) - 1);
        token.position.pos_end += i - 1;
        token.lexeme[i] = '\0';
        token.type = TOKEN_NUMBER;
        return token;
    }

    // TODO: Add keyword and identifier handling here
    // Hint: You'll have to add support for keywords and identifiers, and then string literals

    // TODO: Add string literal handling here

    // Handle operators
    if (isOperator(c))
    {
        token.type = TOKEN_OPERATOR;
        token.lexeme[0] = c;

        // If the following character is a valid logical operator (&&, ||)
        if (isLogicalOperator(input[*pos+1])){

            encapOperator(&token, &pos, &input, LOGICAL_OPERATOR_LENGTH);


        // If it is an invalid consecutive operator
        /*
        } else if (isInvalidOperator(input[*pos+1])) {
            
            // I am pretty sure this logic only has to be done during parsing

            encapOperator(&token, &pos, &input, LOGICAL_OPERATOR_LENGTH);
            token.error = ERROR_CONSECUTIVE_OPERATOR;

        */
       
       } else {
            token.lexeme[1] = '\0';
        }
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

int main(int argc, char *argv[])
{
    /* Potential code for file name/extension checking, although when I run it for some reason although it does not change anything the code does not run properly
    so Work in Progress

    // Input file argument check
    if (argc != 2) { 
        printf("Usage: .\\my-mini-compiler.exe <Input File Name>.cisc");
        exit(-1);
    }

    // Input file extension check
    int file_len = strlen(argv[1]);
    char* file_name = argv[1];
    if(file_len < 5 || strncmp(file_name + file_len - 5, FILE_EXT, 5) != 0){
        printf("Incorrect file extension, the correct extension is .cisc");
        exit(-1);
    }
    */

   // "123 + 456 - 789\n1 ++ 2\n$$$$\n45+54" - Original Test Case

    const char *input = "1 &&== 01\n2$3 |= 20\n3 == 3.2\n5 =< 5.6543\n6 ** 6"; // Test with multi-line input

    
    /*
        For some reason while testing this, you can only add new test cases at the end?
        Whenever I tried to add at the beginning or the middle it just would not run.
    */

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
