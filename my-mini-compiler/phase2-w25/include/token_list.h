#ifndef _LL_H_
#define _LL_H_

#include "tokens.h"

typedef struct _TokenNode {
    Token token;
    struct _TokenNode* next;
} TokenNode;

typedef struct _token_list {
    TokenNode *head;
    size_t size;
}token_list;

token_list* init_tl();
TokenNode* createNode(Token token);
void insertEnd(token_list *list, Token token);
void display(token_list *list);
void deleteNode(TokenNode** head, Token token); //deprecated
TokenNode* search(TokenNode* head, Token token); //deprecated
void free_tl_list(token_list *list);

#endif /* _LL_H_ */