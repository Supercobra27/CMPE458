#include <operators.h>
#include <tokens.h>
#include <ctype.h>
#include <string.h>

static const int num_multi_operators = 18;
static const char *const single_operators = "=+-*/~|&^!><";
static const char *const multi_operators[] = {
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
        if (strncmp(_Str, multi_operators[i], 2) == 0)
            return 2;

    if (isOperator(_Str[0]))
        return 1; // for single operators

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