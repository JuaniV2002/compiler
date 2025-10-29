#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol.h"
#include "ast.h"
#include "tac.h"

TacCode* initTAC() {
    TacCode* tac = (TacCode*) malloc(sizeof(TacCode));
    if (!tac) {
        fprintf(stderr, "Error al asignar memoria para TAC.\n");
        exit(EXIT_FAILURE);
    }
    tac->head = NULL;
    tac->tail = NULL;
    tac->tempCount = 0;
    tac->labelCount = 0;
    return tac;
}

char* newTemp(TacCode* tac) {
    char* temp = (char*) malloc(16);
    sprintf(temp, "t%d", tac->tempCount++);
    return temp;
}

char* newLabel(TacCode* tac) {
    char* label = (char*) malloc(16);
    sprintf(label, "L%d", tac->labelCount++);
    return label;
}

void emitTAC(TacCode* tac, TacOp op, char* dest, char* arg1, char* arg2) {
    TacInstr* instr = (TacInstr*) malloc(sizeof(TacInstr));
    if (!instr) {
        fprintf(stderr, "Error al asignar memoria para instruccion TAC.\n");
        exit(EXIT_FAILURE);
    }
    
    instr->op = op;
    instr->dest = dest ? strdup(dest) : NULL;
    instr->arg1 = arg1 ? strdup(arg1) : NULL;
    instr->arg2 = arg2 ? strdup(arg2) : NULL;
    instr->next = NULL;
    
    if (!tac->head) {
        tac->head = instr;
        tac->tail = instr;
    } else {
        tac->tail->next = instr;
        tac->tail = instr;
    }
}

char* generateExpr(Node* node, TacCode* tac) {
    if (!node) return NULL;
    
    switch (node->t_Node) {
        case N_TERM:
            if (node->sym) {
                if (node->sym->flag == CONST) {
                    char* val = (char*) malloc(16);
                    sprintf(val, "%d", node->sym->value);
                    return val;
                } else if (node->sym->name) {
                    return strdup(node->sym->name);
                }
            }
            return NULL;
            
        case N_EXPR:
            if (node->sym && node->sym->name) {
                return strdup(node->sym->name);
            }
            return generateExpr(node->left, tac);
            
        case N_PLUS: case N_MINUS: case N_MULT: case N_DIV: case N_MOD:
        case N_LESS: case N_GREAT: case N_EQUAL:
        case N_AND: case N_OR: {
            char* left = generateExpr(node->left, tac);
            char* right = generateExpr(node->right, tac);
            char* temp = newTemp(tac);
            
            TacOp op;
            switch (node->t_Node) {
                case N_PLUS:  op = TAC_ADD; break;
                case N_MINUS: op = TAC_SUB; break;
                case N_MULT:  op = TAC_MUL; break;
                case N_DIV:   op = TAC_DIV; break;
                case N_MOD:   op = TAC_MOD; break;
                case N_LESS:  op = TAC_LT; break;
                case N_GREAT: op = TAC_GT; break;
                case N_EQUAL: op = TAC_EQ; break;
                case N_AND:   op = TAC_AND; break;
                case N_OR:    op = TAC_OR; break;
                default: op = TAC_ADD;
            }
            
            emitTAC(tac, op, temp, left, right);
            free(left);
            free(right);
            return temp;
        }
        
        case N_NOT: case N_NEG: {
            char* arg = generateExpr(node->left, tac);
            char* temp = newTemp(tac);
            TacOp op = (node->t_Node == N_NOT) ? TAC_NOT : TAC_NEG;
            emitTAC(tac, op, temp, arg, NULL);
            free(arg);
            return temp;
        }
        
        case N_METHOD_CALL:
            if (node->sym && node->sym->name) {
                // Generar parámetros en orden
                Node* args = node->left;
                if (args) {
                    // Si args tiene left y right no nulos, es una lista de expresiones
                    if (args->t_Node == N_EXPR && args->left && args->right) {
                        // Caso con múltiples argumentos: recorrer lista
                        while (args && args->t_Node == N_EXPR && args->left) {
                            if (args->right) {
                                char* argVal = generateExpr(args->right, tac);
                                if (argVal) {
                                    emitTAC(tac, TAC_PARAM, NULL, argVal, NULL);
                                    free(argVal);
                                }
                            }
                            args = args->left;
                        }
                        // El último argumento
                        if (args) {
                            char* argVal = generateExpr(args, tac);
                            if (argVal) {
                                emitTAC(tac, TAC_PARAM, NULL, argVal, NULL);
                                free(argVal);
                            }
                        }
                    } else {
                        // Caso con un solo argumento o EXPRESSION simple
                        char* argVal = generateExpr(args, tac);
                        if (argVal) {
                            emitTAC(tac, TAC_PARAM, NULL, argVal, NULL);
                            free(argVal);
                        }
                    }
                }
                
                char* temp = newTemp(tac);
                emitTAC(tac, TAC_CALL, temp, node->sym->name, NULL);
                return temp;
            }
            return NULL;
            
        default:
            return NULL;
    }
}

