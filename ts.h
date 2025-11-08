// ts.h
#ifndef TS_H
#define TS_H

#include "symbol.h"
#include "ast.h"

typedef struct TSNode {
    Symbol* symbol;

    struct TSNode* next;
} TSNode;

typedef struct Level {
    struct TSNode* tsNode;

    int levelNumber;
    struct Level* nextLevel;
} Level;

// Inicializa la tabla de simbolos con el nivel global
Level* initializeTS();

// Retorna el nivel mas alto (actual) de la tabla de simbolos
Level* topLevel(Level* firstLevel);

// Abre un nuevo nivel de alcance en la tabla de simbolos
Level* openNewLevel(Level* firstLevel);

// Cierra el nivel actual de la tabla de simbolos
void closeLevel(Level* firstLevel);

// Retorna el ultimo nodo del nivel actual
TSNode* lastTSNode(Level* symbolTable);

// Inserta un nuevo simbolo en el nivel actual
Symbol* insertSymbol(Level* symbolTable, flagType flag, infoType type, char* name, int value);

// Inserta un parametro en la lista de parametros de un metodo
Symbol* insertParameter(Symbol* method, infoType type, char* name, int value);

// Busca un simbolo por nombre en todos los niveles de alcance
Symbol* getSymbol(Level* symbolTable, char* name);

// Libera toda la memoria de la tabla de simbolos
void freeTS(Level* symbolTable);

// Imprime el contenido de la tabla de simbolos
void printTS(Level* symbolTable);

#endif // TS_H