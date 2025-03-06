#ifndef OP_H
#define OP_H

#include "tokens.h"

/**
 * @param _Str Input string to check for operator.
 * @return the length of the longest string that matches. 0 if there is no match.
 */
int isOperatorStr(const char *_Str);

/**
 * @param c The character to check.
 * @return 1 the character exists in the set of operators.
 * @details Returns if the character is a valid operator, like addition, logical, or bitwise.
 */
int isOperator(char c);

/**
 * @param _Str Input string.
 * @return The index of the correct operator value.
*/
int findMappableIndex(const char *_Str);

#endif