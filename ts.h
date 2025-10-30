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

Stack* initializeTS();

void openNewLevel(Stack* stack);

void closeLevel(Stack* stack);

Symbol* insertSymbol(Stack* stack, flagType flag, infoType type, char* name, int value);

Symbol* insertParameter(Symbol* method, infoType type, char* name, int value);

Symbol* getSymbolInOneLevel(Level* currentLevel, char* name);

Symbol* getSymbol(Stack* stack, char* name);

void freeTS(Stack* stack);

void printTS(Stack* stack);

#endif // TS_H