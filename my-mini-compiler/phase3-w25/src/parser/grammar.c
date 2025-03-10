#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


#include "../../include/grammar.h"


size_t find_direct_left_recursive(const CFG_GrammarRule *rule)
{
    assert(rule != NULL);
    for (size_t i = 0; i < rule->num_rules; ++i)
    {
        if (rule->rules[i].tokens[0] == rule->lhs)
        {
            return i;
        }
    }
    return (size_t)-1;
}


size_t find_indirect_left_recursive(const CFG_GrammarRule grammar[ParseToken_COUNT_NONTERMINAL], const ParseToken lhs_token)
{
    assert(grammar != NULL);
    assert(ParseToken_IS_NONTERMINAL(lhs_token));
    
    // need bool array to keep track of visited non-terminals.
    struct elem
    {
        size_t token;
        size_t rule;
    };
    // can only add each non-terminal at most once to the queue (since we only add unvisited non-terminals, and once visited, they are never visited again).
    // made static to recude stack size. and repeated allocation/deallocation. these data structure values are not retained between function calls.
    static struct elem queue[ParseToken_COUNT_NONTERMINAL];
    static bool visited[ParseToken_COUNT_NONTERMINAL];
    memset(visited, 0, ParseToken_COUNT_NONTERMINAL * sizeof(bool));
    size_t queue_back = 0, queue_front = 0;
    size_t result = (size_t)-1;
    // index of lhs_token in the grammar array.
    const size_t target = lhs_token - ParseToken_FIRST_NONTERMINAL;
    // add all left-most non-terminals from production rules of lhs_token to the queue. So we can keep track of which rules are left-recursive.
    for (size_t r = 0; r < grammar[target].num_rules; ++r)
    {
        if (ParseToken_IS_NONTERMINAL(grammar[target].rules[r].tokens[0]) && !visited[grammar[target].rules[r].tokens[0] - ParseToken_FIRST_NONTERMINAL] && grammar[target].rules[r].tokens[0] != lhs_token)
        {
            queue[queue_back++] = (struct elem){grammar[target].rules[r].tokens[0] - ParseToken_FIRST_NONTERMINAL, r};
            visited[grammar[target].rules[r].tokens[0] - ParseToken_FIRST_NONTERMINAL] = 1;
        }
    }
    // while the queue is not empty and the target is not found, keep adding unvisited left-most non-terminals to the queue.
    while (queue_front < queue_back)
    {
        struct elem e = queue[queue_front++];
        if (e.token == target)
        {
            result = e.rule;
            break;
        }
        for (size_t r = 0; r < grammar[e.token].num_rules; ++r)
        {
            if (ParseToken_IS_NONTERMINAL(grammar[e.token].rules[r].tokens[0]) && !visited[grammar[e.token].rules[r].tokens[0] - ParseToken_FIRST_NONTERMINAL])
            {
                queue[queue_back++] = (struct elem){grammar[e.token].rules[r].tokens[0] - ParseToken_FIRST_NONTERMINAL, e.rule};
                visited[grammar[e.token].rules[r].tokens[0] - ParseToken_FIRST_NONTERMINAL] = 1;
            }
        }
    }
    // free(visited);
    // free(queue);
    return result;
}

#include <stdio.h>
CFG_GrammarCheckResult check_cfg_grammar(const CFG_GrammarRule grammar[ParseToken_COUNT_NONTERMINAL])
{
    CFG_GrammarCheckResult result = {
        .is_prefix_free = true,
        .contains_direct_left_recursion = false,
        .contains_direct_left_recursive_rule_as_last_rule = false,
        .contains_indirect_left_recursion = false,
        .missing_or_mismatched_rules = false,
        .contains_improperly_terminated_production_rules = false
    };
    for (size_t t_index = 0; t_index < ParseToken_COUNT_NONTERMINAL; ++t_index)
    {
        const ParseToken t_expected = t_index + ParseToken_FIRST_NONTERMINAL;
        const ParseToken t = grammar[t_index].lhs;
        printf("Checking grammar rule for %s(%d) (grammar[%llu])\n", parse_token_to_string(t), t, t_index);
        // ensure the grammar has a rule for each non-terminal in the correct index
        if (t != t_expected)
        {
            printf("ERROR: Grammar rule is missing/mismatched: got %s(%d), expected %s(%d)\n", 
                parse_token_to_string(t), t, parse_token_to_string(t_expected), t_expected);
            result.missing_or_mismatched_rules = true;
        }
        // iterate through all production rules to ensure there are no segmenation faults and all rule ParseToken strings are terminated with PT_NULL and all ASTNodeType strings are terminated with AST_NULL
        for (size_t r = 0; r < grammar[t_index].num_rules; ++r)
        {
            const ProductionRule *rule = &grammar[t_index].rules[r];
            size_t j;
            for (j = 0; rule->tokens[j] != PT_NULL && rule->ast_types[j] != AST_NULL; ++j);
            if (rule->tokens[j] != PT_NULL || rule->ast_types[j] != AST_NULL)
            {
                printf("ERROR: Production rule %llu is improperly terminated\n", j);
                result.contains_improperly_terminated_production_rules = true;
            }
        }
        // Check if each production rule is prefix-free (deterministic)
        for (size_t i = 0; i < grammar[t_index].num_rules; ++i)
        {
            for (size_t j = i + 1; j < grammar[t_index].num_rules; ++j)
            {
                if (i != j)
                {
                    const ProductionRule *rule1 = &grammar[t_index].rules[i];
                    const ProductionRule *rule2 = &grammar[t_index].rules[j];
                    if (rule1->tokens[0] == rule2->tokens[0])
                    {
                        printf("ERROR: Not prefix-free: rule %llu overlaps with rule %llu\n", i, j);
                        result.is_prefix_free = false;
                    }
                }
            }
        }

        // check for direct left recursion
        size_t left_recursive_rule_index = find_direct_left_recursive(grammar + t_index);
        if (left_recursive_rule_index != (size_t)-1)
        {
            printf("NOTE: ProductionRule %llu is left recursive\n", left_recursive_rule_index);
            result.contains_direct_left_recursion = true;
            // check that the left recursive rule is not the last rule.
            if (left_recursive_rule_index == grammar[t_index].num_rules-1)
            {
                printf("ERROR: direct left recursive rule as the last rule\n");
                result.contains_direct_left_recursive_rule_as_last_rule = true;
            }
        }
        // check for indirect left recursion
        if (find_indirect_left_recursive(grammar, t) != (size_t)-1)
        {
            printf("ERROR: indirect left recursion for %s(%d)\n", parse_token_to_string(t), t);
            result.contains_indirect_left_recursion = true;
        }
    }
    printf("missing_or_mismatched_rules: %s\n" , result.missing_or_mismatched_rules ? "true" : "false");
    printf("contains_improperly_terminated_production_rules: %s\n" , result.contains_improperly_terminated_production_rules ? "true" : "false");
    printf("is_prefix_free: %s\n" , result.is_prefix_free ? "true" : "false");
    printf("contains_direct_left_recursion: %s\n" , result.contains_direct_left_recursion ? "true" : "false");
    printf("contains_direct_left_recursive_rule_as_last_rule: %s\n", result.contains_direct_left_recursive_rule_as_last_rule ? "true" : "false");
    printf("contains_indirect_left_recursion: %s\n" , result.contains_indirect_left_recursion ? "true" : "false");
    return result;
}