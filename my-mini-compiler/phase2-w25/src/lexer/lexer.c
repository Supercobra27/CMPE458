/* lexer.c */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "../../include/tokens.h"
#include "../../include/dynamic_array.h"
#include "../../include/operators.h"

#define FILE_EXT ".cisc"

void print_error(ErrorType error, int line, const char *lexeme)
{
    printf("print_error unimplemented\n");
}

// punctuators
static const char *punctuators[] = {
";", "{", "}", "(", ")", ","
};

static int is_punctuator(const char *str)
{
    for (int i = 0; i < 6; i++)
    {
        if (strcmp(str, punctuators[i]) == 0)
        {
            return i+1; // 1 to say it found something
        }
    }
    return 0;
}

// keywords
static const char *keywords[] = {
    "int", "float", "string", "print", "if", "then", "else", "while",
    "repeat", "until", "factorial"};
static const int num_keywords = sizeof(keywords) / sizeof(keywords[0]);

// to check if a string is a keyword
static int is_keyword(const char *str)
{
    for (int i = 0; i < num_keywords; i++)
    {
        if (strcmp(str, keywords[i]) == 0)
        {
            return i+1; // 1 to say it found something
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
const char *const error_type_to_error_message(ErrorType error)
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
    default:
        return "Unknown error";
    }
}

const char *token_type_to_string(TokenType type)
{
    // need to change to work within ranges or its a huge case statement

    if (type == TOKEN_EOF){
        return "EOF";
    } else if (type == TOKEN_INTEGER_CONST){
        return "INTEGER";
    }else if (type == TOKEN_FLOAT_CONST){
        return "FLOAT";
    }else if (type == TOKEN_STRING_CONST){
        return "STRING_LITERAL";
    }else if (type >= TOKEN_SINGLE_EQUALS && type <= TOKEN_BANG){
        return "OPERATOR";
    }else if (type >= TOKEN_INT_KEYWORD && type <= TOKEN_FACTORIAL_KEYWORD){
        return "KEYWORD";
    }else if (type >= TOKEN_SEMICOLON && type <= TOKEN_RIGHT_PAREN){
        return "PUNCTUATOR";
    }else if (type == TOKEN_ERROR){
        return "ERROR";
    }else if(type == TOKEN_IDENTIFIER){
        return "IDENTIFIER";
    }else {
        return "UNKNOWN";
    }

/*
    switch (type)
    {
    case TOKEN_EOF:
        return "EOF";
    case TOKEN_INTEGER_CONST:
        return "INTEGER";
    case TOKEN_FLOAT_CONST:
        return "FLOAT";
    case TOKEN_SINGLE_EQUALS: // got to change
        return "OPERATOR";
    case TOKEN_INT_KEYWORD:
        return "KEYWORD"; // got to change
    case TOKEN_IDENTIFIER:
        return "IDENTIFIER";
    case TOKEN_STRING_CONST:
        return "STRING_LITERAL";
    case TOKEN_ERROR:
        return "ERROR";
    case TOKEN_SEMICOLON: // got to change
        return "PUNCTUATOR";
    default:
        return "UNKNOWN";
    }
    */
}

/* Print token information */
void print_token(Token token)
{
    printf("Token type=%-10s, lexeme='%s', line=%-2d, column:%d-%d, error_message=\"%s\"; ENUM: %d\n",
           token_type_to_string(token.type), token.lexeme, token.position.line, token.position.col_start, token.position.col_end, error_type_to_error_message(token.error), token.type);
}

// Do we want to distinguish between errors and warnings?
void print_token_compiler_message(const char *input_file_path, const char *input, Token token)
{
    const int line_start_pos = *(int *)array_get(line_start, token.position.line - 1);
    const char *const line_end = strchr(input + line_start_pos, '\n');
    const int line_length = line_end == NULL ? strlen(input + line_start_pos) : line_end - (input + line_start_pos);
    // tildes is supposed to be as long as the longest token lexeme so that it can always be chopped to the right length.
    static const char *tildes = "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
    printf(
        "%s:%d:%d: %s\n"
        "%.*s\n"
        "%*s%.*s\n",
        input_file_path, token.position.line, token.position.col_start, error_type_to_error_message(token.error),
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
        // Block/Multi-line comment
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
        if (input[*pos] == '\0')
        {
            token.error = ERROR_UNTERMINATED_COMMENT;
            return token;
        }
        if (input[*pos] == '!' && input[*pos + 1] == '?')
        {
            (*pos) += 2; // Skip `!?`
        }
        return get_next_token(input, pos); // Skip and get next token
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
                (*pos) += 2; // skip over the starter and dot so not to return error for unknown character
                token.type = TOKEN_FLOAT_CONST;
                float_found = 1;
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
        return token;
    }

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
        (*pos)++;

        // keep reading until we find the closing quote, a non-printable character, or EOF
        while (isprint(c = input[*pos]) && c != '"' && c != '\0')
        {
            // if adding this character would make the string too long, report error
            // need room for: current chars + this char + closing quote + '\0'. - 1 for zero index
            if (i >= sizeof(token.lexeme) - 2)
            {
                // hit max length, report error
                // keep reading until closing quote, non-printable character, or EOF, but don't store
                while (isprint(c = input[*pos]) && c != '"' && c != '\0')
                {
                    (*pos)++;
                }

                // If we found a closing quote, consume it and terminate with too long string error
                if (c == '"')
                {
                    (*pos)++;
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
            (*pos)++;
        }

        // if we found the closing quote and the string wasn't too long, terminate normally
        if (c == '"')
        {
            token.lexeme[i++] = c; // store closing quote
            token.lexeme[i] = '\0';
            (*pos)++;
            token.type = TOKEN_STRING_CONST;
            token.position.col_end += i - 1;
            return token;
        }

        // if we got here without returning, loop was exited without finding closing quote
        // terminate with unterminated string error
        token.lexeme[i] = '\0';
        token.type = TOKEN_ERROR;
        token.error = ERROR_UNTERMINATED_STRING;
        token.position.col_end += i - 1;
        return token;
    }

    // Handle operators
    int operator_len = isOperatorStr(input + *pos);
    if (operator_len)
    {
        token.type = (TokenType)(findMappableIndex(input + *pos)+TOKEN_SINGLE_EQUALS);
        strncpy(token.lexeme, input + *pos, operator_len);
        (*pos) += operator_len;
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

    // Print line start positions.
    for (size_t i = 0; i < array_size(line_start); i++)
    {
        printf("Line %zu starts at position %d\n", i + 1U, *(int *)array_get(line_start, i));
    }
    array_free(line_start);

    // Get all tokens and print all possible compiler messages for tokens.
    init_lexer(&position, &current_line, &line_start);
    do
    {
        token = get_next_token(input, &position);
        print_token_compiler_message(file_name, input, token);
    } while (token.type != TOKEN_EOF);

    array_free(line_start);
    free(input);
    return 0;
}