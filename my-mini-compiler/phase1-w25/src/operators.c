#include <operators.h>
#include <tokens.h>
#include <ctype.h>
#include <string.h>

/**
 * Have a list with single and double operators
 */

const char* operatorList = "+-*/~|&^+!><";
static const int num_operators = 26;
const char* operators[] = {
    ">>>",
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
    "&&",
    "~=",
    "^=",
    "<<",
    ">>",
    "+",
    "-",
    "*",
    "/",
    "~",
    "|",
    "&",
    "^",
    "!",
    "<",
    ">",
    "="
};

int isOperatorStr(const char* _Str){
    for (int i = 0; i < num_operators; i++) {
        if (strncmp(_Str, operators[i], 2) == 0) {
            return 1;
        }
    }
    return 0;
}

int isOperator(char c)
{
    return strchr(operatorList, c);
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
void encapOperator(Token *token, int **pos, char **input, int len)
{
    int i = 1;
    while (i != len && !isInvalidOperator((*input)[**pos]))
    {
        (**pos)++;
        if ((*input)[**pos] == ' ')
            break; // prevent spaces from being added to the lexeme
        token->lexeme[i] = (*input)[**pos];
        token->position.pos_end++;
        i++;
    };
    token->lexeme[i] = '\0';
}