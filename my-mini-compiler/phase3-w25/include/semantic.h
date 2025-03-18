#ifndef _SEMANTIC_H_
#define _SEMANTIC_H_

#include "parser.h"

typedef void (*SemanticAction)(ASTNode *ctx);

typedef struct _SemanticRule {
    const char* name;
    SemanticAction rule; // function pointer to relevant semantic rule
}SemanticRule;

#define SEMANTIC_RULE_COUNT 1

void ProcessOperator(ASTNode *ctx);
void ProcessAssignment(ASTNode *ctx);
void ProcessScope(ASTNode *ctx);
void ProcessDeclaration(ASTNode *ctx);
void ProcessExpression(ASTNode *ctx);
void ProcessConditional(ASTNode *ctx); // keyword expression
void ProcessFunction(ASTNode *ctx);
void ProcessString(ASTNode *ctx);

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