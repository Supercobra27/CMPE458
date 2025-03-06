#include <stdio.h>
#include <stdlib.h>
#include "../include/token_list.h"
#include <string.h>

TokenNode* init_tl() {
    return NULL;
}

TokenNode* createNode(Token token){
    TokenNode* newNode = (TokenNode*)malloc(sizeof(TokenNode));
    if (newNode == NULL) {
        printf("Memory allocation failed!\n");
        exit(1);
    }

    newNode->next = NULL;
    newNode->token = token;

    return newNode;
}

void insertEnd(TokenNode** head, Token token){

    TokenNode* insertedNode = createNode(token);

    if (*head == NULL) {
        *head = insertedNode;
    } else {
        TokenNode* temp = *head;

        while(temp->next != NULL) temp = temp->next;

        temp->next = insertedNode;
    }
}

const char *node_to_string(TokenType type)
{
    // need to change to work within ranges or its a huge case statement

    if (type == TOKEN_EOF){
        return "EOF";
    } else if (type == TOKEN_INTEGER_CONST){
        return "INTEGER";
    }else if (type == TOKEN_FLOAT_CONST){
        return "FLOAT";
    }else if (type == TOKEN_STRING_CONST){
        return "STRING_LITERAL";
    }else if (type >= TOKEN_SINGLE_EQUALS && type <= TOKEN_BANG){
        return "OPERATOR";
    }else if (type >= TOKEN_INT_KEYWORD && type <= TOKEN_FACTORIAL_KEYWORD){
        return "KEYWORD";
    }else if (type >= TOKEN_SEMICOLON && type <= TOKEN_RIGHT_PAREN){
        return "PUNCTUATOR";
    }else if (type == TOKEN_ERROR){
        return "ERROR";
    }else if(type == TOKEN_IDENTIFIER){
        return "IDENTIFIER";
    }else {
        return "UNKNOWN";
    }
}

void display(TokenNode* head) {
    TokenNode* temp = head;
    while (temp != NULL) {
        printf("%s -> ", node_to_string(temp->token.type));  // Print the data of the TokenNode
        temp = temp->next;  // Move to the next TokenNode
    }
    printf("NULL\n");
}

// Function to delete a TokenNode with a given value
void deleteTokenNode(TokenNode** head, Token token) {return;}

// Function to search for a TokenNode with a specific value (position indepedent)
TokenNode* search(TokenNode* head, Token token) {
    TokenNode* temp = head;

    while (temp != NULL) {
        if (strcmp(temp->token.lexeme, token.lexeme) == 0) {
            return temp;
        }

        temp = temp->next;
    }
    return NULL;  // TokenNode not found
}

void free_tl_list(TokenNode** head) {
    TokenNode* temp = *head;
    while (temp != NULL) {
        TokenNode* next = temp->next;
        free(temp);
        temp = next;
    }
    *head = NULL;
}