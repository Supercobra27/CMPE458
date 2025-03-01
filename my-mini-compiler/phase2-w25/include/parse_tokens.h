#ifndef PARSE_TOKENS_H
#define PARSE_TOKENS_H

typedef enum _parseToken {
    /* Statement */
    PT_STATEMENT,
    PT_DECLARATION,
    PT_EXPR,
    PT_BLOCK,

    /* Keywords */
    PT_PRINT,
    PT_IF_THEN,
    PT_IF_THEN_ELSE,
    PT_WHILE,
    PT_REPEATUNTIL,
    PT_FACTORIAL,

    /* Expressions */
    PT_ASSIGNMENTEX_R12,
    PT_OREX_L11,
    PT_ANDEX_L10,
    PT_BITOREX_L9,
    PT_BITXOREX_L8,
    PT_BITANDEX_L7,
    PT_RELATIONEX_L6,
    PT_SHIFTEX_L5,
    PT_SUMEX_L4,
    PT_PRODUCTEX_L3,
    PT_UNARYPREFIXEX_R2,
    PT_FACTOR,
    PT_IDENTIFIER,
    PT_INTEGER,
    PT_FLOAT

} parseToken;

#endif /* PARSE_TOKENS_H */