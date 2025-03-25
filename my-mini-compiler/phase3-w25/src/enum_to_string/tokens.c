#include "../../include/tokens.h"


const char *TokenType_to_string(const TokenType type)
{
    switch (type)
    {
    case TOKEN_NULL:
        return "TOKEN_NULL";
    case TOKEN_ERROR:
        return "TOKEN_ERROR";
    case TOKEN_IDENTIFIER:
        return "TOKEN_IDENTIFIER";
    /* Values */
    case TOKEN_INTEGER_CONST:
        return "TOKEN_INTEGER_CONST";
    case TOKEN_FLOAT_CONST:
        return "TOKEN_FLOAT_CONST";
    case TOKEN_STRING_CONST:
        return "TOKEN_STRING_CONST";
    /* Keywords */
    case TOKEN_INT_KEYWORD:
        return "TOKEN_INT_KEYWORD";
    case TOKEN_FLOAT_KEYWORD:
        return "TOKEN_FLOAT_KEYWORD";
    case TOKEN_STRING_KEYWORD:
        return "TOKEN_STRING_KEYWORD";
    case TOKEN_PRINT_KEYWORD:
        return "TOKEN_PRINT_KEYWORD";
    case TOKEN_IF_KEYWORD:
        return "TOKEN_IF_KEYWORD";
    case TOKEN_THEN_KEYWORD:
        return "TOKEN_THEN_KEYWORD";
    case TOKEN_ELSE_KEYWORD:
        return "TOKEN_ELSE_KEYWORD";
    case TOKEN_WHILE_KEYWORD:
        return "TOKEN_WHILE_KEYWORD";
    case TOKEN_REPEAT_KEYWORD:
        return "TOKEN_REPEAT_KEYWORD";
    case TOKEN_UNTIL_KEYWORD:
        return "TOKEN_UNTIL_KEYWORD";
    case TOKEN_FACTORIAL_KEYWORD:
        return "TOKEN_FACTORIAL_KEYWORD";
    /* Punctuators */
    case TOKEN_SEMICOLON:
        return "TOKEN_SEMICOLON";
    case TOKEN_LEFT_BRACE:
        return "TOKEN_LEFT_BRACE";
    case TOKEN_RIGHT_BRACE:
        return "TOKEN_RIGHT_BRACE";
    case TOKEN_LEFT_PAREN:
        return "TOKEN_LEFT_PAREN";
    case TOKEN_RIGHT_PAREN:
        return "TOKEN_RIGHT_PAREN";
    case TOKEN_EOF:
        return "TOKEN_EOF";
    /* Operators */
    case TOKEN_EQUAL_EQUAL:
        return "TOKEN_EQUAL_EQUAL";
    case TOKEN_BANG_EQUAL:
        return "TOKEN_BANG_EQUAL";
    case TOKEN_GREATER_THAN_EQUAL:
        return "TOKEN_GREATER_THAN_EQUAL";
    case TOKEN_LESS_THAN_EQUAL:
        return "TOKEN_LESS_THAN_EQUAL";
    case TOKEN_LESS_THAN_LESS_THAN:
        return "TOKEN_LESS_THAN_LESS_THAN";
    case TOKEN_GREATER_THAN_GREATER_THAN:
        return "TOKEN_GREATER_THAN_GREATER_THAN";
    case TOKEN_AMPERSAND_AMPERSAND:
        return "TOKEN_AMPERSAND_AMPERSAND";
    case TOKEN_PIPE_PIPE:
        return "TOKEN_PIPE_PIPE";
    case TOKEN_EQUAL:
        return "TOKEN_EQUAL";
    case TOKEN_PLUS:
        return "TOKEN_PLUS";
    case TOKEN_MINUS:
        return "TOKEN_MINUS";
    case TOKEN_STAR:
        return "TOKEN_BANG_EQUALS";
    case TOKEN_FORWARD_SLASH:
        return "TOKEN_FORWARD_SLASH";
    case TOKEN_PERCENT:
        return "TOKEN_PERCENT";
    case TOKEN_TILDE:
        return "TOKEN_TILDE";
    case TOKEN_AMPERSAND:
        return "TOKEN_AMPERSAND";
    case TOKEN_PIPE:
        return "TOKEN_PIPE";
    case TOKEN_CARET:
        return "TOKEN_CARET";
    case TOKEN_BANG:
        return "TOKEN_BANG";
    case TOKEN_READ_KEYWORD:
        return "TOKEN_READ_KEYWORD";
    case TOKEN_LESS_THAN:
        return "TOKEN_LESS_THAN";
    case TOKEN_GREATER_THAN:
        return "TOKEN_GREATER_THAN";
    }
    return "UNKNOWN";
}

const char *ErrorType_to_error_message(const ErrorType error)
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
    }
    return "Unknown error";
}
