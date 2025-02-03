#include <operators.h>
#include <tokens.h>
#include <ctype.h>
#include <string.h>

/**
 * Have a list with single and double operators
 */
static const int num_multi_operators = 18;
const char *single_operators = "=+-*/~|&^!><";
const char *multi_operators[] = {
    "==",
    "!=",
    ">=",
    "<=",
    "+=",
    "-=",
    "*=",
    "/=",
    "|=",
    "&=",
    "~=",
    "^=",
    "<<",
    ">>",
    "&&",
    "||",
    "++",
    "--",
    "=",
    "+",
    "-",
    "/",
    "*",
    "~",
    "&",
    "|",
    "^",
    "!",
    "<",
    ">"};

int isOperatorStr(const char *_Str)
{
    for (int i = 0; i < num_multi_operators; i++)
    {
        if (strncmp(_Str, multi_operators[i], 2) == 0)
        {
            //             printf("%s", _Str);
            return 1;
        }
    }

    if (isOperator(_Str[0]))
        return 1; // for single operators

    return 0;
}

int strictOperatorStr(const char *_Str)
{
    for (int i = 0; i < num_multi_operators; i++)
    {
        if (strncmp(_Str, multi_operators[i], 2) == 0)
        {
            return 1;
        }
    }

    return 0;
}

int isOperator(char c)
{
    return strchr(single_operators, c) != NULL;
}

int isFloatingPrefix(char c, char cn)
{
    return (isdigit(c) && cn == '.');
}

int isValidPrefix(char c)
{
    return isOperator(c);
}

int isValidPostfix(char cp, char c)
{
    return isOperator(cp) && !(c == '<' || c == '>');
}

int isLogicalOperator(char c)
{
    return (c == '&' || c == '|' || c == '=' || c == '>' || c == '<');
}

int isInvalidOperator(char c)
{
    return isOperator(c) && !(isLogicalOperator(c));
}

// dont need double pointers
void encapOperator(Token *token, int *pos, const char *input, int len)
{
    int i = 1;
    while (i != len && !isInvalidOperator(input[*pos]))
    {
        (*pos)++;
        if (input[*pos] == ' ')
            break; // prevent spaces from being added to the lexeme
        token->lexeme[i] = input[*pos];
        token->position.col_end++;
        i++;
    };
    token->lexeme[i] = '\0';
    if (strlen(token->lexeme) == 3 && !(strictOperatorStr(token->lexeme)))
        token->type = TOKEN_ERROR;
}