// ast.h
#ifndef AST_H
#define AST_H

#include "parser.tab.h"
#include "symbol.h"

typedef struct Node {
    int t_Node;     // Los tipos estan declarados como tokens en parser.y (PROG, VAR_DECL, METHOD_DECL, etc)
                    // TODO: Si se puede, usar tambien los tokens de operadores (T_PLUS, etc) como t_Node

    Symbol* sym;

    struct Node* left;
    struct Node* right;
    struct Node* third;  // Para nodos que necesitan un tercer hijo
} Node;

Node* newNode_Terminal(infoType type, Value value);

Node* newNode_NonTerminal(nodeType type, infoType infType, Value value, 
                         struct Node* left, struct Node* right);
Node* newNode_NonTerminal3(nodeType type, infoType infType, Value value,
                          struct Node* left, struct Node* right, struct Node* third);
Node* newNode_NonTerminal4(nodeType type, infoType infType, Value value,
                          struct Node* left, struct Node* right, 
                          struct Node* third, struct Node* fourth);

void printAST(struct Node* root, int level);

void freeAST(struct Node* root);

#endif // AST_H