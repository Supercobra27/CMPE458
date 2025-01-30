#include <operators.h>
#include <tokens.h>

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
            do {
                (**pos)++;
                token->lexeme[i] = (*input)[**pos];
                token->position.pos_end++;
                i++;
            } while ( i != len );
            token->lexeme[2] = '\0';
}