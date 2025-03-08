/* lexer.c */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#include "../../include/lexer.h"
#include "../../include/tokens.h"
#include "../../include/dynamic_array.h"
#include "../../include/operators.h"


/*
// punctuators
static int is_punctuator(const char *str)
{
    static const char *punctuators[] = {";", "{", "}", "(", ")", ","};
    for (int i = 0; i < 6; i++)
    {
        if (strcmp(str, punctuators[i]) == 0)
        {
            return i+1; // 1 to say it found something
        }
    }
    return 0;
}
*/

// keywords
// to check if a string is a keyword
static int is_keyword(const char *str)
{
    static const char *const keywords[] = {
        "int", "float", "string", "print", "if", "then", "else", "while",
        "repeat", "until", "factorial"};
    static const int num_keywords = sizeof(keywords) / sizeof(keywords[0]);

    for (int i = 0; i < num_keywords; i++)
    {
        if (strcmp(str, keywords[i]) == 0)
        {
            return i+1; // 1 to say it found something
        }
    }
    return 0;
}

// global state of the lexer
static bool is_lexer_initialized = false;

// input and input position
static const char *global_input = NULL;
static int global_position = 0;

// Line tracking
static int global_current_line;
// This is a dynamic array of type int to track the start position of each line.
static Array *global_line_start = NULL;

/**
 * global vairables read:
 * 
 */
void init_lexer(const char *input_string, const int start_position)
{
    global_input = input_string;
    global_position = start_position;
    global_current_line = 1;
    if (global_line_start != NULL) array_free(global_line_start);
    global_line_start = array_new(1, sizeof(int));
    array_push(global_line_start, (Element *)&global_position);
    is_lexer_initialized = true;
}


// Call this function to record a newline character.
// If the global_input[position] is a newline character, it will record the position of the next character.
// if the global_input[position] is not a newline character, nothing will happen.
/**
 * global variables read only:
 * - `global_input`
 * - `global_position`
 * 
 * global variables modified only:
 * 
 * global variables read and modified:
 * - `global_current_line`
 * - `global_line_start`
 */
void record_if_newline()
{
    if (global_input[global_position] != '\n')
        return;
    ++global_current_line;
    const int temp_position = global_position + 1;
    array_push(global_line_start, (Element *)&temp_position);
}

/* Error messages for lexical errors */
/* Print token information */
void print_token(Token token)
{
    printf("Token type=%-10s, lexeme='%s', line=%-2d, column:%d-%d, error_message=\"%s\"; ENUM: %d\n",
           token_type_to_string(token.type), token.lexeme, token.position.line, token.position.col_start, token.position.col_end, error_type_to_error_message(token.error), token.type);
}

// Do we want to distinguish between errors and warnings?
void print_token_compiler_message(const char *input_file_path, Token token)
{
    const int line_start_pos = *(int *)array_get(global_line_start, token.position.line - 1);
    const char *const line_end = strchr(global_input + line_start_pos, '\n');
    const int line_length = line_end == NULL ? (int)strlen(global_input + line_start_pos) : line_end - (global_input + line_start_pos);
    // tildes is supposed to be as long as the longest token lexeme so that it can always be chopped to the right length.
    static const char *tildes = "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
    printf(
        "%s:%d:%d: %s\n"
        "%.*s\n"
        "%*s%.*s\n",
        input_file_path, token.position.line, token.position.col_start, error_type_to_error_message(token.error),
        line_length, global_input + line_start_pos,
        token.position.col_start, "^", token.position.col_end - token.position.col_start, tildes);
}

/* Get next token from global_input */
/**
 * global variables read only:
 * - global_input
 * 
 * global variables modified only:
 * 
 * global variables read and modified:
 * - global_position
 * - global_current_line
 * - global_line_start
 */
