// ast.h
#ifndef AST_H
#define AST_H

#include "parser.tab.h"
#include "symbol.h"

typedef enum { N_PROG, N_VAR_DECL, N_METHOD_DECL, N_BLOCK, N_EXTERN, N_STATEMENT, N_ASSIGN, N_METHOD_CALL, N_EXPR, N_PLUS,
                N_MINUS, N_MULT, N_DIV, N_MOD, N_LESS, N_GREAT, N_EQUAL, N_AND, N_OR, N_NOT, N_NEG, N_IF, N_THEN, N_ELSE, N_WHILE, N_RETURN, N_TERM } nodeType;

typedef struct Node {
    nodeType t_Node;

    Symbol* sym;

    struct Node* left;
    struct Node* right;
    struct Node* third;  // Para nodos que necesitan un tercer hijo
} Node;

// Crea un nodo terminal del AST con un simbolo
Node* newNode_Terminal(Symbol* symbol);

// Crea un nodo no terminal con tipo, simbolo e hijos especificados
Node* newNode_NonTerminal(nodeType type, Symbol* symbol, struct Node* left, struct Node* right, struct Node* third);

// Imprime el arbol AST de forma jerarquica
void printAST(struct Node* root, int level);

// Libera toda la memoria del AST
void freeAST(struct Node* root);

#endif // AST_H