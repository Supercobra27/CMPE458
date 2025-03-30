/* lexer.h */
#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>

#include "tokens.h"

#include "dynamic_array.h"

typedef struct _Lexer {
    const char *input_string;
    int current_position;
    int current_line;
    Array *line_start_positions;
    bool is_initialized;
} Lexer;

/**
 * @brief Initializes the internal state of the lexer.
 * 
 * Must be called before calling `get_next_token`.
 * 
 * Internally, this sets global variables for the input string, current position, and line tracking.
 * 
 * @param input_string The input string to tokenize. This string must be terminated by a null character `'\0'`. This string will not be modified by the lexer. The memory will not be freed by the lexer. 
 * @param start_position The position in the input where the lexer should start. This is required to track positions of the start of each newline.
 */
void init_lexer(Lexer *l, const char *input_string, const int start_position);

/**
 * @brief Get the next token from the input.
 * 
 * WARNING: You must call `init_lexer` before calling this function. Calling this function after `TOKEN_EOF` is returned will result in undefined behavior.
 * 
 * @return Token The next token in the input.
 * 
 */
Token get_next_token(Lexer *l);

#endif /* LEXER_H */