#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../../include/grammar.h"

size_t is_direct_left_recursive(const CFG_GrammarRule *rule)
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


size_t is_indirect_left_recursive(const CFG_GrammarRule *grammar, const size_t grammar_size, const ParseToken lhs_token)
{
    assert(grammar != NULL);
    assert(grammar_size > 0);
    assert(ParseToken_IS_NONTERMINAL(lhs_token));
    // need bool array to keep track of visited non-terminals.
    struct elem
    {
        size_t token;
        size_t rule;
    };
    // can only add each non-terminal at most once to the queue (since we only add unvisited non-terminals, and once visited, they are never visited again).
    struct elem queue[grammar_size];
    bool visited[grammar_size];
    memset(visited, 0, grammar_size * sizeof(bool));
    // struct elem *queue = malloc(grammar_size * sizeof(struct elem));
    // bool *visited = calloc(grammar_size, sizeof(bool));
    size_t queue_back = 0, queue_front = 0;
    size_t result = (size_t)-1;
    // index of lhs_token in the grammar array.
    const size_t target = lhs_token - ParseToken_FIRST_NONTERMINAL;
    // add all left-most non-terminals from production rules of lhs_token to the queue. So we can keep track of which rules are left-recursive.
    for (size_t i = 0; i < grammar[target].num_rules; ++i)
    {
        if (ParseToken_IS_NONTERMINAL(grammar[target].rules[i].tokens[0]) && !visited[grammar[target].rules[i].tokens[0] - ParseToken_FIRST_NONTERMINAL] && grammar[target].rules[i].tokens[0] != lhs_token)
        {
            queue[queue_back++] = (struct elem){grammar[target].rules[i].tokens[0] - ParseToken_FIRST_NONTERMINAL, i};
            visited[grammar[target].rules[i].tokens[0] - ParseToken_FIRST_NONTERMINAL] = 1;
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
        for (size_t i = 0; i < grammar[e.token].num_rules; ++i)
        {
            if (ParseToken_IS_NONTERMINAL(grammar[e.token].rules[i].tokens[0]) && !visited[grammar[e.token].rules[i].tokens[0] - ParseToken_FIRST_NONTERMINAL])
            {
                queue[queue_back++] = (struct elem){grammar[e.token].rules[i].tokens[0] - ParseToken_FIRST_NONTERMINAL, e.rule};
                visited[grammar[e.token].rules[i].tokens[0] - ParseToken_FIRST_NONTERMINAL] = 1;
            }
        }
    }
    // free(visited);
    // free(queue);
    return result;
}