Token get_next_token()
{
    char c, cn; // first char and the following for checking operations

    // Skip whitespace and track line numbers
    while ((c = global_input[global_position]) != '\0' && isspace(c))
    {
        record_if_newline();
        global_position++;
    }

    Token token = {
        .type = TOKEN_ERROR,
        .lexeme = "",
        .position = (LexemePosition){
            .line = global_current_line, 
            .col_start = global_position - *(int *)array_get(global_line_start, global_current_line - 1) + 1, 
            .col_end = global_position - *(int *)array_get(global_line_start, global_current_line - 1) + 1},
        .error = ERROR_NONE};

    if (global_input[global_position] == '\0')
    {
        token.type = TOKEN_EOF;
        strcpy(token.lexeme, "EOF");
        return token;
    }

    c = global_input[global_position];
    cn = global_input[global_position + 1];

    // Handle comments
    if (c == '?' && cn == '?')
    {
        // Single-line comment
        while (global_input[global_position] != '\n' && global_input[global_position] != '\0')
        {
            global_position++;
        }
        return get_next_token(); // Skip and get next token
    }

    if (c == '?' && cn == '!')
    {
        // Block/Multi-line comment
        global_position += 2; // Skip `?!`
        while (!(global_input[global_position] == '!' && global_input[global_position + 1] == '?') && global_input[global_position] != '\0')
        {
            record_if_newline();
            global_position++;
        }
        if (global_input[global_position] == '\0')
        {
            token.error = ERROR_UNTERMINATED_COMMENT;
            return token;
        }
        if (global_input[global_position] == '!' && global_input[global_position + 1] == '?')
        {
            global_position += 2; // Skip `!?`
        }
        return get_next_token(); // Skip and get next token
    }

    // Handle numbers
    /**
     * TODO: Length checking as it is a lexical error. (current max length is set by the size of token.lexeme)
     */
    if (isdigit(c))
    {
        int i = 0;
        token.type = TOKEN_INTEGER_CONST;
        int float_found = 0;

        do
        {
            if (cn == '.' && float_found == 0)
            {
                token.lexeme[i++] = c;
                token.lexeme[i++] = cn;
                global_position += 2; // skip over the starter and dot so not to return error for unknown character
                token.type = TOKEN_FLOAT_CONST;
                float_found = 1;
            }
            else
            {
                token.lexeme[i++] = c;
                global_position++;
            }
            c = global_input[global_position];
            cn = global_input[global_position + 1];
        } while (isdigit(c) && i < (int)(sizeof(token.lexeme) - 1));
        token.position.col_end += i - 1;
        token.lexeme[i] = '\0';
        return token;
    }

    // handle keywords and identifiers
    if (isalpha(c) || c == '_')
    {
        int i = 0;
        do
        {
            token.lexeme[i++] = c;
            global_position++;
            c = global_input[global_position];
        } while ((isalnum(c) || c == '_') && i < (int)(sizeof(token.lexeme) - 1));

        token.lexeme[i] = '\0';
        token.position.col_end += i - 1;

        // Check if it's a keyword
        int keyword_id = is_keyword(token.lexeme);
        if (keyword_id)
        {
            token.type = (TokenType)(i-1+TOKEN_INT_KEYWORD);
        }
        else
        {
            token.type = TOKEN_IDENTIFIER;
        }

        return token;
    }

    // handling string literals (these string literals do not support escape characters)
    if (c == '"')
    {
        int i = 0;
        token.lexeme[i++] = c; // store opening quote
        global_position++;

        // keep reading until we find the closing quote, a non-printable character, or EOF
        while (isprint(c = global_input[global_position]) && c != '"' && c != '\0')
        {
            // if adding this character would make the string too long, report error
            // need room for: current chars + this char + closing quote + '\0'. - 1 for zero index
            if (i >= (int)(sizeof(token.lexeme) - 2))
            {
                // hit max length, report error
                // keep reading until closing quote, non-printable character, or EOF, but don't store
                while (isprint(c = global_input[global_position]) && c != '"' && c != '\0')
                {
                    global_position++;
                }

                // If we found a closing quote, consume it and terminate with too long string error
                if (c == '"')
                {
                    global_position++;
                    // terminate with string too long error
                    token.lexeme[i] = '\0';
                    token.type = TOKEN_ERROR;
                    token.error = ERROR_STRING_TOO_LONG;
                    token.position.col_end += i - 1;
                    return token;
                }

                // here the string is both too long and there is a non-printable character/EOF before the closing quote
                // terminate with unterminated string error
                token.lexeme[i] = '\0';
                token.type = TOKEN_ERROR;
                token.error = ERROR_UNTERMINATED_STRING;
                token.position.col_end += i - 1;
                return token;
            }

            token.lexeme[i++] = c; // add characters
            global_position++;
        }

        // if we found the closing quote and the string wasn't too long, terminate normally
        if (c == '"')
        {
            token.lexeme[i++] = c; // store closing quote
            token.lexeme[i] = '\0';
            global_position++;
            token.type = TOKEN_STRING_CONST;
            token.position.col_end += i - 1;
            return token;
        }

        // if we got here without returning, loop was exited without finding closing quote
        // terminate with unterminated string error
        record_if_newline();
        token.lexeme[i] = '\0';
        token.type = TOKEN_ERROR;
        token.error = ERROR_UNTERMINATED_STRING;
        token.position.col_end += i - 1;
        return token;
    }

    // Handle operators
    int operator_len = isOperatorStr(global_input + global_position);
    if (operator_len)
    {
        token.type = (TokenType)(findMappableIndex(global_input + global_position)+TOKEN_SINGLE_EQUALS);
        strncpy(token.lexeme, global_input + global_position, operator_len);
        global_position += operator_len;
        return token;
    }

    // Handle punctuation & delimiters
    const char *const punctuation = ";{}(),";
    char* punctcheck = strchr(punctuation, c);
    if (punctcheck)
    {
        token.type = (TokenType)((punctcheck-punctuation)+TOKEN_SEMICOLON);
        token.lexeme[0] = c;
        token.lexeme[1] = '\0';
        global_position++;
        return token;
    }

    // Handle invalid characters
    token.error = ERROR_INVALID_CHAR;
    token.lexeme[0] = c;
    token.lexeme[1] = '\0';
    global_position++;
    return token;
}

