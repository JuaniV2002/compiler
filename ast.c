#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

Node* newNode_Terminal(infoType type, Value value) {
    Node* newNode = (Node*) malloc(sizeof(Node));
    if (!newNode) {
        fprintf(stderr, "Error al asignar memoria para un nuevo nodo terminal.\n");
        exit(EXIT_FAILURE);
    }

    newNode->t_Node = TERM;
    newNode->t_Info = type;

    Value* valPtr = (Value*) malloc(sizeof(Value));
    if (!valPtr) {
        fprintf(stderr, "Error al asignar memoria para el valor del nodo terminal.\n");
        free(newNode);
        exit(EXIT_FAILURE);
    }
    *valPtr = value;
    newNode->info = valPtr;

    newNode->left = NULL;
    newNode->right = NULL;

    return newNode;
}

Node* newNode_NonTerminal(nodeType type, infoType infType, Value value, Node* left, Node* right) {
    Node* newNode = (Node*) malloc(sizeof(Node));
    if (!newNode) {
        fprintf(stderr, "Error al asignar memoria para un nuevo nodo no terminal.\n");
        exit(EXIT_FAILURE);
    }

    newNode->t_Node = type;
    newNode->t_Info = infType;

    Value* valPtr = (Value*) malloc(sizeof(Value));
    if (!valPtr) {
        fprintf(stderr, "Error al asignar memoria para el valor del nodo no terminal.\n");
        free(newNode);
        exit(EXIT_FAILURE);
    }
    *valPtr = value;
    newNode->info = valPtr;

    newNode->left = left;
    newNode->right = right;

    return newNode;
}

// Funcion auxiliar para imprimir espacios
void printIndent(int level) {
    for (int i = 0; i < level; i++) {
        printf("|    "); // 4 espacios por nivel
    }
}

void printAST(Node* root, int level) {
    if (!root) return;

    // Imprimir el nodo actual
    printIndent(level);
    switch (root->t_Node) {
        case PROG:   printf("PROG\n"); break;
        case DECL:   printf("DECL\n"); break;
        case SENT:   printf("SENT\n"); break;
        case EXP:    printf("EXP, Operador: %c\n\n", root->info->bin_op); break;
        case ASSIGN: printf("ASSIGN\n"); break;
        case RET:    printf("RET\n"); break;
        case TERM:
            if (root->t_Info == TYPE_INT && root->info) {
                printf("TERM, Tipo: INT, Valor: %d\n\n", root->info->int_num);
            } else if (root->t_Info == TYPE_ID && root->info) {
                printf("TERM, Tipo: ID, Nombre: %s\n\n", root->info->id);
            } else if (root->t_Info == TYPE_BOOL && root->info) {
                printf("TERM, Tipo: BOOL, Valor: %s\n\n", root->info->boolean ? "true" : "false");
            } else {
                printf("TERM, Tipo desconocido o info nula\n\n");
            }
            break;
        default:
            printf("Nodo: Tipo desconocido\n");
    }

    printAST(root->left, level + 1);
    printAST(root->right, level + 1);
}

void freeAST(Node* root) {
    if (!root) return;

    // Liberar los nodos hijos primero
    freeAST(root->left);
    freeAST(root->right);

    // Liberar la informacion del nodo si existe
    if (root->info) {
        // Si el tipo es ID, liberar la cadena
        if (root->t_Info == TYPE_ID && root->info->id) {
            free(root->info->id);
        }
        free(root->info);
    }

    // Liberar el nodo
    free(root);
}