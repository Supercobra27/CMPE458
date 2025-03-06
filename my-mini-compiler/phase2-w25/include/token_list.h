#ifndef _LL_H_
#define _LL_H_

#include "tokens.h"

typedef struct _TokenNode {
    Token token;
    struct _TokenNode* next;
} TokenNode;

TokenNode* init_tl();
TokenNode* createNode(Token token);
void insertEnd(TokenNode** head, Token token);
void display(TokenNode* head);
void deleteNode(TokenNode** head, Token token);
TokenNode* search(TokenNode* head, Token token);
void free_tl_list(TokenNode** head);

#endif /* _LL_H_ */