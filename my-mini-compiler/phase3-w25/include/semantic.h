#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "parser.h"

typedef void (*SemanticAction)(ASTNode *ctx);

typedef struct _SemanticRule {
    const char* name;
    SemanticAction rule; // function pointer to relevant semantic rule
}SemanticRule;

#define SEMANTIC_RULE_COUNT 1
#define MAX_ARGS_OPERATOR (size_t)2
#define CHILD_TYPE(node, i) (*(node)).items[i].type
#define CHILD_ITEM(node, i) (*(node)).items[i]
void ProcessOperator(ASTNode *ctx);
void ProcessAssignment(ASTNode *ctx); // Ryan
void ProcessDeclaration(ASTNode *ctx); // Simon
void ProcessExpression(ASTNode *ctx); // Ryan
void ProcessConditional(ASTNode *ctx); // Ryan keyword expression
void ProcessFunction(ASTNode *ctx); // Simon
void ProcessScope(ASTNode *ctx); // Simon
void ProcessString(ASTNode *ctx); // Simon
void ProcessProgram(ASTNode *ctx);
void ProcessNode(ASTNode *ctx);
/*
Need to figure out where to put scope stuff (most likely in symbol table)
*/

// static const SemanticRule semantics[SEMANTIC_RULE_COUNT] = {
//     {
//         .name = "Test",
//         .rule = ProcessOperator,
//     }
// };

#endif /* SEMANTIC_H */