#ifndef OP_H
#define OP_H

#define LOGICAL_OPERATOR_LENGTH 2

/**
 * @param c The character to check.
 * @return 1 if true.
 * @details Returns if the character is a valid operator, like addition, logical, or bitwise.
*/
int isOperator(char c);

/**
 * @param c The character to check.
 * @return 1 if true.
 * @details Returns based on if the character is a valid logical like & or |.
*/
int isLogicalOperator(char c);

/**
 * @param c The character to check.
 * @return 1.
 * @details Returns based on if the character is a valid operator to follow another.
*/
int isInvalidOperator(char c);


/**
 * @param token The token to encapsulate.
 * @param pos The current position of the lexer.
 * @param input The input string of the lexer.
 * @param len The desired length to encapsulate.
*/
void encapOperator(Token *token, int **pos, char **input, int len);

#endif