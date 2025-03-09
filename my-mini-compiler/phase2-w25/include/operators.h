#ifndef OP_H
#define OP_H

#include "tokens.h"

// These operators are ordered in non-increasing length so that they can be checked in order of the longest operator first. Not all of these operators (namely the compound assignment operators) are actually used (see reduced_operators for all operators used in the programming language).
#define STRLEN_MAX_OPERATOR 3
static const char *const extended_operators[] = {
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
    "*",
    "/",
    "%",
    "~",
    "&",
    "|",
    "^",
    "!",
    "<",
    ">"
};

static const char *const reduced_operators[] = {
    // "<<=",
    // ">>=",
    "==",
    "!=",
    ">=",
    "<=",
    // "+=",
    // "-=",
    // "*=",
    // "/=",
    // "|=",
    // "&=",
    // "^=",
    "<<",
    ">>",
    "&&",
    "||",
    // "++",
    // "--",
    "=",
    "+",
    "-",
    "*",
    "/",
    "%",
    "~",
    "&",
    "|",
    "^",
    "!",
    "<",
    ">"
};

/**
 * @param s Input string to check for operator.
 * @return the index of the operator in the reduced_operators array.
 */
int operator_index(const char *const s);

#endif