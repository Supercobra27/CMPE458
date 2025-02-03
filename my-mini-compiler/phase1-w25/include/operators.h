#ifndef OP_H
#define OP_H

#include <tokens.h>

#define LOGICAL_OPERATOR_LENGTH 2

/**
 * @param _Str Input string to check for operator.
 * @return 1 if the string exists in the set of operators.
*/
int isOperatorStr(const char* _Str);

/**
 * @param c The character to check.
 * @return 1 the character exists in the set of operators.
 * @details Returns if the character is a valid operator, like addition, logical, or bitwise.
 */
int isOperator(char c);

/**
 * @param c The first character.
 * @param cn The following character.
 * @details Useful for checking if a number is a FP.
 */
int isFloatingPrefix(char c, char cn);

/**
 * @param c The character to check.
 * @return 1 if it is a valid character as a prefix to an operator.
 * @note Not required yet.
 * @deprecated
 */
int isValidPrefix(char c);

/**
 * @param c The character to check.
 * @return 1 if it is a valid character as a postfix to an operator.
 * @note Not required yet.
 * @deprecated
 */
int isValidPostfix(char cp, char c);

/**
 * @param c The character to check.
 * @return 1 if the operator is part of the logical set.
 * @details Returns based on if the character is a valid logical like & or |.
 */
int isLogicalOperator(char c);

/**
 * @param c The character to check.
 * @return 1 if the character is not part of the logical/double operator set.
 * @details Returns based on if the character is a valid operator to follow another.
 */
int isInvalidOperator(char c);

/**
 * @param token The token to encapsulate.
 * @param pos The current position of the lexer.
 * @param input The input string of the lexer.
 * @param len The desired length to encapsulate.
 */
void encapOperator(Token *token, int **pos, const char **input, int len);

#endif