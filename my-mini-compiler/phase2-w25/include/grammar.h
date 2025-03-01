#ifndef GRAMMAR_H
#define GRAMMAR_H
#include "tokens.h"
#include "parser.h"
#include <stddef.h>

typedef struct _Symbol {
    union {
        ASTNodeType non_terminal;
        TokenType terminal;
    } symbol;
    int is_terminal;
} Symbol;

typedef struct _ProductionRule {
    Symbol* symbols;
    size_t num_symbols;
} ProductionRule;


typedef struct _GrammarRule {
    ProductionRule* rhs;
    size_t num_productions;
} GrammarRule;

// Grammar rules
// Array of Null-terminated arrays of Symbols
// Each array of Symbols represents a rule in the grammar
// static const * grammar[] = {};


#endif /* GRAMMAR_H */