#include <operators.h>
#include <tokens.h>
#include <ctype.h>

int isOperator(char c){
    return (c == '+' || c == '-' || c == '*' || c == '/' || c == '~' || c == '|' || c == '&' || c == '^');
}

int isLogicalOperator(char c){
    return (c == '&' || c == '|');
}

int isInvalidOperator(char c){
    return isOperator(c) && !(isLogicalOperator(c));
}

void encapOperator(Token *token, int **pos, char **input, int len){
            int i = 1;
            while ( i != len  && !isInvalidOperator((*input)[**pos])){
                (**pos)++;
                token->lexeme[i] = (*input)[**pos];
                token->position.pos_end++;
                i++;
            };
            token->lexeme[i] = '\0';
}