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

typedef struct Stack {
    struct Level* top;
} Stack;

// Inicializa la tabla de simbolos con el nivel global
Stack* initializeTS();

// Abre un nuevo nivel de alcance en la tabla de simbolos
void openNewLevel(Stack* stack);

// Abre un nuevo nivel de alcance y asocia el nivel con el simbolo del metodo dado
void openNewLevelMethod(Stack* stack, Symbol* methodSymbol);

// Cierra el nivel actual de la tabla de simbolos
void closeLevel(Stack* stack);

// Inserta un nuevo simbolo en el nivel actual
Symbol* insertSymbol(Stack* stack, flagType flag, infoType type, char* name, int value);

// Inserta un parametro en la lista de parametros de un metodo
Symbol* insertParameter(Symbol* method, infoType type, char* name, int value);

// Busca un simbolo por nombre en un solo nivel de alcance
Symbol* getSymbolInOneLevel(Level* currentLevel, char* name);

// Busca un simbolo por nombre en todos los niveles de alcance
Symbol* getSymbol(Stack* stack, char* name);

// Libera toda la memoria de la tabla de simbolos
void freeTS(Stack* stack);

// Imprime el contenido de la tabla de simbolos
void printTS(Stack* stack);

#endif // TS_H