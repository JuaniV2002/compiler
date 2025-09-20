#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "parser.tab.h"

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
    newNode->third = NULL;
    newNode->fourth = NULL;

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
    newNode->third = NULL;
    newNode->fourth = NULL;

    return newNode;
}

Node* newNode_NonTerminal3(nodeType type, infoType infType, Value value, Node* left, Node* right, Node* third) {
    Node* newNode = newNode_NonTerminal(type, infType, value, left, right);
    newNode->third = third;
    return newNode;
}

Node* newNode_NonTerminal4(nodeType type, infoType infType, Value value, Node* left, Node* right, Node* third, Node* fourth) {
    Node* newNode = newNode_NonTerminal3(type, infType, value, left, right, third);
    newNode->fourth = fourth;
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
    if (node->fourth) count++;
    return count;
}

void printASTHelper(Node* root, int* isLast, int level) {
    if (!root) return;

    // Imprimir el prefijo del árbol
    printTreePrefix(isLast, level);

    // Imprimir el nodo actual con colores y mejor formato
    switch (root->t_Node) {
        case PROG:   
            printf("\033[1;36mPROGRAM\033[0m\n"); 
            break;
        case DECL:   
            printf("\033[1;35mDECLARATION\033[0m\n"); 
            break;
        case SENT:   
            printf("\033[1;33mSTATEMENT\033[0m\n"); 
            break;
        case EXP:    
            if (root->t_Info == TYPE_BIN_OP && root->info) {
                printf("\033[1;32mEXPRESSION\033[0m ");
                switch(root->info->bin_op) {
                    case T_PLUS: printf("[\033[1;31m+\033[0m]\n"); break;
                    case T_MINUS: printf("[\033[1;31m-\033[0m]\n"); break;
                    case T_MULT: printf("[\033[1;31m*\033[0m]\n"); break;
                    case T_DIVISION: printf("[\033[1;31m/\033[0m]\n"); break;
                    case T_MOD: printf("[\033[1;31m%%\033[0m]\n"); break;
                    case T_LESS: printf("[\033[1;31m<\033[0m]\n"); break;
                    case T_GREATER: printf("[\033[1;31m>\033[0m]\n"); break;
                    case T_EQUAL: printf("[\033[1;31m==\033[0m]\n"); break;
                    case T_AND: printf("[\033[1;31m&&\033[0m]\n"); break;
                    case T_OR: printf("[\033[1;31m||\033[0m]\n"); break;
                    default: printf("[\033[1;31m?\033[0m]\n"); break;
                }
            } else if (root->t_Info == TYPE_UN_OP && root->info) {
                printf("\033[1;32mEXPRESSION\033[0m ");
                switch(root->info->un_op) {
                    case T_UN_MINUS: printf("[\033[1;31munary -\033[0m]\n"); break;
                    case T_UN_NOT: printf("[\033[1;31munary !\033[0m]\n"); break;
                    default: printf("[\033[1;31munary ?\033[0m]\n"); break;
                }
            } else {
                printf("\033[1;32mEXPRESSION\033[0m\n");
            }
            break;
        case ASSIGN: 
            printf("\033[1;34mASSIGNMENT\033[0m\n"); 
            break;
        case RET:    
            printf("\033[1;35mRETURN\033[0m\n"); 
            break;
        case METHOD: 
            printf("\033[1;36mMETHOD\033[0m\n"); 
            break;
        case PARAM:  
            printf("\033[1;37mPARAMETER\033[0m\n"); 
            break;
        case PARAMS: 
            printf("\033[1;37mPARAMETERS\033[0m\n"); 
            break;
        case BLOCK:  
            printf("\033[1;33mBLOCK\033[0m\n"); 
            break;
        case EXPRS:  
            printf("\033[1;32mEXPRESSIONS\033[0m\n"); 
            break;
        case METHOD_CALL: 
            printf("\033[1;36mMETHOD_CALL\033[0m\n"); 
            break;
        case EXTERN: 
            printf("\033[1;35mEXTERN\033[0m\n"); 
            break;
        case WHILE:  
            printf("\033[1;33mWHILE\033[0m\n"); 
            break;
        case IF:     
            printf("\033[1;33mIF\033[0m\n"); 
            break;
        case LITERAL:
            if (root->t_Info == TYPE_INTEGER && root->info) {
                printf("\033[1;92mLITERAL\033[0m [\033[1;96m%d\033[0m]\n", root->info->int_num);
            } else if (root->t_Info == TYPE_BOOL && root->info) {
                printf("\033[1;92mLITERAL\033[0m [\033[1;96m%s\033[0m]\n", 
                       root->info->boolean ? "true" : "false");
            } else {
                printf("\033[1;92mLITERAL\033[0m\n");
            }
            break;
        case TERM:
            if (root->t_Info == TYPE_INTEGER && root->info) {
                printf("\033[1;94mTERM\033[0m [\033[1;96mint: %d\033[0m]\n", root->info->int_num);
            } else if (root->t_Info == TYPE_ID && root->info) {
                printf("\033[1;94mTERM\033[0m [\033[1;93mid: %s\033[0m]\n", root->info->id);
            } else if (root->t_Info == TYPE_BOOL && root->info) {
                printf("\033[1;94mTERM\033[0m [\033[1;96mbool: %s\033[0m]\n", 
                       root->info->boolean ? "true" : "false");
            } else if (root->t_Info == TYPE_VOID) {
                printf("\033[1;94mTERM\033[0m [\033[1;90mvoid\033[0m]\n");
            } else {
                printf("\033[1;94mTERM\033[0m [\033[1;90mtype: %d\033[0m]\n", root->t_Info);
            }
            break;
        default:
            printf("\033[1;31mUNKNOWN\033[0m [type: %d]\n", root->t_Node);
    }

    // Procesar hijos en orden
    Node* children[4] = {root->left, root->right, root->third, root->fourth};
    int childCount = countChildren(root);
    int currentChild = 0;

    for (int i = 0; i < 4; i++) {
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
    printASTHelper(root, isLast, 0);
    
    printf("\n\033[1;36m============================\033[0m\n\n");
}

void freeAST(Node* root) {
    if (!root) return;

    // Liberar los nodos hijos primero
    freeAST(root->left);
    freeAST(root->right);
    freeAST(root->third);
    freeAST(root->fourth);

    // Liberar la información del nodo si existe
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