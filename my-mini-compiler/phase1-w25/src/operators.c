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