/*
int main(int argc, char *argv[])
{

    // Input file argument check
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
    // Load file into input.
    FILE *file = fopen(file_name, "r");
    if (file == NULL)
    {
        printf("Error: Unable to open file %s\n", file_name);
        exit(-1);
    }
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *input = malloc(file_size + 1);
    if (input == NULL)
    {
        printf("Error: Unable to allocate memory for file %s\n", file_name);
        exit(-1);
    }
    size_t bytes_read = 0;
    char ch;
    while (bytes_read < file_size && (ch = fgetc(file)) != EOF && ch != '\0')
        input[bytes_read++] = ch;
    input[bytes_read] = '\0';
    fclose(file);

    // input = "123 + 456 - 789\n1 ++ 2\n$$$$\n45+54\nif else while\nvariablename ifelse whilesomething\n"
    //         "?? this is a comment\n(123+456);\n?! this is a multline comment\nstill comment\nendingcomment!?\n"
    //         "\"this string literal is toooooooooo loooooooooooong some more chracters to fill up what needs"
    //         " to be filled to make a really long string\n\"normal string literal\"";
    // const char *input = "000123 + 456";

    Token token;
    int position;
    Array *tokens = array_new(1, sizeof(Token));
    printf("Analyzing input:\n%s\n\n", input);

    // Get all tokens from input and print them.
    position = 0;
    init_lexer(position);
    do
    {
        token = get_next_token(input, &position);
        print_token(token);
        array_push(tokens, &token);
    } while (token.type != TOKEN_EOF);

    for (size_t i = 0; i < array_size(tokens); i++)
    {
        print_token(*(Token *)array_get(tokens, i));
    }

    array_free(tokens);

    // Print line start positions.
    for (size_t i = 0; i < array_size(global_line_start); i++)
    {
        printf("Line %zu starts at position %d\n", i + 1U, *(int *)array_get(global_line_start, i));
    }
    array_free(global_line_start);

    // Get all tokens and print all possible compiler messages for tokens.
    position = 0;
    init_lexer(position);
    do
    {
        token = get_next_token(input, &position);
        print_token_compiler_message(file_name, input, token);
    } while (token.type != TOKEN_EOF);

    array_free(global_line_start);
    free(input);
    return 0;
}
*/