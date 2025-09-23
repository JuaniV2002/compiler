#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ts.h"

Level* initializeTS() {
    Level* levelNode = (Level*) malloc(sizeof(Level));
    if (!levelNode) {
        fprintf(stderr, "Error al asignar memoria para la tabla de simbolos.\n");
        exit(EXIT_FAILURE);
    }
    levelNode->tsNode = NULL;

    levelNode->level = 0;
    levelNode->nextLevel = NULL;
    return levelNode;
}

TSNode* insertTSNode(Level* symbolTable, flagType flag, Symbol* symbol, Method* method) {
    TSNode* newTSNode = (TSNode*) malloc(sizeof(TSNode));
    if (!newTSNode) {
        fprintf(stderr, "Error al asignar memoria para una nueva entrada en la tabla de simbolos.\n");
        exit(EXIT_FAILURE);
    }
    newTSNode->flag = flag;

    newTSNode->sm = (SymbolOrMethod*) malloc(sizeof(SymbolOrMethod));
    if (!newTSNode->sm) {
        fprintf(stderr, "Error al asignar memoria para SymbolOrMethod.\n");
        free(newTSNode);
        exit(EXIT_FAILURE);
    }
    if (flag == METH) {
        newTSNode->sm->method = method;
    } else {
        newTSNode->sm->symbol = symbol;
    }

    newTSNode->next = NULL;

    if (symbolTable->tsNode == NULL) {
        symbolTable->tsNode = newTSNode;
    } else {
        TSNode* current = symbolTable->tsNode;
        while (current->next) {
            current = current->next;
        }
        current->next = newTSNode;
    }

    return newTSNode;
}

Symbol* insertSymbol(Level* symbolTable, char* name, infoType type, Value* value) {
    if (getSymbol(symbolTable, name)) {
        fprintf(stderr, "Error: El simbolo '%s' ya existe en la tabla de simbolos.\n", name);
        return NULL;
    }

    Symbol* newSymbol = (Symbol*) malloc(sizeof(Symbol));
    if (!newSymbol) {
        fprintf(stderr, "Error al asignar memoria para un nuevo simbolo.\n");
        exit(EXIT_FAILURE);
    }
    newSymbol->name = strdup(name);
    newSymbol->type = type;
    newSymbol->value = value;

    TSNode* newTSNode = insertTSNode(symbolTable, VAR, newSymbol, NULL);
    newTSNode->sm->symbol = newSymbol;

    TSNode* current = symbolTable->tsNode;
    while (current->next) {
        current = current->next;
    }

    newTSNode->next = current->next;
    current->next = newTSNode;

    return newSymbol;
}

Method* insertMethod(Level* symbolTable, char* name, infoType returnType) {
    if (getMethod(symbolTable, name)) {
        fprintf(stderr, "Error: El metodo '%s' ya existe en la tabla de simbolos.\n", name);
        return NULL;
    }

    Method* newMethod = (Method*) malloc(sizeof(Method));
    if (!newMethod) {
        fprintf(stderr, "Error al asignar memoria para un nuevo metodo.\n");
        exit(EXIT_FAILURE);
    }
    newMethod->name = strdup(name);
    newMethod->returnType = returnType;
    newMethod->paramList = NULL;

    TSNode* newTSNode = insertTSNode(symbolTable, METH, NULL, newMethod);
    newTSNode->sm->method = newMethod;

    TSNode* current = symbolTable->tsNode;
    while (current->next) {
        current = current->next;
    }

    newTSNode->next = current->next;
    current->next = newTSNode;

    return newMethod;
}

ParamNode* insertParam(Method* method, char* name, infoType type) {
    if (getParam(method, name)) {
        fprintf(stderr, "Error: El parametro '%s' ya existe en la tabla de simbolos.\n", name);
        return NULL;
    }

    Symbol* newParamSymbol = (Symbol*) malloc(sizeof(Symbol));
    if (!newParamSymbol) {
        fprintf(stderr, "Error al asignar memoria para un nuevo simbolo de parametro.\n");
        exit(EXIT_FAILURE);
    }

    newParamSymbol->name = strdup(name);
    newParamSymbol->type = type;
    newParamSymbol->value = NULL; // Los parametros no tienen valor inicial

    ParamNode* newParam = (ParamNode*) malloc(sizeof(ParamNode));
    if (!newParam) {
        fprintf(stderr, "Error al asignar memoria para un nuevo parametro.\n");
        exit(EXIT_FAILURE);
    }
    newParam->flag = PARAMET;
    newParam->param = newParamSymbol;
    newParam->next = NULL;

    if (!method->paramList) {
        method->paramList = newParam;
    } else {
        ParamNode* current = method->paramList;
        while (current->next) {
            current = current->next;
        }
        current->next = newParam;
    }

    return newParam;
}

Symbol* getParam(Method* method, char* name) {
    ParamNode* current = method->paramList;
    while (current) {
        if (strcmp(current->param->name, name) == 0) {
            return current->param;
        }
        current = current->next;
    }
    return NULL; // No encontrado
}

Symbol* getSymbol(Level* symbolTable, char* name) {
    TSNode* current = symbolTable->tsNode;

    while (current) {
        if (current->sm->symbol && strcmp(current->sm->symbol->name, name) == 0) {
            return current->sm->symbol;
        }
        current = current->next;
    }
    return NULL; // No encontrado
}

Method* getMethod(Level* symbolTable, char* name) {
    TSNode* current = symbolTable->tsNode;

    while (current) {
        if (current->sm->method && strcmp(current->sm->method->name, name) == 0) {
            return current->sm->method;
        }
        current = current->next;
    }
    return NULL; // No encontrado
}

void printTS(Level* symbolTable) {
    Level* currentLevel = symbolTable;
    printf("Tabla de Simbolos:\n");
    printf("------------------\n");
    while (currentLevel) {
        TSNode* current = currentLevel->tsNode;
        printf("Nivel %d:\n", currentLevel->level);

        while (current) {
            printf("Flag: %s\n", current->flag == METH ? "METH" : "VAR");

            if (current->flag == VAR && current->sm->symbol) {
                printf("Nombre: %s, Tipo: ", current->sm->symbol->name);
                switch (current->sm->symbol->type) {
                    case TYPE_VOID: printf("void\n\n"); break;
                    case TYPE_INTEGER: printf("int, Valor: %i\n\n", current->sm->symbol->value->int_num); break;
                    case TYPE_BOOL: printf("bool, Valor: %s\n\n", (current->sm->symbol->value->boolean) ? "true" : "false"); break;
                    default: printf("none\n\n"); break;
                }
            } else {
                printf("Nombre: %s, Tipo de retorno: ", current->sm->method->name);
                switch (current->sm->method->returnType) {
                    case TYPE_VOID: printf("void\n"); break;
                    case TYPE_INTEGER: printf("int\n"); break;
                    case TYPE_BOOL: printf("bool\n"); break;
                    default: printf("none\n"); break;
                }
                printf("Parametros:\n");
                ParamNode* paramCurrent = current->sm->method->paramList;
                while (paramCurrent) {
                    printf("  - Nombre: %s, Tipo: ", paramCurrent->param->name);
                    switch (paramCurrent->param->type) {
                        case TYPE_VOID: printf("void\n"); break;
                        case TYPE_INTEGER: printf("int\n"); break;
                        case TYPE_BOOL: printf("bool\n"); break;
                        default: printf("none\n"); break;
                    }
                    paramCurrent = paramCurrent->next;
                }
            }
            current = current->next;
        }
        currentLevel = currentLevel->nextLevel;
    }
    printf("------------------\n");
}