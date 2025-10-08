#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol.h"

// Crea un nuevo simbolo
Symbol* newSymbol(flagType flag, infoType type, char* name, int value) {
    Symbol* newSym = (Symbol*) malloc(sizeof(Symbol));
    if (!newSym) {
        fprintf(stderr, "Error al asignar memoria para un nuevo simbolo.\n");
        exit(EXIT_FAILURE);
    }
    newSym->flag = flag;
    newSym->type = type;

    // Si existe name, asigna memoria y copia el nombre, si no asigna NULL (para CONST)
    if (name) {
        newSym->name = strdup(name);
        if (!newSym->name) {
            fprintf(stderr, "Error al asignar memoria para el nombre del simbolo.\n");
            free(newSym);
            exit(EXIT_FAILURE);
        }
    } else {
        newSym->name = NULL;
    }

    newSym->value = value;
    newSym->nextParam = NULL;

    return newSym;
}

// Busca un parametro en la lista de parametros de un metodo
Symbol* getParameter(Symbol* method, char* name) {
    if (!method || !name) {
        return NULL;
    }
    
    Symbol* current = method->nextParam;
    while (current) {
        if (current->name && strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->nextParam;
    }
    return NULL;
}

// Agrega al final de la lista de parametros, si no hay ninguno, es el primero, si ya hay, recorre hasta el final y agrega
Symbol* addParameter(Symbol* method, Symbol* param) {
    if (!method || !param) {
        return NULL;
    }
    
    if (!method->nextParam) {
        method->nextParam = param;
    } else {
        Symbol* current = method->nextParam;
        while (current->nextParam) {
            current = current->nextParam;
        }
        current->nextParam = param;
    }

    return param;
}

// Agrega un nuevo parametro a la lista de parametros de un metodo
Symbol* newParameter(Symbol* method, infoType type, char* name, int value) {
    if (!method || !name) {
        return NULL;
    }
    
    if (getParameter(method, name)) {
        fprintf(stderr, "Error: El parametro '%s' ya existe en el metodo '%s'.\n", name, method->name ? method->name : "unknown");
        return NULL;
    }

    Symbol* newParam = newSymbol(PARAMET, type, name, value);

    addParameter(method, newParam);

    return newParam;
}

Symbol* newParameterCall(Symbol* method, Symbol* param) {
    if (!method || !param) {
        return NULL;
    }
    
    if (getParameter(method, param->name)) {
        fprintf(stderr, "Error: El parametro '%s' ya existe en el metodo '%s'.\n", 
                param->name ? param->name : "unknown", 
                method->name ? method->name : "unknown");
        return NULL;
    }

    addParameter(method, param);

    return param;
}

// Libera la memoria de un simbolo (sin liberar parámetros)
void freeSymbol(Symbol* sym) {
    if (!sym) return;

    if (sym->name) {
        free(sym->name);
    }

    // NO liberamos los parámetros aquí - se liberan por separado en el AST
    // ya que también están vinculados al AST

    free(sym);
}

// Libera una lista completa de símbolos enlazados por nextParam
void freeSymbolList(Symbol* list) {
    while (list) {
        Symbol* temp = list;
        list = list->nextParam;
        freeSymbol(temp);
    }
}

// Imprime la informacion de un simbolo
void printSymbol(Symbol* sym) {
    if (!sym) {
        printf("Simbolo nulo.\n");
        return;
    }

    printf("Simbolo:\n");
    printf("  Flag: ");
    switch (sym->flag) {
        case METH:      printf("METH\n"); break;
        case VAR:       printf("VAR\n"); break;
        case PARAMET:   printf("PARAMET\n"); break;
        case CONST:     printf("CONST\n"); break;
        default:        printf("Desconocido\n"); break;
    }

    if (sym->name) {
        printf("  Nombre: %s\n", sym->name);
    }

    // Imprime el tipo y el valor segun el tipo
    printf("  Tipo: ");
    switch (sym->type) {
        case TYPE_VOID: printf("void\n"); break;
        case TYPE_INTEGER:
            printf("integer\n");
            printf("  Valor: %d\n", sym->value);
            break;
        case TYPE_BOOL:
            printf("bool\n");
            printf("  Valor: %s\n", sym->value ? "true" : "false");
            break;
        default: printf("Desconocido\n"); break;
    }

    // Si existen parametros, los imprime
    if (sym->nextParam) {
        printf("  Parametros:\n");
        Symbol* current = sym->nextParam;
        while (current) {
            printSymbol(current);
            current = current->nextParam;
        }
        printf("Fin de parametros.\n\n");
    }
}