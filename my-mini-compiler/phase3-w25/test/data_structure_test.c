#include <stdio.h>
#include <string.h>

#include "../include/grammar.h"
// Define the production rules separately
const ProductionRule program_rules[] = {
    {
        .tokens = (const ParseToken[]){PT_SCOPE, PT_EOF, PT_NULL},
        .ast_types = (const ASTNodeType[]){AST_SCOPE, AST_IGNORE, AST_NULL},
        .promote_index = -1
    }
};

const ProductionRule scope_rules[] = {
    {
        .tokens = (const ParseToken[]){PT_STATEMENT_LIST, PT_NULL},
        .ast_types = (const ASTNodeType[]){AST_FROM_CHILDREN, AST_NULL},
        .promote_index = -1
    }
};

const ProductionRule statement_list_rules[] = {
    {
        .tokens = (const ParseToken[]){PT_STATEMENT, PT_STATEMENT_LIST, PT_NULL},
        .ast_types = (const ASTNodeType[]){AST_FROM_PROMOTION, AST_FROM_CHILDREN, AST_NULL},
        .promote_index = -1
    },
    {
        .tokens = (const ParseToken[]){PT_NULL},
        .ast_types = (const ASTNodeType[]){AST_NULL},
        .promote_index = -1
    }
};

#define sizeof_array(arr) (sizeof(arr) / sizeof(arr[0]))
// Define the grammar rules using the production rules
const CFG_GrammarRule grammar[] = {
    {
        .lhs = PT_PROGRAM,
        .rules = program_rules,
        .num_rules = sizeof_array(program_rules)
    },
    {
        .lhs = PT_SCOPE,
        .rules = scope_rules,
        .num_rules = sizeof(scope_rules)
    },
    {
        .lhs = PT_STATEMENT_LIST,
        .rules = statement_list_rules,
        .num_rules = sizeof(statement_list_rules)
    }
};
int main()
{
    // print the contents of the grammar
    for (size_t i = 0; i < 2; ++i)
    {
        printf("Grammar Rule %llu\n", i);
        printf("LHS: %d\n", grammar[i].lhs);
        printf("Rules: %llu\n", grammar[i].num_rules);
        for (size_t j = 0; j < grammar[i].num_rules; ++j)
        {
            printf("Rule %llu\n", j);
            printf("Tokens: ");
            for (size_t k = 0; grammar[i].rules[j].tokens[k] != PT_NULL; ++k)
            {
                printf("%d ", grammar[i].rules[j].tokens[k]);
            }
            printf("\n");
            printf("AST Types: ");
            for (size_t k = 0; grammar[i].rules[j].tokens[k] != PT_NULL; ++k)
            {
                printf("%d ", grammar[i].rules[j].ast_types[k]);
            }
            printf("\n");
            printf("Promote Index: %llu\n", grammar[i].rules[j].promote_index);
        }
    }
}