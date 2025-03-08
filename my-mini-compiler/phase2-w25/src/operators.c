#include <operators.h>
#include <tokens.h>
#include <ctype.h>
#include <string.h>

static const int num_multi_operators = 18;
static const char *const single_operators = "=+-*/~|&^!><";
// These operators are ordered in non-increasing length. Not all of these operators (namely the compound assignment operators) are actually used (see ordered_operators for all operators used in the programming language).
static const char *const multi_operators[] = {
    "<<=",
    ">>=",
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
    "%",
    "~",
    "&",
    "|",
    "^",
    "!",
    "<",
    ">"
};

// These operators are ordered such that the ordering coincides with the Token enum.
static const char *const ordered_operators[] = {
    "=",
    "||",
    "&&",
    "|",
    "^",
    "&",
    "<=",
    "<",
    ">=",
    ">",
    "==",
    "!=",
    "<<",
    ">>",
    "+",
    "-",
    "*",
    "/",
    "%",
    "~",
    "!"
};

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
    return c != '\0' && strchr(single_operators, c) != NULL;
}

#define OrderedLength (sizeof(ordered_operators)/sizeof(ordered_operators[0]))
int findMappableIndex(const char *_Str)
{
    for (unsigned int i = 0; i < OrderedLength; i++){
        if (strncmp(_Str, ordered_operators[i], 2) == 0){
            return i;
        }
        else if (strncmp(_Str, ordered_operators[i], 1) == 0){
            return i;
        }
    }
    return -1;
}
