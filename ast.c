#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol.h"
#include "ast.h"
#include "parser.tab.h"

Node* newNode_Terminal(Symbol* symbol) {
    Node* newNode = (Node*) malloc(sizeof(Node));
    if (!newNode) {
        fprintf(stderr, "Error al asignar memoria para un nuevo nodo terminal.\n");
        exit(EXIT_FAILURE);
    }

    newNode->t_Node = N_TERM;

    newNode->sym = symbol;
    if (!newNode->sym) {
        fprintf(stderr, "Error al crear símbolo para nodo terminal.\n");
        free(newNode);
        exit(EXIT_FAILURE);
    }

    newNode->left = NULL;
    newNode->right = NULL;
    newNode->third = NULL;

    return newNode;
}

Node* newNode_NonTerminal(nodeType type, Symbol* symbol, struct Node* left, struct Node* right, struct Node* third) {
    Node* newNode = (Node*) malloc(sizeof(Node));
    if (!newNode) {
        fprintf(stderr, "Error al asignar memoria para un nuevo nodo no terminal.\n");
        exit(EXIT_FAILURE);
    }

    newNode->t_Node = type;
    
    newNode->sym = symbol; // NULL is allowed for non-terminal nodes

    newNode->left = left;
    newNode->right = right;
    newNode->third = third;

    return newNode;
}

// Función auxiliar para imprimir el prefijo del árbol
void printTreePrefix(int* isLast, int level) {
    for (int i = 0; i < level - 1; i++) {
        if (isLast[i]) {
            printf("    ");
        } else {
            printf("│   ");
        }
    }
    if (level > 0) {
        if (isLast[level - 1]) {
            printf("└── ");
        } else {
            printf("├── ");
        }
    }
}

// Función auxiliar para contar hijos no nulos
int countChildren(Node* node) {
    if (!node) return 0;
    int count = 0;
    if (node->left) count++;
    if (node->right) count++;
    if (node->third) count++;
    return count;
}

void printASTHelper(Node* root, int* isLast, int level) {
    if (!root) return;

    // Imprimir el prefijo del árbol
    printTreePrefix(isLast, level);

    // Imprimir el nodo actual con colores y mejor formato
    switch (root->t_Node) {
        case N_PROG:
            printf("\033[1;36mPROGRAM\033[0m");
            if (root->sym && root->sym->name) {
                printf(" '%s'", root->sym->name);
            }
            printf("\n");
            break;
        case N_VAR_DECL:
            printf("\033[1;35mVAR_DECLARATION\033[0m");
            if (root->sym) {
                printf(" '%s' (%s)", 
                       root->sym->name ? root->sym->name : "unnamed",
                       root->sym->type == TYPE_INTEGER ? "int" : 
                       root->sym->type == TYPE_BOOL ? "bool" : 
                       root->sym->type == TYPE_VOID ? "void" : "unknown");
                if (root->sym->type == TYPE_INTEGER || root->sym->type == TYPE_BOOL) {
                    printf(" = %d", root->sym->value);
                }
            }
            printf("\n");
            break;
        case N_METHOD_DECL:
            printf("\033[1;34mMETHOD_DECLARATION\033[0m");
            if (root->sym) {
                printf(" '%s' -> %s", 
                       root->sym->name ? root->sym->name : "unnamed",
                       root->sym->type == TYPE_INTEGER ? "int" : 
                       root->sym->type == TYPE_BOOL ? "bool" : 
                       root->sym->type == TYPE_VOID ? "void" : "unknown");
            }
            printf("\n");
            break;
        case N_STATEMENT:
            printf("\033[1;33mSTATEMENT\033[0m\n");
            break;
        case N_EXPR:
            printf("\033[1;32mEXPRESSION\033[0m");
            if (root->sym && root->sym->name) {
                printf(" '%s'", root->sym->name);
            }
            printf("\n");
            break;
        case N_ASSIGN:
            printf("\033[1;34mASSIGNMENT\033[0m\n");
            break;
        case N_RETURN:
            printf("\033[1;35mRETURN\033[0m\n");
            break;
        case N_BLOCK:
            printf("\033[1;33mBLOCK\033[0m\n");
            break;
        case N_METHOD_CALL:
            printf("\033[1;36mMETHOD_CALL\033[0m");
            if (root->sym && root->sym->name) {
                printf(" '%s()'", root->sym->name);
            }
            printf("\n");
            break;
        case N_EXTERN:
            printf("\033[1;35mEXTERN\033[0m\n");
            break;
        case N_WHILE:
            printf("\033[1;33mWHILE\033[0m\n");
            break;
        case N_IF:
            printf("\033[1;33mIF\033[0m\n");
            break;
        case N_THEN:
            printf("\033[1;33mTHEN\033[0m\n");
            break;
        case N_ELSE:
            printf("\033[1;33mELSE\033[0m\n");
            break;
        case N_TERM:
            printf("\033[1;94mTERM\033[0m");
            if (root->sym) {
                if (root->sym->flag == CONST) {
                    printf(" %d", root->sym->value);
                } else if (root->sym->name) {
                    printf(" '%s'", root->sym->name);
                }
            }
            printf("\n");
            break;
        case N_PLUS:
            printf("\033[1;33mOPERATOR\033[0m +\n");
            break;
        case N_MINUS:
            printf("\033[1;33mOPERATOR\033[0m -\n");
            break;
        case N_MULT:
            printf("\033[1;33mOPERATOR\033[0m *\n");
            break;
        case N_DIV:
            printf("\033[1;33mOPERATOR\033[0m /\n");
            break;
        case N_MOD:
            printf("\033[1;33mOPERATOR\033[0m %%\n");
            break;
        case N_LESS:
            printf("\033[1;33mOPERATOR\033[0m <\n");
            break;
        case N_GREAT:
            printf("\033[1;33mOPERATOR\033[0m >\n");
            break;
        case N_EQUAL:
            printf("\033[1;33mOPERATOR\033[0m ==\n");
            break;
        case N_OR:
            printf("\033[1;33mOPERATOR\033[0m ||\n");
            break;
        case N_NOT:
            printf("\033[1;33mOPERATOR\033[0m !\n");
            break;
        case N_NEG:
            printf("\033[1;33mOPERATOR\033[0m - (unary)\n");
            break;
        default:
            printf("\033[1;31mUNKNOWN\033[0m [type: %d]\n", root->t_Node);
    }

    // Procesar hijos en orden
    Node* children[3] = {root->left, root->right, root->third};
    int childCount = countChildren(root);
    int currentChild = 0;

    for (int i = 0; i < 3; i++) {
        if (children[i]) {
            currentChild++;
            isLast[level] = (currentChild == childCount);
            printASTHelper(children[i], isLast, level + 1);
        }
    }
}

void printAST(Node* root, int level) {
    if (!root) {
        printf("\033[1;31m(Empty AST)\033[0m\n");
        return;
    }
    
    printf("\n\033[1;36m=== ABSTRACT SYNTAX TREE ===\033[0m\n\n");
    
    int isLast[100] = {0}; // Asumiendo profundidad máxima de 100
    printASTHelper(root, isLast, level);
    
    printf("\n\033[1;36m============================\033[0m\n\n");
}

void freeAST(Node* root) {
    if (!root) return;

    // Liberar los nodos hijos primero
    freeAST(root->left);
    freeAST(root->right);
    freeAST(root->third);

    // Liberar el símbolo constante del nodo (los vinculados a la tabla de símbolos no se liberan)
    if (root->sym && root->sym->flag == CONST) {
        freeSymbol(root->sym);
    }

    // Liberar el nodo
    free(root);
}