void generateStmt(Node* node, TacCode* tac) {
    if (!node) return;
    
    switch (node->t_Node) {
        case N_VAR_DECL:
            // Generar asignación para declaración de variables
            if (node->sym && node->sym->name) {
                // Por ahora no hacemos nada, las variables se inicializan en tiempo de ejecución
                // Si tuvieran una expresión de inicialización, se generaría aquí
            }
            // Si hay más declaraciones (encadenadas), procesarlas
            if (node->left) {
                generateStmt(node->left, tac);
            }
            break;
            
        case N_STATEMENT:
            generateStmt(node->left, tac);
            generateStmt(node->right, tac);
            break;
            
        case N_ASSIGN: {
            char* rval = generateExpr(node->right, tac);
            if (node->left && node->left->sym && node->left->sym->name) {
                emitTAC(tac, TAC_COPY, node->left->sym->name, rval, NULL);
            }
            if (rval) free(rval);
            break;
        }
        
        case N_IF: {
            char* cond = generateExpr(node->left, tac);
            char* labelElse = newLabel(tac);
            char* labelEnd = newLabel(tac);
            
            emitTAC(tac, TAC_IF_FALSE, NULL, cond, labelElse);
            free(cond);
            
            generateStmt(node->right, tac);
            
            if (node->third) {
                emitTAC(tac, TAC_GOTO, labelEnd, NULL, NULL);
                emitTAC(tac, TAC_LABEL, labelElse, NULL, NULL);
                generateStmt(node->third, tac);
                emitTAC(tac, TAC_LABEL, labelEnd, NULL, NULL);
            } else {
                emitTAC(tac, TAC_LABEL, labelElse, NULL, NULL);
            }
            
            free(labelElse);
            free(labelEnd);
            break;
        }
        
        case N_WHILE: {
            char* labelStart = newLabel(tac);
            char* labelEnd = newLabel(tac);
            
            emitTAC(tac, TAC_LABEL, labelStart, NULL, NULL);
            
            char* cond = generateExpr(node->left, tac);
            emitTAC(tac, TAC_IF_FALSE, NULL, cond, labelEnd);
            free(cond);
            
            generateStmt(node->right, tac);
            emitTAC(tac, TAC_GOTO, labelStart, NULL, NULL);
            emitTAC(tac, TAC_LABEL, labelEnd, NULL, NULL);
            
            free(labelStart);
            free(labelEnd);
            break;
        }
        
        case N_RETURN: {
            if (node->left) {
                char* retVal = generateExpr(node->left, tac);
                emitTAC(tac, TAC_RETURN, NULL, retVal, NULL);
                if (retVal) free(retVal);
            } else {
                emitTAC(tac, TAC_RETURN, NULL, NULL, NULL);
            }
            break;
        }
        
        case N_BLOCK:
            generateStmt(node->right, tac);
            break;
            
        case N_THEN:
        case N_ELSE:
            // Los nodos THEN y ELSE son intermediarios que contienen bloques
            // Generar el código del bloque hijo
            if (node->left) {
                generateStmt(node->left, tac);
            }
            break;
            
        case N_METHOD_CALL: {
            char* result = generateExpr(node, tac);
            if (result) free(result);
            break;
        }
            
        default:
            break;
    }
}

void generateMethod(Node* node, TacCode* tac) {
    if (!node) return;
    
    if (node->t_Node == N_METHOD_DECL) {
        // Si tiene un hijo izquierdo que es METHOD_DECL, procesarlo primero (recursion)
        if (node->left && node->left->t_Node == N_METHOD_DECL) {
            generateMethod(node->left, tac);
        }
        
        // Procesar el método actual (hijo derecho cuando hay recursion, o el nodo mismo)
        Node* methodNode = (node->left && node->left->t_Node == N_METHOD_DECL) ? node->right : node;
        
        if (methodNode && methodNode->t_Node == N_METHOD_DECL && methodNode->sym && methodNode->sym->name) {
            // Solo generar código para métodos que no son extern
            if (methodNode->left && methodNode->left->t_Node != N_EXTERN) {
                emitTAC(tac, TAC_BEGIN_FUNC, methodNode->sym->name, NULL, NULL);
                
                if (methodNode->left->t_Node == N_BLOCK) {
                    // Generar código para variables declaradas en el bloque
                    if (methodNode->left->left) {
                        generateStmt(methodNode->left->left, tac);
                    }
                    // Generar código para statements
                    if (methodNode->left->right) {
                        generateStmt(methodNode->left->right, tac);
                    }
                } else {
                    generateStmt(methodNode->left, tac);
                }
                
                emitTAC(tac, TAC_END_FUNC, methodNode->sym->name, NULL, NULL);
            }
        }
    }
}

