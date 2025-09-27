#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ts.h"

// Inicializa la tabla de simbolos vacia con un nivel 0
Level* initializeTS() {
    Level* levelNode = (Level*) malloc(sizeof(Level));
    if (!levelNode) {
        fprintf(stderr, "Error al asignar memoria para la tabla de simbolos.\n");
        exit(EXIT_FAILURE);
    }
    levelNode->tsNode = NULL;

    levelNode->levelNumber = 0;
    levelNode->nextLevel = NULL;
    return levelNode;
}

// Retorna el nivel mas alto de la tabla de simbolos (tope de la pila)
Level* topLevel(Level* firstLevel) {
    Level* current = firstLevel;
    while (current->nextLevel) {
        current = current->nextLevel;
    }
    return current;
}

// Abre un nuevo nivel en la tabla de simbolos y lo enlaza al final de la lista encadenada (funciona como una pila con el tope al final)
Level* openNewLevel(Level* firstLevel) {
    Level* newLevel = (Level*) malloc(sizeof(Level));
    if (!newLevel) {
        fprintf(stderr, "Error al asignar memoria para un nuevo nivel de la tabla de simbolos.\n");
        exit(EXIT_FAILURE);
    }

    Level* top = topLevel(firstLevel);

    newLevel->tsNode = NULL;
    newLevel->levelNumber = top->levelNumber + 1;
    newLevel->nextLevel = NULL;

    top->nextLevel = newLevel;

    return newLevel;
}

// Cierra el nivel mas alto (tope de la pila) de la tabla de simbolos y libera su memoria
void closeLevel(Level* firstLevel) {
    if (!firstLevel) return;

    Level* top = topLevel(firstLevel);

    TSNode* currentTSNode = top->tsNode;
    while (currentTSNode) {
        TSNode* temp = currentTSNode;
        currentTSNode = currentTSNode->next;
        freeSymbol(temp->symbol);
        free(temp);
    }
    free(top);
}

// Retorna el ultimo nodo de un nivel dado
TSNode* lastTSNode(Level* symbolTable) {
    if (!symbolTable || !symbolTable->tsNode) return NULL;

    TSNode* current = symbolTable->tsNode;
    while (current->next) {
        current = current->next;
    }
    return current;
}

// Inserta un nuevo simbolo al final del nivel mas alto de la tabla de simbolos (tope de la pila)
Symbol* insertSymbol(Level* symbolTable, flagType flag, infoType type, char* name, int value) {
    if (getSymbol(symbolTable, name)) {
        fprintf(stderr, "Error: El simbolo '%s' ya existe en la tabla de simbolos.\n", name);
        return NULL;
    } 

    Symbol* newSym = newSymbol(flag, type, name, value);

    TSNode* newTSNode = (TSNode*) malloc(sizeof(TSNode));
    newTSNode->symbol = newSym;
    newTSNode->next = NULL;

    TSNode* last = lastTSNode(symbolTable);
    if (!last) {
        symbolTable->tsNode = newTSNode;
        return newSym;
    }

    last->next = newTSNode;
    return newSym;
}

// Agrega un nuevo parametro al final de la lista de parametros de un metodo dado
Symbol* insertParameter(Symbol* method, infoType type, char* name, int value) {
    if (getParam(method, name)) {
        fprintf(stderr, "Error: El parametro '%s' ya existe en la tabla de simbolos.\n", name);
        return NULL;
    }

    Symbol* newParam = newParameter(method, type, name, value);
    if (!newParam) {
        return NULL;
    }

    return newParam;
}

// Busca un simbolo por nombre en un nivel dado de la tabla de simbolos
Symbol* getSymbol(Level* symbolTable, char* name) {
    TSNode* current = symbolTable->tsNode;

    while (current) {
        if (current->symbol && strcmp(current->symbol->name, name) == 0) {
            return current->symbol;
        }
        current = current->next;
    }
    return NULL; // No encontrado
}

// Libera toda la memoria asociada a la tabla de simbolos
void freeTS(Level* symbolTable) {
    Level* currentLevel = symbolTable;
    while (currentLevel) {
        Level* tempLevel = currentLevel;
        currentLevel = currentLevel->nextLevel;
        freeLevel(tempLevel);
    }
}

// Imprime la tabla de simbolos completa
void printTS(Level* symbolTable) {
    printf("Tabla de Simbolos:\n");
    printf("------------------\n");

    Level* currentLevel = symbolTable;
    while (currentLevel) {
        printf("Nivel %d:\n", currentLevel->levelNumber);

        TSNode* current = currentLevel->tsNode;
        while (current) {
            printSymbol(current->symbol);
            current = current->next;
        }

        currentLevel = currentLevel->nextLevel;
    }

    printf("------------------\n");
}