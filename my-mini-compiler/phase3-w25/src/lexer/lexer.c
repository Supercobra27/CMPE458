/* lexer.c */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "../../include/lexer.h"
#include "../../include/dynamic_array.h"
#include "../../include/operators.h"



// keywords
// to check if a string is a keyword
// return the offset of the keyword in the keywords array
// return -1 if it is not a keyword
static inline int is_keyword(const char *str)
{
    static const char *const keywords[] = {
        "int", "float", "string", "print", "read", "if", "then", "else", "while",
        "repeat", "until", "factorial"};
    static const int num_keywords = 12;

    for (int i = 0; i < num_keywords; i++)
    {
        if (strcmp(str, keywords[i]) == 0)
        {
            return i; 
        }
    }
    return -1;
}

void init_lexer(Lexer *const l, const char *input_string, const int start_position)
{
    l->input_string = input_string;
    l->current_position = start_position;
    l->current_line = 1;
    if (l->line_start_positions != NULL) array_free(l->line_start_positions);
    l->line_start_positions = array_new(1, sizeof(int));
    array_push(l->line_start_positions, (Element *)&l->current_position);
    l->is_initialized = true;
}


// Call this function to record a newline character.
// If the l->input_string[l->current_position] is a newline character, it will record the position of the next character.
// if the l->input_string[l->current_position] is not a newline character, nothing will happen.
static inline void record_if_newline(Lexer *const l)
{
    if (l->input_string[l->current_position] != '\n')
        return;
    ++l->current_line;
    const int temp_position = l->current_position + 1;
    array_push(l->line_start_positions, (Element *)&temp_position);
}


/* Get next token from l->input_string */
Token get_next_token(Lexer *const l)
{
    char c, cn; // first char and the following for checking operations

    // Skip whitespace and track line numbers
    while ((c = l->input_string[l->current_position]) != '\0' && isspace(c))
    {
        record_if_newline(l);
        l->current_position++;
    }

    Token token = {
        .type = TOKEN_ERROR,
        .lexeme = "",
        .position = (LexemePosition){
            .line = l->current_line, 
            .col_start = l->current_position - *(int *)array_get(l->line_start_positions, l->current_line - 1) + 1, 
            .col_end = l->current_position - *(int *)array_get(l->line_start_positions, l->current_line - 1) + 1},
        .error = ERROR_NONE};

    if (l->input_string[l->current_position] == '\0')
    {
        token.type = TOKEN_EOF;
        strcpy(token.lexeme, "\0");
        return token;
    }

    c = l->input_string[l->current_position];
    cn = l->input_string[l->current_position + 1];

    // Handle comments
    if (c == '?' && cn == '?')
    {
        // Single-line comment
        while (l->input_string[l->current_position] != '\n' && l->input_string[l->current_position] != '\0')
        {
            l->current_position++;
        }
        return get_next_token(l); // Skip and get next token
    }

    if (c == '?' && cn == '!')
    {
        // Block/Multi-line comment
        l->current_position += 2; // Skip `?!`
        while (!(l->input_string[l->current_position] == '!' && l->input_string[l->current_position + 1] == '?') && l->input_string[l->current_position] != '\0')
        {
            record_if_newline(l);
            l->current_position++;
        }
        if (l->input_string[l->current_position] == '\0')
        {
            token.error = ERROR_UNTERMINATED_COMMENT;
            return token;
        }
        if (l->input_string[l->current_position] == '!' && l->input_string[l->current_position + 1] == '?')
        {
            l->current_position += 2; // Skip `!?`
        }
        return get_next_token(l); // Skip and get next token
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
                l->current_position += 2; // skip over the starter and dot so not to return error for unknown character
                token.type = TOKEN_FLOAT_CONST;
                float_found = 1;
            }
            else
            {
                token.lexeme[i++] = c;
                l->current_position++;
            }
            c = l->input_string[l->current_position];
            cn = l->input_string[l->current_position + 1];
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
            l->current_position++;
            c = l->input_string[l->current_position];
        } while ((isalnum(c) || c == '_') && i < (int)(sizeof(token.lexeme) - 1));

        token.lexeme[i] = '\0';
        token.position.col_end += i - 1;

        // Check if it's a keyword
        int keyword_id = is_keyword(token.lexeme);
        if (keyword_id != -1)
        {
            token.type = (TokenType)(keyword_id+TOKEN_INT_KEYWORD);
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
        l->current_position++;

        // keep reading until we find the closing quote, a non-printable character, or EOF
        while (isprint(c = l->input_string[l->current_position]) && c != '"' && c != '\0')
        {
            // if adding this character would make the string too long, report error
            // need room for: current chars + this char + closing quote + '\0'. - 1 for zero index
            if (i >= (int)(sizeof(token.lexeme) - 2))
            {
                // hit max length, report error
                // keep reading until closing quote, non-printable character, or EOF, but don't store
                while (isprint(c = l->input_string[l->current_position]) && c != '"' && c != '\0')
                {
                    l->current_position++;
                }

                // If we found a closing quote, consume it and terminate with too long string error
                if (c == '"')
                {
                    l->current_position++;
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
            l->current_position++;
        }

        // if we found the closing quote and the string wasn't too long, terminate normally
        if (c == '"')
        {
            token.lexeme[i++] = c; // store closing quote
            token.lexeme[i] = '\0';
            l->current_position++;
            token.type = TOKEN_STRING_CONST;
            token.position.col_end += i - 1;
            return token;
        }

        // if we got here without returning, loop was exited without finding closing quote
        // terminate with unterminated string error
        record_if_newline(l);
        token.lexeme[i] = '\0';
        token.type = TOKEN_ERROR;
        token.error = ERROR_UNTERMINATED_STRING;
        token.position.col_end += i - 1;
        return token;
    }

    // Handle operators
    int operator_id = operator_index(l->input_string + l->current_position);
    if (operator_id != -1)
    {
        size_t operator_len = strlen(reduced_operators[operator_id]);
        token.type = (TokenType)(TokenType_FIRST_OPERATOR + operator_id);
        strncpy(token.lexeme, l->input_string + l->current_position, operator_len);
        token.lexeme[operator_len] = '\0';
        token.position.col_end += operator_len;
        l->current_position += operator_len;
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
        l->current_position++;
        return token;
    }

    // Handle invalid characters
    token.error = ERROR_INVALID_CHAR;
    token.lexeme[0] = c;
    token.lexeme[1] = '\0';
    l->current_position++;
    return token;
}
