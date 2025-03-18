#ifndef _SEMANTIC_H_
#define _SEMANTIC_H_

#include "parser.h"

typedef void (*SemanticAction)(ASTNode *ctx);

typedef struct _SemanticRule {
    const char* name;
    SemanticAction rule; // function pointer to relevant semantic rule
}SemanticRule;

#define SEMANTIC_RULE_COUNT 1

void ProcessOperator(ASTNode *ctx); // Ryan
void ProcessAssignment(ASTNode *ctx); // Ryan
void ProcessDeclaration(ASTNode *ctx); // Simon
void ProcessExpression(ASTNode *ctx); // Ryan
void ProcessConditional(ASTNode *ctx); // Ryan keyword expression
void ProcessFunction(ASTNode *ctx); // Simon
void ProcessScope(ASTNode *ctx); // Simon
void ProcessString(ASTNode *ctx); // Simon

/*
Need to figure out where to put scope stuff (most likely in symbol table)
*/

static const SemanticRule semantics[SEMANTIC_RULE_COUNT] = {
    {
        .name = "Test",
        .rule = ProcessOperator,
    }
};

#endif _SEMANTIC_H_