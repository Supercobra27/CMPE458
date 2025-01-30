#include <operators.h>
#include <tokens.h>
#include <ctype.h>

int isOperator(char c){
    return (c == '+' || c == '-' || c == '*' || c == '/' || c == '~' || c == '|' || c == '&' || c == '^' || c == '=' || c == '!' || c == '>' || c == '<');
}

int isValidPrefix(char c){
    return isOperator(c);
}

int isValidPostfix(char cp, char c){
    return isOperator(cp) && !(c == '<' || c == '>');
}

int isLogicalOperator(char c){
    return (c == '&' || c == '|' || c == '=' || c == '>' || c == '<');
}

int isInvalidOperator(char c){
    return isOperator(c) && !(isLogicalOperator(c));
}

void encapOperator(Token *token, int **pos, char **input, int len){
            int i = 1;
            while ( i != len  && !isInvalidOperator((*input)[**pos])){
                (**pos)++;
                if ((*input)[**pos] == ' ') break; // prevent spaces from being added to the lexeme
                token->lexeme[i] = (*input)[**pos];
                token->position.pos_end++;
                i++;
            };
            token->lexeme[i] = '\0';
}


// For Simon, you can change this around for variable keyword lengths, it worked for me for operators
void encapKeyword(Token *token, int **pos, char **input, int len){
    int i = 1;
            while ( i < len  && !isInvalidOperator((*input)[**pos])){ // change to isInvalidKeyword?

                (**pos)++;
                if ((*input)[**pos] == ' ') break;
                token->lexeme[i] = (*input)[**pos];
                token->position.pos_end++;
                i++;
            };
            token->lexeme[i] = '\0';
}