#include<stdio.h>
#include<string.h>

#include "../include/grammar.h"

int main()
{
    CFG_GrammarRule grammar[3] = {
        (CFG_GrammarRule){
            .lhs = PT_PROGRAM,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_SCOPE, PT_EOF, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_SCOPE, AST_SKIP, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_SCOPE,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_STATEMENT_LIST, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_CHILDREN, AST_NULL},
                    .promote_index = -1}},
            .num_rules = 1U},
        (CFG_GrammarRule){
            .lhs = PT_STATEMENT_LIST,
            .rules = (ProductionRule[]){
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_STATEMENT, PT_STATEMENT_LIST, PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_FROM_PROMOTION, AST_FROM_CHILDREN, AST_NULL},
                    .promote_index = -1},
                (ProductionRule){
                    .tokens = (ParseToken[]){PT_NULL},
                    .ast_types = (ASTNodeType[]){AST_NULL},
                    .promote_index = -1}},
            .num_rules = 2U},
    };
    // print the contents of the grammar
    for (size_t i = 0; i < 2; ++i)
    {
        printf("Grammar Rule %lu\n", i);
        printf("LHS: %d\n", grammar[i].lhs);
        printf("Rules: %lu\n", grammar[i].num_rules);
        for (ProductionRule *p = grammar[i].rules; memcmp(p, &(ProductionRule){0}, sizeof(ProductionRule)) != 0; ++p)
        {
            printf("Rule\n");
            printf("Tokens: ");
            for (size_t j = 0; p->tokens[j] != PT_NULL; ++j)
            {
                printf("%d ", p->tokens[j]);
            }
            printf("\n");
            printf("AST Types: ");
            for (size_t j = 0; p->tokens[j] != PT_NULL; ++j)
            {
                printf("%d ", p->ast_types[j]);
            }
            printf("\n");
            printf("Promote Index: %ld\n", p->promote_index);
        }
        // for (size_t j = 0; j < grammar[i].num_rules; ++j)
        // {
        //     printf("Rule %lu\n", j);
        //     printf("Tokens: ");
        //     for (size_t k = 0; grammar[i].rules[j].tokens[k] != PT_NULL; ++k)
        //     {
        //         printf("%d ", grammar[i].rules[j].tokens[k]);
        //     }
        //     printf("\n");
        //     printf("AST Types: ");
        //     for (size_t k = 0; grammar[i].rules[j].tokens[k] != PT_NULL; ++k)
        //     {
        //         printf("%d ", grammar[i].rules[j].ast_types[k]);
        //     }
        //     printf("\n");
        //     printf("Promote Index: %ld\n", grammar[i].rules[j].promote_index);
        // }
    }
}