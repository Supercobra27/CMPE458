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

#endif