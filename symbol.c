#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol.h"

// Crea un nuevo simbolo
Symbol* newSymbol(flagType flag, infoType type, char* name, int value, Symbol* nextParam) {
    Symbol* newSym = (Symbol*) malloc(sizeof(Symbol));
    if (!newSym) {
        fprintf(stderr, "Error al asignar memoria para un nuevo simbolo.\n");
        exit(EXIT_FAILURE);
    }
    newSym->flag = flag;
    newSym->type = type;

    // Si existe el nombre, asigna memoria y copia el nombre, si no asigna NULL (para CONST)
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
    newSym->nextParam = nextParam;

    return newSym;
}

// Busca un parametro en la lista de parametros de un metodo
Symbol* getParameter(Symbol* method, char* name) {
    Symbol* current = method->nextParam;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->nextParam;
    }
    return NULL;    // No encontrado
}

// Agrega un nuevo parametro a la lista de parametros de un metodo
Symbol* newParameter(Symbol* method, infoType type, char* name, int value) {
    if (getParameter(method, name)) {
        fprintf(stderr, "Error: El parametro '%s' ya existe en el metodo '%s'.\n", name, method->name);
        return NULL;
    }

    Symbol* newParam = newSymbol(PARAMET, type, name, value, NULL);

    // Agrega al final de la lista de parametros, si no hay ninguno, es el primero, si ya hay, recorre hasta el final y agrega
    if (!method->nextParam) {
        method->nextParam = newParam;
    } else {
        Symbol* current = method->nextParam;
        while (current->nextParam) {
            current = current->nextParam;
        }
        current->nextParam = newParam;
    }

    return newParam;
}

// Libera la memoria de un simbolo y su lista de parametros
void freeSymbol(Symbol* sym) {
    if (!sym) return;

    if (sym->name) {
        free(sym->name);
    }

    // Si tiene parametros, libera la lista completa
    Symbol* current = sym->nextParam;
    while (current) {
        Symbol* temp = current;
        current = current->nextParam;
        freeSymbol(temp);
    }

    free(sym);
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
    }
}