void generateTAC(Node* root, TacCode* tac) {
    if (!root) return;
    
    if (root->t_Node == N_PROG) {
        // METHOD_DECLS está en right (según la gramática)
        if (root->right && root->right->t_Node == N_METHOD_DECL) {
            generateMethod(root->right, tac);
        }
    }
}

void printTAC(TacCode* tac) {
    if (!tac) return;

    printf("\n\033[1;36m=== THREE ADDRESS CODE ===\033[0m\n\n");

    TacInstr* instr = tac->head;
    int indent = 0;
    
    while (instr) {
        // Adjust indent for function boundaries
        if (instr->op == TAC_END_FUNC) {
            indent = 0;
        }
        
        // Print indent
        for (int i = 0; i < indent; i++) {
            printf("  ");
        }
        
        switch (instr->op) {
            case TAC_ADD:
                printf("\033[1;32m%s\033[0m = %s \033[1;33m+\033[0m %s\n", instr->dest, instr->arg1, instr->arg2);
                break;
            case TAC_SUB:
                printf("\033[1;32m%s\033[0m = %s \033[1;33m-\033[0m %s\n", instr->dest, instr->arg1, instr->arg2);
                break;
            case TAC_MUL:
                printf("\033[1;32m%s\033[0m = %s \033[1;33m*\033[0m %s\n", instr->dest, instr->arg1, instr->arg2);
                break;
            case TAC_DIV:
                printf("\033[1;32m%s\033[0m = %s \033[1;33m/\033[0m %s\n", instr->dest, instr->arg1, instr->arg2);
                break;
            case TAC_MOD:
                printf("\033[1;32m%s\033[0m = %s \033[1;33m%%\033[0m %s\n", instr->dest, instr->arg1, instr->arg2);
                break;
            case TAC_LT:
                printf("\033[1;32m%s\033[0m = %s \033[1;33m<\033[0m %s\n", instr->dest, instr->arg1, instr->arg2);
                break;
            case TAC_GT:
                printf("\033[1;32m%s\033[0m = %s \033[1;33m>\033[0m %s\n", instr->dest, instr->arg1, instr->arg2);
                break;
            case TAC_EQ:
                printf("\033[1;32m%s\033[0m = %s \033[1;33m==\033[0m %s\n", instr->dest, instr->arg1, instr->arg2);
                break;
            case TAC_AND:
                printf("\033[1;32m%s\033[0m = %s \033[1;33m&&\033[0m %s\n", instr->dest, instr->arg1, instr->arg2);
                break;
            case TAC_OR:
                printf("\033[1;32m%s\033[0m = %s \033[1;33m||\033[0m %s\n", instr->dest, instr->arg1, instr->arg2);
                break;
            case TAC_NOT:
                printf("\033[1;32m%s\033[0m = \033[1;33m!\033[0m%s\n", instr->dest, instr->arg1);
                break;
            case TAC_NEG:
                printf("\033[1;32m%s\033[0m = \033[1;33m-\033[0m%s\n", instr->dest, instr->arg1);
                break;
            case TAC_COPY:
                printf("\033[1;35m%s\033[0m = %s\n", instr->dest, instr->arg1);
                break;
            case TAC_LABEL:
                printf("\033[1;36m%s:\033[0m\n", instr->dest);
                break;
            case TAC_GOTO:
                printf("\033[1;33mgoto\033[0m %s\n", instr->dest);
                break;
            case TAC_IF_FALSE:
                printf("\033[1;33mif\033[0m !%s \033[1;33mgoto\033[0m %s\n", instr->arg1, instr->arg2);
                break;
            case TAC_PARAM:
                printf("\033[1;33mparam\033[0m %s\n", instr->arg1);
                break;
            case TAC_CALL:
                printf("\033[1;32m%s\033[0m = \033[1;33mcall\033[0m \033[1;34m%s\033[0m\n", instr->dest, instr->arg1);
                break;
            case TAC_RETURN:
                if (instr->arg1) {
                    printf("\033[1;33mreturn\033[0m %s\n", instr->arg1);
                } else {
                    printf("\033[1;33mreturn\033[0m\n");
                }
                break;
            case TAC_BEGIN_FUNC:
                printf("\033[1;34m┌─ func %s\033[0m\n", instr->dest);
                indent = 1;
                break;
            case TAC_END_FUNC:
                printf("\033[1;34m└─ endfunc %s\033[0m\n", instr->dest);
                break;
            default:
                break;
        }
        instr = instr->next;
    }

    printf("\n\033[1;36m==========================\033[0m\n\n");
}

void freeTAC(TacCode* tac) {
    if (!tac) return;
    
    TacInstr* instr = tac->head;
    while (instr) {
        TacInstr* temp = instr;
        instr = instr->next;
        
        if (temp->dest) free(temp->dest);
        if (temp->arg1) free(temp->arg1);
        if (temp->arg2) free(temp->arg2);
        free(temp);
    }
    
    free(tac);
}
